#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <io.h>
#include "arp.h"
#include "packet.h"
#include "types.h"
#include "utils.h"
#include "tcp.h"
#include "tcpsockm.h"

#define BUFFER_SIZE 1024

// Function to check if a file exists
int file_exists(const char *path) {
    return access(path, 0) == 0;
}


// Check this flag once in a while to see if the user wants out.
volatile uint8_t CtrlBreakDetected = 0;

void __interrupt __far ctrlBreakHandler( ) {
  CtrlBreakDetected = 1;
}

void __interrupt __far ctrlCHandler( ) {
  // Do Nothing - Ctrl-C is a legal character
}


// Function to serve the file over HTTP
void serve_file(const char *file_path, uint16_t port) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        printf("Error: Could not open file %s\n", file_path);
        return;
    }

    printf("Serving file %s on port %u\n", file_path, port);


    // Parse mTCP configuration
    if (Utils::parseEnv() != 0) {
        printf("Error: Could not parse mTCP configuration\n");
        fclose(file);
        return;
    }

    // Initialize mTCP stack
    if ( Utils::initStack( 2, TCP_SOCKET_RING_SIZE, ctrlBreakHandler, ctrlCHandler ) ) {
        printf("Error: Could not initialize mTCP stack\n");
        fclose(file);
        return;
    }

    // Create a listening socket
    TcpSocket *listeningSocket = TcpSocketMgr::getSocket();
    if (!listeningSocket) {
        printf("Error: Could not create listening socket\n");
        fclose(file);
        Utils::endStack();
        return;
    }

    if (listeningSocket->listen(port, BUFFER_SIZE) != 0) {
        printf("Error: Could not bind to port %u\n", port);
        TcpSocketMgr::freeSocket(listeningSocket);
        fclose(file);
        Utils::endStack();
        return;
    }

    printf("Waiting for connections...\n");

    while (1) {
        PACKET_PROCESS_SINGLE;
        Arp::driveArp();
        Tcp::drivePackets();

        // Accept a new connection
        TcpSocket *clientSocket = TcpSocketMgr::accept();
        if (clientSocket) {
            printf("Client connected\n");

            // Send HTTP response headers
            const char *response_header = "HTTP/1.0 200 OK\r\n"
                                          "Content-Type: application/octet-stream\r\n"
                                          "Connection: close\r\n\r\n";
            clientSocket->send((uint8_t *)response_header, strlen(response_header));

            // Send file content
            char buffer[BUFFER_SIZE];
            size_t bytes_read;
            while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
                clientSocket->send((uint8_t *)buffer, bytes_read);
            }

            clientSocket->close();
            TcpSocketMgr::freeSocket(clientSocket);
            rewind(file); // Reset file pointer for the next client
        }

        if (biosIsKeyReady()) {
            char c = biosKeyRead();
            if (c == 27) { // ESC key to exit
                break;
            }
        }
    }

    listeningSocket->close();
    TcpSocketMgr::freeSocket(listeningSocket);
    fclose(file);
    Utils::endStack();
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: qrserve <file_path> <port>\n");
        return 1;
    }

    const char *file_path = argv[1];
    uint16_t port = atoi(argv[2]);

    // Validate the file
    if (!file_exists(file_path)) {
        printf("Error: File %s does not exist\n", file_path);
        return 1;
    }

    // Serve the file
    serve_file(file_path, port);

    return 0;
}

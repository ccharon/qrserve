# QR Serve


## Project setup
The Project needs to be build under a Windows OS, I did not get it to work under Linux. The reasons are the filenames (sources mix upper and lower case) and the compiler setup, I was not able to get version 1.9. of the open watcom compiler to run. Using version 2.0 got it running but the mTCP sources would not compile.

To build the project, ensure you have the following prerequisites:

1. **Watcom C/C++ Compiler**:
   - Install Open Watcom C/C++ version 1.9.
   - Ensure the `wcc`, `wpp`, and `wlink` tools are available in your system's PATH.

2. **Directory Structure**:
   - The project assumes the following directory structure:
     ```
     qrserve/
       ├── Makefile      # Build instructions
       ├── README.md     # Project documentation
       └── qrserve.cpp   # Sources
     mTCP-src/     # External mTCP sourcecode
     ```
     mTCP-src are the extracted sources of mTCP downloaded from 

3. **Building the Project**:
   - Run the following command to clean and build the project:
     ```
     wmake
     ```
   - The resulting executable (`qrserve.exe`) and other build artifacts will be placed in the project directory.

4. **Cleaning the Project**:
   - To clean up build artifacts, run:
     ```
     wmake clean
     ```

5. **Configuration**:
   - The project uses a configuration file named `qrserve.cfg`. Ensure this file is present in the working directory when running the executable.

Note to make `wmake patch` work the mTCP-src/UTILS makefile must be run. then I manually needed to copy PATCH.EXE to ptach.exe.

## References
- mbrutman mtcp
- 
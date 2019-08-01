# RDS
## Environment setup instructions:
- For all applications:
  - Install [Git](https://git-scm.com/).
  - Install [CMake](https://cmake.org/).
  - Install [Visual Studio](https://visualstudio.microsoft.com/downloads/).
  - Install [Intel Parallel Studio XE](https://software.intel.com/en-us/parallel-studio-xe). To get a serial number for free, you will have to create a user with your institution email.
  
  ![Parallel Studio XE packages seletion during installation](images/parallel_studio.png)

- Additional prerequisites for electron applications:
  - Install [NodeJS](https://nodejs.org).
  - Install [Visual Studio Code](https://code.visualstudio.com/).
  - Open command prompt, and run the following commands:
    - `npm install -g cmake-js`

## Build instructions:
 1. Clone repository recursively - `git clone --recursive https://github.com/HaifaGraphics/RDS.git`
 2. Generate Visual Studio project:
    - For native applications:
       1. Generate Visual Studio project using CMake GUI (Need to be done after a modification to a CMakeLists.txt file).
    - For electron applications:
      1. Open command prompt:
         1. Run `npm install` from the repository's root folder (need to be done only once).
         2. Run `cmake-js` from the repository's root folder (Need to be done after a modification to a CMakeLists.txt file).

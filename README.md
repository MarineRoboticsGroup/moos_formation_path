##############################################################################
# FILE:        moos-ivp-extend/README
# DATE:        2021/8/24
# DESCRIPTION: Contains important information regarding the moos_formation_path
#              repository.
##############################################################################

#=============================================================================
# Introduction
#=============================================================================
This repository contains examples for extending the MOOS-IvP
Autonomy system. This includes a MOOS application and an IvP behavior.


#=============================================================================
# Directory Structure
#=============================================================================
The directory structure for the moos-ivp-extend is decribed below:

bin              - Directory for generated executable files
build            - Directory for build object files
build.sh         - Script for building moos-ivp-extend
CMakeLists.txt   - CMake configuration file for the project
data             - Directory for storing data
lib              - Directory for generated library files
missions         - Directory for mission files
README           - Contains helpful information - (this file).
scripts          - Directory for script files
src              - Directory for source code
eigen-3.4.0      - Library used rigidity calculations
format_converters- Directory for any code that performs format conversion


#=============================================================================
# Build Instructions
#=============================================================================
#--------------------
# Linux and Mac Users
#--------------------

To build on Linux and Apple platforms, execute the build script within this
directory:

   $ ./build.sh

To build without using the supplied script, execute the following commands
within this directory:

   $ mkdir -p build
   $ cd build
   $ cmake ../
   $ make
   $ cd ..


#--------------
# Windows Users
#--------------
To build on Windows platform, open CMake using your favorite shortcut. Then 
set the source directory to be this directory and set the build directory
to the "build" directory inside this directory.

The source directory is typically next to the question:
   "Where is the source code?"

The build directory is typically next to the question:
   "Where to build the binaries?"

Alternatively, CMake can be invoked via the command line. However, you must
specify your gernerator. Use "cmake --help" for a list of generators and
additional help.

#=============================================================================
# Eigen-3.4.0
#=============================================================================
In order to add Eigen to the compiler's path, run the following command:
$ sudo ln -s /usr/local/include ~[YOUR PATH TO THIS REPOSITORY]/moos_formation_path/eigen-3.4.0/Eigen
And also add the following line to ~/.bashrc and source before proceeding:
export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH$ :~/[YOUR PATH TO THIS REPOSITORY]/moos_formation_path/eigen-3.4.0

##############################################################################
#                               END of README
##############################################################################


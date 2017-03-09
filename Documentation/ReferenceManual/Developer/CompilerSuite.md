# Installing a Compiler Suite # {#compiler_suite}

## Minimum Supported Compiler Versions ##

DREAM.3D is written in C\++ and the developers utilize features from the C\+\+11 specification so therefor
any C\+\+ compiler that is used **MUST** be C\+\+11 compliant.

A compiler suite (Visual Studio, Xcode, GCC, etc.) for your operating system is necessary. 
The following table lists the compilers that DREAM.3D is commonly compiled against.

## 32 Bit Support ##

DREAM.3D is compiled as a 64 bit application and libraries on all operating systems. 32 Bit support
has been removed and is no longer under active development. In fact assumptions are made during the development process that the application is running as a 64 bit application.

## Windows 8/10 ##

| Compiler Version | Notes |
| ---------------- | ----- |
| Visual Studio 2013 Pro & Community SP5 | DREAM3D SDK **ONLY** supports this version |
| Visual Studio 2015 Pro & Community | Developer will need to compile all dependent libraries |


## OS X ##

Mac OS X 10.10 (Yosemite), 10.11 (El Capitan) are supported

| Compiler Version | Notes |
| ---------------- | ----- |
| Xcode 7.2 | OS X 10.10 |
| Xcode 7.3 | OS X 10.11 |

## Linux ##

There are many Linux distributions in use. Below is the list of the minimum compiler versions that are known to work on linux. DREAM.3D is commonly built on Ubuntu 14.04 LTS with Clang 3.8.

| Compiler Version | Notes |
| ---------------- | ----- |
| GCC 4.8.x |  |
| Clang 3.8 | Part of the LLVM 3.8 package. |


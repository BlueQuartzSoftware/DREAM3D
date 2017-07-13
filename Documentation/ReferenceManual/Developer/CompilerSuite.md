# Installing a Compiler Suite # {#compiler_suite}

## Minimum Supported Compiler Versions ##

DREAM.3D is written in C\++ and the developers utilize features from the C\+\+11 specification.
Therefore, any C\+\+ compiler that is used **MUST** be C\+\+11 compliant.

A compiler suite (Visual Studio, Xcode, GCC, etc.) for your operating system is necessary. 
The following table lists the compilers that DREAM.3D is commonly compiled against.

Find your current OS and download the appropriate compiler.

## Windows 8/10 ##

| Compiler Version | Notes |
| ---------------- | ----- |
| Visual Studio 2015 Pro & Community | Developer will need to compile all dependent libraries |


## OS X ##

Mac OS X 10.10 (Yosemite), 10.11 (El Capitan), and 10.12 (Sierra) are supported

| Compiler Version | Notes |
| ---------------- | ----- |
| Xcode 7.2 | OS X 10.10 |
| Xcode 7.3 | OS X 10.11 |
| Xcode 8.2 | OS X 10.12 |

## Linux ##

There are many Linux distributions in use. Below is the list of the minimum compiler versions that are known to work on linux. DREAM.3D is commonly built on Ubuntu 14.04 LTS with GCC 4.8.5

| Compiler Version | Notes |
| ---------------- | ----- |
| GCC 4.8.5 | Minimum working version of GCC |
| Clang 3.8 | Part of the LLVM 3.8 package. |

## Enterprise Linux Distributions ##

+ DREAM.3D will *NOT* compile or run on SUSE Enterprise Linux 11 SP4.
+ DREAM.3D will *NOT* compile or run on Red Hat Enterprise Linux 6.x


## 32 Bit Support ##

DREAM.3D and its libraries are compiled as a 64 bit application on all operating systems.

32-bit support has been removed and is no longer under active development. Assumptions are made during the development process that the application is running as a 64 bit application.

---
**Next Page (Windows)**: <a href="https://github.com/bluequartzsoftware/DREAM3DSuperbuild/blob/develop/docs/Making_an_SDK_Windows.md">Making an SDK</a>

**Next Page (OS X)**: <a href="https://github.com/bluequartzsoftware/DREAM3DSuperbuild/blob/develop/docs/Making_an_SDK_OSX.md">Making an SDK</a>

**Next Page (Linux)**: <a href="https://github.com/bluequartzsoftware/DREAM3DSuperbuild/blob/develop/docs/Making_an_SDK_Linux.md">Making an SDK</a>
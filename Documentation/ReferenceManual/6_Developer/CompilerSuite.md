# Installing a Compiler Suite #

## Minimum Supported Compiler Versions ##

DREAM.3D is written in C\++ and the developers utilize features from the C\+\+17 specification.
Therefore, any C\+\+ compiler that is used **MUST** be C\+\+17 compliant.

A compiler suite (Visual Studio, Xcode, GCC, etc.) for your operating system is necessary.
The following table lists the compilers that DREAM.3D is commonly compiled against.

Find your current OS and download the appropriate compiler.

## Windows ##

| Product | Product Version | Compiler Version | MSVC++ Toolset |
| ------- | --------------- | ---------------- | -------------- |
| Visual Studio 2017 Pro & Community | 15.8 | 19.15 | 14.15 |

## OS X ##

Mac OS X 10.15 (Catalina) is supported.

| Compiler Version | OS X Version |
| ---------------- | ------------ |
| Xcode 11 | OS X 10.15 |

## Linux ##

There are many Linux distributions in use. Below is the list of the minimum compiler versions that are known to work on Linux.

| Compiler Version |
| ---------------- |
| GCC 8 |
| Clang 7 |

## Enterprise Linux Distributions ##

+ DREAM.3D will *NOT* compile or run on SUSE Enterprise Linux 11 SP4.
+ DREAM.3D will *NOT* compile or run on Red Hat Enterprise Linux 6.x

## 32 Bit Support ##

DREAM.3D and its libraries are compiled as a 64 bit application on all operating systems.

32-bit support has been removed and is no longer under *ANY* development. Assumptions are made during the development process that the application is running as a 64 bit application.

---
**Next Page (Windows)**: <a href="https://github.com/bluequartzsoftware/DREAM3DSuperbuild/blob/develop/docs/Making_an_SDK_Windows.md">Making an SDK</a>

**Next Page (OS X)**: <a href="https://github.com/bluequartzsoftware/DREAM3DSuperbuild/blob/develop/docs/Making_an_SDK_OSX.md">Making an SDK</a>

**Next Page (Linux)**: <a href="https://github.com/bluequartzsoftware/DREAM3DSuperbuild/blob/develop/docs/Making_an_SDK_Linux.md">Making an SDK</a>
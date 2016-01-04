Versioning {#versioning}
========

The DREAM.3D version scheme is a set of 4 values. The first 3 values are integers while the last value is the first part of a SHA1 hash. An example is the following:

    6.1.3.as34fd

The scheme is [major].[minor].[patch].[revision]

# What Do Version Changes Indicate? #

## Major ##

Major user interface changes, file compatibility changes, or major API/ABI changes.

## Minor ##
 
Minor API/ABI changes requiring recompiling of **Plugins**. Minor features added to user interface. No file compatibility changes.

## Patch ##

Bugs fixed in any area, such as **Filters**, user interface, **Plugins**, APIs, documentation fixes and updates, etc. The patch version is calculated based on a git tag for the last minor release. This should **NOT** require other **Plugins** to be recompiled.

## Revision ##

The revision is the git SHA1 hash for the commit that the release was based on. This string is always generated for the DREAM.3D project by the build system.

# CMake and Version #

CMake version 3.1 introduced _project_ version variables. These variables are used by introducing a VERSION argument with [major].[minor]. Every _project_ command in any CMake file should have this _VERSION_ argument.

    project(DREAM3DProj VERSION 5.6)
    project(CoolPlugin VERSION 3.4)

If projects use the infrastructure codes that are provided with the DREAM.3D project, the git patch and revision numbers should be generated automatically. Otherwise, if git is not found, then a default set of versions will be generated. 

# Releasing DREAM.3D with Proper Version #

Ensure **EVERY** piece of DREAM.3D compiles correctly, passes all tests, and generates packages correctly. After all of these tests pass **THEN** tag the repository with the version (given as "V[major].[minor]").


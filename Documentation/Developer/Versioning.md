# DREAM3D Version Scheme #

The DREAM3D version scheme is a set of 4 values, the first 3 are integers, the last is the first part of a SHA1 hash. An example is the following:

    6.1.3.as34fd

The scheme is [major].[minor].[patch].[revision]

## When to Change the version numbers ##

### Major ###

 Major UI interface changes, File compatibility changes, major API/ABI changes

### Minor ###
 
Minor API/ABI changes requiring recompiling of plugins. Minor features added to user interface. No breaking file compatibility changes.

### Patch ###

Bugs fixed in any area: Filters, User Interface, plugins, APIs, documentation fixes/updates. The patch version is calculated based on a Git tag for the last minor release. This should **NOT** require other plugins to be recompiled. They should be drop in replacements.

### Revision ###

The revision is the Git SHA1 hash for the commit that the release was based on. This is always generated for the DREAM3D project by the build system.


## CMake and Version ##

CMake version 3.1 introduced _project_ version variables. This is done by introducing a VERSION argument with [major].[minor]. Every _project_ command in any CMake file should have this _VERSION_ argument including Plugins.

    project(DREAM3DProj VERSION 5.6)
    project(CoolPlugin VERSION 3.4)

If projects use the infrastructure codes that are provided with the DREAM3D project then the git patch & revision numbers should be generated automatically. Otherwise if git is not found then a default set of versions will be generated. 


## Releasing DREAM3D with Proper Version ##

Ensure **EVERY** piece of DREAM3D compiles correctly, passes all tests, and generates packages correctly. After all of these tests pass **THEN** tag the repository with the version (given as "V[major].[minor]"). Now do one last clean pull and build (Have the build bots do this).


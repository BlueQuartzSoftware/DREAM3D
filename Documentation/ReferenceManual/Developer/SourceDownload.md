# Downloading the Source Code for DREAM.3D # {#source_download}

DREAM.3D is publically available on Github, and can be downloaded using [Git](http://www.git-scm.org). The user is strongly encouraged to use a GUI based Git tool such as [SourceTree](http://www.sourcetreeapp.com) in order to manage cloning all four (4) source archives.

## Mac OS X ##
Open Terminal at /Applications/Utilities/Terminal.app.

## Windows 8.1/10 ##
Open the git-bash application that was installed when you downloaded and installed Git.

## Linux ##

## Download the 4 Source Archives ##
Use git to clone the DREAM.3D repository:

    git clone -b develop https://www.github.com/bluequartzsoftware/DREAM3D

In addition there are 3 other repositories that are required to build DREAM.3D:

    git clone -b develop https://www.github.com/bluequartzsoftware/CMP
    git clone -b develop https://www.github.com/bluequartzsoftware/SIMPL
    git clone -b develop https://www.github.com/bluequartzsoftware/SIMPLView

## Directory Layout ##


All 4 of these repositories should be cloned/downloaded into the same folder so that the final folder structure looks like the following:

    CMP
    DREAM3D
    SIMPL
    SIMPLView

---
**Next Page**: [Installing the DREAM.3D SDK](@ref sdk_install).

**Previous Page**: [Installing a Compiler Suite](@ref compiler_suite).
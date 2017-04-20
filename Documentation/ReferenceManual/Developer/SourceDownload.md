# Downloading the Source Code for DREAM.3D # {#source_download}
<a name="prerequisites">
## Prerequisites ##
</a>

These prerequisites need to be completed before the source code for DREAM.3D is downloaded.

If you have already fulfilled all of these prerequisites, skip to the [Downloading the Source Code](#downloading_src_code) section.

<a name="compiler_suite">
### Install a Compiler Suite ###
</a>

A compatible compiler needs to be installed on your system to be able to build DREAM.3D.

For more information, please visit [Installing a Compiler Suite](@ref compiler_suite).

<a name="downloading_src_code">
## Downloading the Source Code ##
</a>

DREAM.3D is publicly available on Github, and can be downloaded using [Git](http://www.git-scm.org). The user is strongly encouraged to use a GUI based Git tool such as [SourceTree](http://www.sourcetreeapp.com) in order to manage cloning all four (4) source archives.

#### Mac OS X ####
1. Make a new directory called **Workspace** in your home directory.  The **Workspace** directory's path should be **/Users/[*YOUR-HOME-DIRECTORY*]/Workspace**.

2. Open Terminal at /Applications/Utilities/Terminal.app.

3. Navigate to your Workspace directory at **/Users/[*YOUR-HOME-DIRECTORY*]/Workspace**.

4. Use git to clone the DREAM.3D repository to your **Workspace** directory:

    `git clone -b develop https://www.github.com/bluequartzsoftware/DREAM3D`

	Clone 3 other repositories to your **Workspace** directory that are required to build DREAM.3D:

    `git clone -b develop https://www.github.com/bluequartzsoftware/CMP`
    `git clone -b develop https://www.github.com/bluequartzsoftware/SIMPL`
    `git clone -b develop https://www.github.com/bluequartzsoftware/SIMPLView`

	All 4 of these repositories should be cloned/downloaded into the **Workspace** folder so that the final paths to all 4 repositories look like the following:

	/Users/[*YOUR-HOME-DIRECTORY*]/Workspace/DREAM3D
    
    /Users/[*YOUR-HOME-DIRECTORY*]/Workspace/CMP
    
    /Users/[*YOUR-HOME-DIRECTORY*]/Workspace/SIMPL
    
    /Users/[*YOUR-HOME-DIRECTORY*]/Workspace/SIMPLView

#### Windows 8.1/10 ####
1. Make a new directory called **Workspace** in your user directory.  The **Workspace** directory's path should be **C:\\Users\\[*YOUR-USER-DIRECTORY*]\\Workspace**.

2. Open the git-bash application that was installed when you downloaded and installed Git.

3. Navigate to your Workspace directory at **C:\\Users\\[*YOUR-USER-DIRECTORY*]\\Workspace**.

4. Use git to clone the DREAM.3D repository to your **Workspace** directory:

    `git clone -b develop https://www.github.com/bluequartzsoftware/DREAM3D`

	Clone 3 other repositories to your **Workspace** directory that are required to build DREAM.3D:

    `git clone -b develop https://www.github.com/bluequartzsoftware/CMP`
    `git clone -b develop https://www.github.com/bluequartzsoftware/SIMPL`
    `git clone -b develop https://www.github.com/bluequartzsoftware/SIMPLView`

	All 4 of these repositories should be cloned/downloaded into the **Workspace** folder so that the final paths to all 4 repositories look like the following:

    C:\\Users\\[*YOUR-USER-DIRECTORY*]\\Workspace\\DREAM3D
    
    C:\\Users\\[*YOUR-USER-DIRECTORY*]\\Workspace\\CMP
    
    C:\\Users\\[*YOUR-USER-DIRECTORY*]\\Workspace\\SIMPL
    
    C:\\Users\\[*YOUR-USER-DIRECTORY*]\\Workspace\\SIMPLView

#### Linux ####
1. Make a new directory called **Workspace** in your home directory.  The **Workspace** directory's path should be **/home/[*YOUR-HOME-DIRECTORY*]/Workspace**.

2. Open the Terminal program.

3. Navigate to your Workspace directory at **/home/[*YOUR-HOME-DIRECTORY*]/Workspace**.

4. Use git to clone the DREAM.3D repository to your **Workspace** directory:

    `git clone -b develop https://www.github.com/bluequartzsoftware/DREAM3D`

	Clone 3 other repositories to your **Workspace** directory that are required to build DREAM.3D:

    `git clone -b develop https://www.github.com/bluequartzsoftware/CMP`
    `git clone -b develop https://www.github.com/bluequartzsoftware/SIMPL`
    `git clone -b develop https://www.github.com/bluequartzsoftware/SIMPLView`

	All 4 of these repositories should be cloned/downloaded into the **Workspace** folder so that the final paths to all 4 repositories look like the following:

	/home/[*YOUR-HOME-DIRECTORY*]/Workspace/DREAM3D
    
    /home/[*YOUR-HOME-DIRECTORY*]/Workspace/CMP
    
    /home/[*YOUR-HOME-DIRECTORY*]/Workspace/SIMPL
    
    /home/[*YOUR-HOME-DIRECTORY*]/Workspace/SIMPLView

---
**Next Page**: [Installing the DREAM.3D SDK](@ref sdk_install).

**Previous Page**: [Installing a Compiler Suite](@ref compiler_suite).

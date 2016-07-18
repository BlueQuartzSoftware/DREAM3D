# Download the source code for DREAM.3D # {#source_download}

The development of DREAM.3D is done "in the open" using public [git](http://www.git-scm.org) servers and is best accessed using [git](http://www.git-scm.org). The user is strongly encouraged to use a GUI based Git tool such as [SourceTree](http://www.sourcetreeapp.com) in order to manage cloning all four (4) source archives.



The source for DREAM.3D is located at [http://www.github.com/bluequartzsoftware/dream3d](http://www.github.com/bluequartzsoftware/dream3d).

You do not need an account at github to download it but it does help if you are going to be serious about fixing bugs in DREAM.3D and want to contribute those fixes back to the DREAM.3D developers through merge requests.

## I have Git Installed on My System ##

Use git clone the DREAM.3D repository:

	git clone -b develop https://www.github.com/bluequartzsoftware/dream3d

In addition there are 3 other repositories that are required to build DREAM.3D:

	git clone -b develop https://www.github.com/bluequartzsoftware/CMP
    git clone -b develop https://www.github.com/bluequartzsoftware/SIMPL
    git clone -b develop https://www.github.com/bluequartzsoftware/SIMPLView


## I do NOT have GIt Installed on My System ##

The user will need to download 4 zip archives directly from each of the project web pages:

[https://www.github.com/bluequartzsoftware/CMP](https://www.github.com/bluequartzsoftware/CMP)
[https://www.github.com/bluequartzsoftware/DREAM3D](https://www.github.com/bluequartzsoftware/DREAM3D)
[https://www.github.com/bluequartzsoftware/SIMPL](https://www.github.com/bluequartzsoftware/SIMPL)
[https://www.github.com/bluequartzsoftware/SIMPLView](https://www.github.com/bluequartzsoftware/SIMPLView)

The following images shows what the user interface should look like:

![images/download_source_image1.png](images/download_source_image1.png)


## Directory Layout ##


All 4 of these repositories should be cloned/downloaded so that the final folder structure looks like the following:

	CMP
    DREAM3D
    SIMPL
    SIMPLView

Once the above directory structure is obtained the developer can proceed to the next steps of installing or compiling all the DREAM.3D dependent libraries.

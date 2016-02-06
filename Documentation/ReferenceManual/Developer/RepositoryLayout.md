# Repository Layout #

The DREAM.3D project is organized into three separate repositories. These repositories are hosted at [GitHub.com/bluequartzsoftware](http://www.github.com/bluequartzsoftware). The repositories are the following:

+ CMP
+ SIMPL
+ SIMPLView
+ DREAM3D

As we discuss the various repositories, we are going to start from the very bottom and work our way up to the top.

### CMP ###
 This repository holds commonly used CMake codes that are shared among the various projects and repositories. This is the lowest level repository.

---

### SIMPL Repo ###

The SIMPL repo holds the base SIMPLib and H5Support libraries that are the foundation of all the other projects. SIMPL allows the developer to utilize the "DREAM.3D" file format, read and write DREAM.3D files, create **DataContainers**, **AttributeMatrix**, **DataArrays**, and **Geometries**. In addition SIMPLib also has the plugin loading mechanism which allows the developer to create external plugins of their own specialized filters. In order to execute custom pipelines the SIMPL repo also includes the **PipelineRunner** application which is a command line driven program that can run pipelines based on the input from a pipeline file. In the figure below the yellow shapes represent libraries and applications built by the SIMPL repository where-as the purple colored shapes represent dependent libraries.

![SIMPL Repository](images/SIMPL_Repo.png)

---

## SIMPLView Repo ##

The **SIMPLView** repo holds the next higher level project. The SIMPLView project is the basic Pipeline viewer application that can load, edit, save and execute a pipeline (with the assumption that all necessary plugins and libraries are loaded). The application is [Qt](http://www.qt.io) based and presents the user with a window that allows the ordering of the filters into a pipeline. The user can then execute the pipeline. 

![SIMPLView Application](images/SIMPLView_1.png)

In the figure below the red shapes represent libraries and applications that are built by the SIMPLView project. Purple shapes represent additional dependencies that are needed for the compile.

![SIMPLView Repository](images/SIMPLView_Repo.png)

The project also includes an application called "DevHelper" that can assist a developer in creating the skeleton code for an entire plugin or just a new filter within an existing plugin.

---

## DREAM3D Repo ##

The DREAM.3D repo holds the highest level project which brings together specific *branding* of the SIMPLView application and a set of plugins that are specific to processing 3D microstructure data or synthetically generating 3D microstructure data. In this project the developers have used a scheme where new sets of icons, images and splash screen images have been inserted into the SIMPLView build that lead to a newly named "DREAM3D" being created. This application has over 200 filters that can be used to import, export and process various types of data. 

![DREAM3D Application](images/DREAM3D_1.png)

In the figure below the blue shapes represent libraries and programs built by the DREAM.3D project. Purple shapes are dependent libraries and the greenish shapes are plugins that are included in the DREAM.3D project.

![DREAM3D Repository](images/DREAM3D_Repo.png)


---

## All Repositories ##

The figure below shows graphically the how each repository is related to the next lower level repository.

![All Repositories](images/All_Repos.png)
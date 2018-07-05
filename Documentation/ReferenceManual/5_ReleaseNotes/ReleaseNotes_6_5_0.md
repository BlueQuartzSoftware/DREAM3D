# Version 6.5 Release Notes #

These release notes summarize the major changes for each officially released version of DREAM.3D. The official bug report is located at the [DREAM.3D GitHub issues page](https://github.com/bluequartzsoftware/DREAM3D/issues).

## Getting Help ##

The developers of DREAM.3D maintain a pair of Google Groups for discussions on the use of DREAM.3D and the development of filters or plugins for DREAM.3D. If you have a question those are the best places to ask and get help from the community.

[DREAM.3D Users List](https://groups.google.com/forum/?/dream3d-users#!forum/dream3d-users)

[DREAM.3D Developers List](https://groups.google.com/forum/?hl=en#!forum/dream3d-developers)

## Version 6.5.10 ##

+ Fix a hard crash inside of the DataContainerReader that would prevent the use of the filter within the GUI environment.


## Version 6.5.9 ##

+ Improvements to handling of relative file paths on macOS. There is now a default "DREAM3D Data" directory that is used which is initially set to a directory called "DREAM3DData" and is placed inside the users home directory

## Version 6.5.3 ##

### New Filters & Features ###

+ Implemented a custom style for the BrandedDREAM3D folder where any intrepid developer can use any Qt5 supported CSS (Cascading Style Sheet) codes to completely customize the look of DREAM.3D/SIMPLView.
+ DREAM3D Issue #752: Add filter to reduce orientations to their fundamental zone.
+ Add ability to create discrete pole figures instead of Lambert based pole figures.
+ Pole Figures are now generated as PDF files using libHaru
+ Add new filter that allows the user to summarize a pipeline by entering text into a basic text edit widget
+ Add filter to convert from 3 Component Rodrigues vectors to 4 component
+ Add filter to compute the conjugate of a Quaternioni
+ Add filter to compute the transpose of an Orientation Matrix
+ Updated the organization of the Prebuilt pipelines.

### Fixed DREAM3D Issues ###

+ Add cancel checks for the EMsoftSO3Sampler filter
+ Add information parameter that shows the size of the image geometry in real length units.
+ DREAM3D Issue #755: Implement a read only preflight updated value that states the range and delta for each axis in synthetic volume. This allows better feedback to the user that they are hitting specific size targets that they have in mind.
+ DREAM3D Issue #756: Figure out how to remove the JQuery.js file that Doxygen includes.
+ DREAM3D Issue #758: CMake code in the plugins are not generating any unit test code
+ DREAM3D Issue #760: DREAM.3D will crash when adjusting the RDF input values of the StatsGenerator filter
+ DREAM3D Issue #762: SPParks Dump File Importer does not properly calculate the bounds internally in the code when the BOX BOUNDS header is an integer.
+ DREAM3D Issue #764: Add warning to Alignment filters to warn user if any shift is greater than the dimension of the shift
+ DREAM3D Issue #765: Add a minimize button to the Help window.
+ DREAM3D Issue #766: xdmf file name is truncated at the first "." character instead of the last
+ DREAM3D Issue #767: Parallelize the AlignSections and NeighborOreintationCorrelation filters where the data is being copied around
+ DREAM3D Issue #773: Dream3D crashes when generating ensemble statistics 
+ DREAM3D Issue #774: Parallelize Convert Orientation Representation
+ DREAM3D Issue #775: Segment Features by Misorientation takes too much time reallocating a std::vector. Investigate and fix
+ DREAM3D Issue #777: Qt 5.10.x & StatsGenerator Progress windows stays visible after StatsGenerator has loaded 
+ DREAM3D Issue #780: Help Window can not be resized.
+ DREAM3D Issue #781: Help Window stops loading help files?
+ DREAM3D Issue #782: Combine Attribute Arrays should have an option to copy or move the data
+ DREAM3D Issue #783: Identify Sample Documentation needs example image when to to use the filter 
+ DREAM3D Issue #784: Crop Volume (Image) needs to have a cancel check in the inner cropping loop
+ DREAM3D Issue #789: Calculate the Sphericity of each feature
+ DREAM3D Issue #794: Parallelize the FindNeighborhoods filter.ß
+ Enable Tetragonal High, Tetragonal Low, Trigonal High & Trigonal Low Laue Group for Pole Figure generation
+ Fix a regression where incorrect static_cast<> would cause black pole figures.
+ Fix memory leak in the LaueOps classes when multi-threading.
+ Fixing bug where choosing the Precipitate Rolled Preset in StatsGenerator would not display a dialog box.
+ Remove the IPF Legend filter due to use of QtGui. Insert into its place static 2kx2k pre-rendered images in the data folder

### Fixed SIMPLView Issues ###

+ SIMPLView Issue #29: Updated the Show / Hide on Error preference values
+ SIMPLView Issue #33: Filter Parameter Input widget should add the pipeline index to the title
+ SIMPLView Issue #34: Add more information to the Image Geoemtry InfoString

### Fixed SIMPL Issues ###

+ Add new filter that allows the user to Summarize a pipeline
+ Added UUID to every filter. Loading pipelines now prefers to use the UUID, then falls back to the C++ classname
+ Clean up Memory leak in IssuesWidget due to no delete on the member pointer variable that represents the UI.
+ Convert the PipelineView Widget into a Dockable Widget and subclass from QListView
+ Fix GenerateColorTableTest unit test that was failing
+ Fix ImportAsciDataArray failing when reading boolean data from a file.
+ Fix issue where filters in pipelines that have 10 or more filters would not be read correctly and interpreted as unknown filters.
+ Fix many issues with over flows of int32 values and other crashes within the RadialDistribution Function class
+ Fix memory leak in ImportHDF5Dataset
+ ImageGeom & RectGridGeom: Fix crash issue where the number of Z Dimensions need to be greater than the number of processors.
+ Modernizing the CMake codes to use target specific properties where possible instead of global properties
+ Removing SafePointerDowncast and other old school C++ codes and updating to newer C++ 11/14 standards instead.
+ SIMPL Issue #29: Updating ReadASCIIData to allow the filter to read strings.
+ SIMPL Issue #141: Add information parameter that shows the size of the image geometry in real length units.
+ SIMPL Issue #144: Create a "Pipeline Summary" filter
+ SIMPL Issue #145: Fixing issue where the Cell Attribute Matrix was not written out for Rectilinear Grids
+ SIMPL Issue #147: Fix bug where the index was left off a pointer comparison.
+ SIMPL Issue #154: ImageGeom header throws warnings
+ SIMPL Issue #158: SIMPLib Unit test template file does not use the correct CMake variable
+ SIMPL Issue #164: Importing a .dream3d file does not select all DataArrays for importing
+ SIMPL Issue #167: Update const qualifiers on AbstractFilter for those methods that will not change the object instance
+ SIMPL Issue #168: CMake variable does not pass Windows install path requirements
+ SIMPL Issue #170: H5StatsDelegateReader does not properly read the Name of the StatsArray
+ SIMPL Issue #172: xdmf file name is truncated at the first "." character instead of the last
+ SIMPL Issue #176: Remove SITKExplicit library from SIMPL and Create ITKBridge Library instead.
+ SIMPL Issue #181: Help Window can not be resized. 
+ SIMPL Issue #186: Add more information to the Image Geoemtry InfoString
+ SIMPL Issue #187: CreateGeometry and CreateImageGeometry should have better tooltips
+ SIMPL Issue #188: When searching for a filter, allow searching of the ClassName, HumanLabel, Plugin Name, Branding Label
+ Update all filter documentation with section stating which Example Pipelines the filter appears in

# Version 6.5 Release Notes #

These release notes summarize the major changes for each officially released version of DREAM.3D. The official bug report is located at the [DREAM.3D GitHub issues page](https://github.com/bluequartzsoftware/DREAM3D/issues).

## Getting Help ##

The developers of DREAM.3D maintain a pair of Google Groups for discussions on the use of DREAM.3D and the development of filters or plugins for DREAM.3D. If you have a question those are the best places to ask and get help from the community.

[DREAM.3D Users List](https://groups.google.com/forum/?/dream3d-users#!forum/dream3d-users)

[DREAM.3D Developers List](https://groups.google.com/forum/?hl=en#!forum/dream3d-developers)

## Version 6.5.83 ##

### New Filters & Features ###

### Fixed DREAM3D Issues ###

+ Improvements to StatsGenerator Documentation and Axis ODF Widget. (#824)
+ Fix incorrect labeling on pole figures for Hexagonal High Laue Class (#826)
+ Add example pipeline that produces a cylindrical shaped synthetic microstructure (#828)
+ Fix regression in installation codes where some data directories were left out. (#829)
+ Update dataCheck() to use a standardized output file checking mechanism. (#833)
+ Add second strategy for parallel implementation of SampleSurfaceMesh (#835)
+ Update all classes to properly implement constructors and assignment operators. (#837)
+ Fix calculation of centroid of the voxel. (#841)
+ Allow some filters to set specific DataArrays t0 ignore during the tuple copy operations
+ Calculate GBCD: Fix crashing bug due to use of -1 as a pointer offset. (#845)
+ Multiple entries in the AxisODF table were not deserialized correctly. (#846)
+ Additional fixes to QuickSurfaceMesh to address triangle winding issues. (#847)

### Fixed SIMPLView Issues ###

+ Fix issue in generated Filter that would cause compile errors on MSVC due to lack of EXPORT macro
+ Use distinct output name for the GUI plugin. Not doing so causes issues with debugging on MSVC
+ Fix bad CMake code generation in the Python wrapping code. Incorrect template filename was used.
+ Update codes to modern C++ standards using clang-tidy

### Fixed SIMPL Issues ###

+ LINUX: Properly move up a directory so that relative paths work correctly
+ Use the SVDialog as a superclass so that more GUI classes pick up the stying
+ Redo the Plugin Information UI to be more streamlined.
+ Improve Styling of Table widgets, improve AboutPlugins UX (#235)
+ Update TemplateHelpers implementations. (#237)
+ Removing pixmap that was being saved when creating an SVSpinBox.
+ Fix compile error on macOS 10.14 and Qt 5.11.1 (#247)
+ Fixes assert in setTuple in DataArray.hpp (#244)
+ Made FeatureDataCSVWriter use the new delimiter enum
+ Adds HexahedralGeom to CreateGeometry filter
+ Fix bugs in the SplitAttributeArrayTest and SplitAttributeArray filter (#251)
+ Fixing bug where changing "Start Import at Row" resets the data types in the Import ASCII Data wizard. (#254)
+ Allow wrapping of the file path in the Import ASCII Data Widget. (#258)
+ Create more verbose error message if parsing Json Pipeline fails. (#257)
+ Enable support for using the HDF5 1.10.3 library (#266)
+ Fix issue when reading a JSON file with an extended suffix.
+ Remove any colon characters that end up in a file path. (#271)
+ Update codes to modern C++ standards using clang-tidy

## Version 6.5.47 ##

### New Filters & Features ###

+ ITKImageProcessing::ITKImageWriter gained the ability to write out various orthogonal planes from the image geometry.

### Fixed DREAM3D Issues ###

+ Update documentation for StatsGenerator to reflect Neighbor Distribution generation
+ Removed Write IPF Standard Triangle from available filters. Static images are available in the Data folder
+ Issue 817: Correctly calculate the northern hemisphere XYZ coord from the southern hemisphere
+ Issue 819: Allow users to pick either a Lambert or Discrete pole figure for the ODF tab in StatsGenerator

### Fixed SIMPLView Issues ###

+ Change from 'Clear Cache' to 'Reset Preferences' which is more descriptive
+ Window settings are saved each time any dock widgets are moved or updated
+ Dock widgets with Tabs are now located at the top instead of the bottom.

### Fixed SIMPL Issues ###

+ Fix issue in MultiAttributeMatrixSelectionWidget where the selections would not deserialize from a pipeline file correctly.
+ Issue #228: Fix issue where the contents of the QTextEdit would not be saved to the filter in the Pipeline Annotation Filter
+ Issue #233: Fix bug where disabled filters are not written to the pipeline files.
+ Merge Coordinate transform system into mainline develop branch.
+ Adding a sanity check when loading bookmarks to make sure that all bookmark items have the proper item type.
+ Added unit test for SplitAttributeArray filter
+ Improves the output location of files related to Python Wrapping when used with Visual Studio.

## Version 6.5.36 ##

### New Filters & Features ###

No new features or filters were added. This bug fix is mainly a bug fix release based on feedback from users.

### Fixed DREAM3D Issues ###

+ Pack Primary Phases and Insert Precipitates now use "Feature Generation" filter parameter instead of "Alread Have Features"
+ Issue #809: changed the Feature Attribute Array Default Name from "AxisLengths" to "SemiAxisLengths," with other consistency changes (J. Tucker)
+ Issue #808: Adjust the min height of vertical tabs for some visual styles.
+ Issue #803: FindSizes overflow on large data
+ Issue #799: Allow FeatureInfoReader to accept comments in the file
+ Update prebuild pipelines that use the PackPrimaryPhases filter
+ Remove 'Input Stats File' for the estimation of number of features
+ Update documentation for PackPrimaryPhases (A.D. Rollett)
+ Update the Reference Frame documentation with edits from M.D. Graef.
+ Promote the "Detect Ellipsoids" filter into the Processing plugin

### Fixed SIMPLView Issues ###

+ Updating the text in the dialog box that appears when the SIMPL_RELATIVE_PATH_CHECK CMake variable is On and no data directory path has been set previously.  The dialog text is now consistent with the menu system text when telling users how to set a different data directory location.

### Fixed SIMPL Issues ###

+ Import HDF5 Dataset: Now import multiple datasets per filter. Better guessing for the component dimensions
+ Fix bug where filter parameters were shared among multiple filter instances in certain situations.
+ Fixed a bug where moving a filter would incorrectly prevent the filter from being deleted, thus causing the move to behave like a copy and place the same filter into the pipeline multiple times.
+ Fixed a bug in PipelineListWidget where the pipeline could be run even with errors if they were caused during pipeline execution. Updated PipelineListWidget to use an enum to check and change its current state rather than comparing text output to the UI.
+ Disabled the option of reading FilterPipelines from .dream3d files.  If a .dream3d file is dropped into the pipeline view, a Read DREAM.3D Data File filter is automatically created for the file.
+ Fixed a bug where pressing cancel when asked if the user wants to import a .dream3d file as a pipeline or as an import data filter would not cancel the import process.
+ Issue #215: Fixed
+ Issue #212: Add a Hexahedral geometry class.
+ Promote several filters from DREAM3DReview into SIMPL/Core: Copy Object, Create Geometry, Crop Vertex Geometry, Extract Attribute Arrays from Geometry, Split Attribute Array

## Version 6.5.10 ##

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

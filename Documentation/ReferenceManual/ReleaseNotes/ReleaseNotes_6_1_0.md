Version 6.1.0 Release Notes {#releasenotes_6_1_0}
===========


These release notes summarize the major changes for each officially released version of DREAM.3D. The official bug report is located at [https://github.com/DREAM3D/DREAM3D/issues](https://github.com/DREAM3D/DREAM3D/issues).

## Getting Help ##

Users:[https://groups.google.com/forum/?/dream3d-users#!forum/dream3d-users](https://groups.google.com/forum/?/dream3d-users#!forum/dream3d-users)

Developers:[https://groups.google.com/forum/?hl=en#!forum/dream3d-developers](https://groups.google.com/forum/?hl=en#!forum/dream3d-developers)

## Version 6.1. ##
+ Added feature where Attribute Matrix and Attribute Array combo boxes can be filtered out by the type of object that is required. This helps the user make more informed decisions.
+ Fixed crash using QImage to save an image whose total byte count would exceed the 32 bit signed integer limit
+ Fixed issue where user adjusted start and end slices would be ignored or reset in the 'Read H5Ebsd' filter GUI
+ Fixed issue in EbsdLib where the BS, BC and Grain ID columns from CTF Data were not being read correctly from the H5Ebsd file
+ Fixed crashes in FilterParameterUtilities due to missing check for NULL pointers
+ Fixed error when trying to import EBSD files into an H5Ebsd file and the full output path was not available on the filesystem
+ Fixed issue where some filter parameters that represent extremely large numbers (> 2^33) were not read from the JSON files correctly
+ Fixed issue where the Red and Blue Channels were swapped when using the QImage based image importers
+ Fixed issue where a DataArray object that was resized could potentially have its internal array marked as NOT ALLOCATED
+ Added additional Operators (<, >) to the "Neighbor CI Correlation" filter and renamed the filter to "Replace Element Attributes with Neighbor Values"
+ Added ability to build DREAM.3D *without* the use of QtWebEngine libraries. This allows DREAM.3D to be built on some HPC systems.
+ Fixed issue where erroneous coordinates would be returned for ImageGeometry objects.
+ Added ability to have DREAM.3D open .json files on OS X.
+ Fix bug in erode/dilate mask filter.
+ Added more data types that can be written to a VTK RectilinearGrid file
+ Added filter to find the difference between two data arrays.
+ Added additional information to the Triangle Geometry export file.
+ Fixed crashing bugs and incorrect results for the Feature Face Curvature filter.
+ Fixed 2D Calculation issues with the *FindShapes* filter.
+ Added a *RectGrid* Geometry type.
+ Added a "Replace Value in Array" and "Replace Value in Array (Conditional)" filters.
+ Added a "Find Feature Clustering" filter.
+ Added additional capabilities to the "Generate Ensemble Statistics" filter.




## Version 6.1.77 ##

+ Fixed incorrect calculation of Euclidean distance in the Find Euclidean Distance Map **Filter**.
+ Fixed crashes in generation of misorientation colors due to the incorrect sizes of Rodrigues vectors. All space groups should now return the correctly sized Rodrigues vectors.
+ Added a check to ensure the proper number of columns are read from a CTF file.
+ Fixed issues with preflighting certain parameters in **Filters** that subclass from another **Filter**. This includes the align sections **Filters** and the segmentation **Filters**.
+ Fixed the GUI of the Convert Hex Grid to Square Grid **Filter** to allow the user to select the output directory.
+ Added a check to the ANG readers to properly check the reading of hex grid ANG files.
+ Fixed issue where StatsGenerator would not properly save a texture if it had been bulk loaded from an angles file.
+ The StatsGenerator GUI has been redesigned to put pole figures into their own tabs. This allows for the pole figures to be visualized on smaller screens.
+ Added **Filter** that writes out an orientations file that can be loaded by StatsGenerator to represent the texture. This file is now the default used by StatsGenerator to bulk load a texture. 
+ Fixed issue with resizing and saving pole figures in StatsGenerator.
+ Added check in StatsGenerator to ensure that A >= B >= C in the rolled preset dialog.
+ Added **Filter** that uses ITK to read a stack of images. This **Filter** will ensure that imported images are of the correct type (e.g., RGBA, RGB, grayscale, 32-bit float, etc.). The other **Filter** that imports a stack of images uses the Qt image readers, which will convert everything to an RGBA array.
+ Fixed crashing and incorrect calculations in the Find Feature Face Curvature **Filter**.
+ Fixed crashing during renumbering of **Features** in the Change Resolution **Filter**.

## Version 6.1.3 ##

+ Fixed issue with the PhaseTypeSelectionFilterParameter and ShapeTypeSelectionFilterParameter classes where they were not returning the proper widget type.

## Version 6.1.0 ##

+ Major change to the way FilterParameters and user interface widgets interact. There is now an explicit one-to-one mapping of a Filter Parameter to a user interface widget. This mail effects developers.
+ Added widget to create an AttributeMatrix
+ Fixed issue detecting when the pipeline document has changed. This would manifest itself by not allowing the user to save the pipeline.
+ Fixed issue with DynamicTableFilterParameter not copying its data to a new filter instance.
+ Fixed issue where preflight errors that occur just before actually executing the pipeline would not appear in the Issues Table.
+ Fixed issue with the "**Rotate Sample Reference Frame**" filter where the inputs would show as combo boxes instead of edit fields to set the axis and angle.
+ Fixed issue with the "**Rotate Euler Reference Frame**" filter where the inputs would show as combo boxes instead of edit fields to set the axis and angle.
+ Updated the shell scripts to compile the DREAM3D_SDK libraries.
+ Added additional information to update request headers
+ Fix bug in PreflightUpdatedValue widget
+ DataContainerArrayProxyWidget now only preflights when a value changes
+ Hyperlinks in the Issues table now open the filter in the normal help window instead of the users web browser
+ Added new Menu Item to clear the users preferences file

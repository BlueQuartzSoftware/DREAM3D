Version 6.1.0 Release Notes {#releasenotes_6_1_0}
===========

These release notes summarize the major changes for each officially released version of DREAM.3D. The official bug report is located at the [DREAM.3D GitHub issues page](https://github.com/DREAM3D/DREAM3D/issues).

## Getting Help ##

[Users](https://groups.google.com/forum/?/dream3d-users#!forum/dream3d-users)

[Developers](https://groups.google.com/forum/?hl=en#!forum/dream3d-developers)

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
+ Added widget to create an AttributeMatrix.
+ Fixed issue detecting when the pipeline document has changed. This would manifest itself by not allowing the user to save the pipeline.
+ Fixed issue with DynamicTableFilterParameter not copying its data to a new **Filter** instance.
+ Fixed issue where preflight errors that occur just before actually executing the pipeline would not appear in the Issues Table.
+ Fixed issue with the "Rotate Sample Reference Frame" **Filter** where the inputs would show as combo boxes instead of edit fields to set the axis and angle.
+ Fixed issue with the "Rotate Euler Reference Frame" **Filter** where the inputs would show as combo boxes instead of edit fields to set the axis and angle.
+ Updated the shell scripts to compile the DREAM3D_SDK libraries.
+ Added additional information to update request headers.
+ Fix bug in PreflightUpdatedValue widget.
+ DataContainerArrayProxyWidget now only preflights when a value changes.
+ Hyperlinks in the Issues table now open the **Filter** in the normal help window instead of the users web browser.
+ Added new Menu Item to clear the users preferences file.

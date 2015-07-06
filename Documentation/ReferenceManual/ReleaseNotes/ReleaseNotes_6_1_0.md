Version 6.1.0 Release Notes {#releasenotes_6_1_0}
===========


These release notes summarize the major changes for each officially released version of DREAM.3D. The official bug report is located at [https://github.com/DREAM3D/DREAM3D/issues](https://github.com/DREAM3D/DREAM3D/issues).

## Getting Help ##

Users:[https://groups.google.com/forum/?/dream3d-users#!forum/dream3d-users](https://groups.google.com/forum/?/dream3d-users#!forum/dream3d-users)

Developers:[https://groups.google.com/forum/?hl=en#!forum/dream3d-developers](https://groups.google.com/forum/?hl=en#!forum/dream3d-developers)

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

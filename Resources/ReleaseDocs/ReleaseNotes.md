# DREAM.3D Release Notes #

These release notes summarize the major changes for each officially released version of DREAM.3D. The official bug report is located at [https://github.com/DREAM3D/DREAM3D/issues](https://github.com/DREAM3D/DREAM3D/issues).

## Getting Help ##

Users:[https://groups.google.com/forum/?/dream3d-users#!forum/dream3d-users](https://groups.google.com/forum/?/dream3d-users#!forum/dream3d-users)

Developers:[https://groups.google.com/forum/?hl=en#!forum/dream3d-developers](https://groups.google.com/forum/?hl=en#!forum/dream3d-developers)

## Version 6.1.0 ##

+ Updated the shell scripts to compile the DREAM3D_SDK libraries.
+ Added additional information to update request headers
+ Fix bug in PreflightUpdatedValue widget
+ DataContainerArrayProxyWidget now only preflights when a value changes
+ Hyperlinks in the Issues table now open the filter in the normal help window instead of the users web browser
+ Added new Menu Item to clear the users preferences file
+ Major change to the way FilterParameters and user interface widgets interact. There is now an explicit one-to-one mapping of a Filter Parameter to a user interface widget. This mail effects developers.
+ Added widget to create an AttributeMatrix

## Version 6.0.0 ##

This is a major version update with breaking changes that will invalidate any previous .dream3d file and pipeline. Breaking changes are:

+ Pipeline files are now stored as JSON files instead of INI files
+ .dream3d files now have an updated internal structure that is not compatible with any previous version of DREAM.3D
+ **Bookmarks** (formerly **Favorites**) now store direct links to the json file on the file system. This means the user can store the bookmarked json file where ever they want.
+ The **User Interface** has been completely updated and mostly rewritten using Qt version 5.
+ Added new filter searching field which allows the user to easily find filters by name
+ Prebuilt Pipelines have all been updated with bug fixes
+ Internal data structures have been updated to allow DREAM.3D to scale better to larger datasets.
+ DREAM.3D now contains over 200 filters to process data
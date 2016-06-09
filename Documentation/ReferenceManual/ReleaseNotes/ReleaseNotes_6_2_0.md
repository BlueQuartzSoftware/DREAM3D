Version 6.2.0 Release Notes {#releasenotes_6_2_0}
===========

These release notes summarize the major changes for each officially released version of DREAM.3D. The official bug report is located at the [DREAM.3D GitHub issues page](https://github.com/DREAM3D/DREAM3D/issues).

## Getting Help ##

[Users](https://groups.google.com/forum/?/dream3d-users#!forum/dream3d-users)

[Developers](https://groups.google.com/forum/?hl=en#!forum/dream3d-developers)

## Version 6.2.XXX ##


### General Changes ###
+ Releases are now built against Qt 5.6.0, HDF5 1.8.16, Boost 1.60.0, Qwt 6.1.2, Eigen 3.2.8, ITK 4.9.1
+ Created new "ToolBox" window that contains the *Filter List*, *Filter Library* and *Bookmarks* tabs from earlier versions

### DREAM.3D Changes ###

+ Fixing a bug that was causing DREAM3D to crash when quitting if the user launched the help dialog during the session.
+ Adding explanation to CTF readers why 30 degrees is added to phi2 angle.
+ Bug Fix: Enforce that the packing grid has dimension of at least 1.
+ Bug Fix: Fix incorrect computation of seed during segmentation. Thanks to W. Lenthe for the bug report.
+ Bug Fix: Fixing an issue when reading a H5EBSD file where the Euler Angles are represented in Degrees AND the user selects to apply the 'Recommended Transformations'. The Euler Angles are silently treated as radians for some internal calculations that transform from Euler angles to an Orientation Matrix and back to Euler Angles.
+ Feature: Added filter **Find GBCD (Metric Based)**. Thanks to Krzysztof Glowinski for donating the filter.
+ Crash Fix: certain Rodrigues and Axis-Angle combinations would cause a seg- fault due to all values being Zero. Correct formatting of the various labels. distinguish between Errors and Warnings Automatically normalizing the Axis-Angle and Rodrigues vectors. Updating documentation to explicitly state that the utility is auto-normalizing some of the inputs.
+ Bug Fix: DREAM3D_UI closeEvent slot was being called twice when exiting DREAM3D from the dock menu.
+ Adding custom dock menu for Mac OS X.

### SIMPLView Changes ###
+ Bug Fix: Fixing bug where it was possible to delete filter widgets using the delete key while the pipeline was running.
+ Added GUI for **Pause Pipeline** filter
+ Added GUI for **Execute Process** filter
+ Added GUI for **StatsGenerator** filter
+ Added GUI for **Array Calculator** filter
+ Added GUI for standard output to capture output during the **Execute Process** filter
+ Feature: Adding drag-and-drop of filters between DREAM.3D/SIMPLView windows
+ Feature: Adding a "Select All" keyboard short cut
+ Bug Fix: Fixing a bug relating to the help dialog crashing on Windows when DREAM3D closes.
+ Bug Fix: Adding high-DPI support for Windows on retina quality screens.
+ Feature: Allow reading a stack of scans from inside an EDAX HDF5 file.
+ Bug Fix: Fixing bug where changing the name of a data array or attribute matrix in the DataArrayCreationWidget or AttributeMatrixCreationWidget, respectively, would cause the name to revert back to its old name when explicitly pressing the Apply button.
+ Feature: Adding code to disable the Go button whenever preflight finishes with errors.


### SIMPLib Changes ###
+ Feature: Add infrastructure to allow Cut/Copy/Paste of filters into/out-of a pipeline view
+ Feature: Add infrastructure to allow "Undo/Redo" commands
+ Feature: Add **Execute Process** filter that allows the filter to execute any program on the computer system.
+ Feature: Add a **Pause Pipeline** filter that allows the user to pause a pipeline at a given point.
+ Feature: Added a **StatsGenerator** filter that allows the user to generate statistics for a synthetic microstructure inside a pipeline instead of using a separate application.
+ Feature: Added an **Array Calculator** filter
+ Bug Fix: Fix bug that was not allowing **Pipeline Runner** to execute properly
+ Optimize Array initialization during the **Create Data Array** filter
+ Bug Fix: Fix issue where attempting to read a single variable length string from an HDF5 file would result in random bytes copied into the buffer
+ Bug Fix: incorrect logic test in copyData() method of the DataArray class
+ Feature: Filters can now track warning conditions
+ Bug Fix: Fixing a bug that was causing the range filter parameter's values to fail during the H5FilterParameter writing process.
+ Bug Fix: Do not allocate Geometry Arrays during preflight.


## Version 6.2.327 Jan 6, 2016 ##
+ DREAM.3D has new logo and splash screen!
+ Added **Filter** "Import ASCII Data", which implements a generic text reader to import data into DREAM.3D.  The **Filter** utilizes a wizard to guide the user through importing their column-oriented ASCII data.
+ Added **Filter** "Combine Attribute Arrays", which allows the user to stack any number of **Attribute Arrays** to form a new combined array.
+ Added **Filter** "Find GBCD (Metric-Based Approach)" (contribution from K. Glowinski).
+ Added **Filter** "Combine Attribute Matrices" (contribution from M. Groeber).
+ The "Import Images (3D Stack)" **Filter** should now correctly recognize the number of pixel bytes in the incoming images.
+ **Pipeline** files written on Windows should no longer use escaped forward slashes in paths.  This allows for better sharing of **Pipelines** across operating systems.
+ Fixed incorrect documentation on file formats for the "Pack Primary Phases" and "Insert Precipitate Phases" **Filters**.
+ Reverted change to packing routine in "Insert Precipitate Phases" **Filter** that made particles unable to overlap, which prevented reaching large volume fractions of precipitates.  The **Filter** now functions the same as in version 4 of DREAM.3D.
+ Significantly sped up "Change Resolution" **Filter**.
+ Significantly sped up filters that perform segmentations (contribution from M. Groeber).
+ Updated internationalization settings on input widgets that allow the use of decimal numbers. These widgets should now correctly key off the local standards for use of commas and periods in decimal notation. 
+ Refactored and parallelized the derivative calculations of all IGeometry classes.
+ Several prebuilt **Pipelines** have been consolidated and renamed for clarity.

### Developer Notes ###
+ The DREAM.3D SDK has been update to support C++11.  Developers are now free to use C++11 compliant code.
+ Shared pointers now use C++11 shared pointers instead of boost shared pointers.
+ **Plugins** that link against the ITK library now link against the SimpleITKExplicit library instead.  This fixes potential address space issues when using multiple dynamically loaded libraries that may link agains ITK.
+ **Filters** should now implement their own versions for compiled library names, branding strings, and version numbers, instead of using the defaults from AbstractFilter.

### Known Issues ###
+ When navigating the help window, the back and forward buttons may not function.

## Version 6.2.63 ##
+ Version 6.2.63 will be the last version to support OS X 10.8. The minimum version of OS X for all future DREAM.3D updates will be 10.9.
+ Fixed incorrect symbolic link creation for OS X builds that prevented documentation windows from properly appearing.
+ Fixed incorrect **Filter** input requirement in the "Write INL File" **Filter** (issue #471).
+ Fixed "Read H5EBSD File" not properly loading zMax value from a **Pipeline** file (issue #474).
+ Fixed potential array out of bounds error in "Align Sections (Mutual Information)" **Filter** (issue #472).
+ Fixed incorrect bounds check in "Fuse Regular Grids (Nearest Point)" **Filter**.
+ Added internationalization to a few DREAM.3D widgets.
+ Fixed issue when calculating the fundamental zone Rodrigues vector for hexagonal systems.
+ Fixed missing axis normalization in GBCD export **Filters** (issue #477).
+ Added ability in the "Quick Surface Mesh" **Filter** to carry forward any voxel information onto the generated surface mesh.
+ Added "Orientation Utility" **Filter** that will compute all 7 supported representations for a supplied orientation.
+ Fixed potential out of bounds crash in "Match Crystallography" **Filter**.

### Developer Notes ###
+ **Plugin** CMake code has been updated to better organize projects in Visual Studio and Xcode.

## Version 6.2.0 ##
+ Completed migration of all internal rotation transformations to those documented in the paper "Tutorial: Consistent Representations of and Conversions Between 3D Rotations", D. Rowenhorst, A.D. Rollett, G.S. Rohrer, M. Groeber, M. Jackson, P.J. Konijnenberg, and M. De Graef.
+ The rotation transformation codes are tested to be internally consistent using 64 bit floating point precision. 32 bit floating point precision may result in symmetrically equivalent angles but not numerically equivalent angles for certain rotations. See the "Convert Orientations" **Filter** for more details.
+ Added feature where **Data Container**, **Attribute Matrix** and **Attribute Array** combo boxes can be filtered out by the type of object that is required. This helps the user make more informed decisions.
+ Fixed crash when using QImage to save an image whose total byte count would exceed the 32 bit signed integer limit.
+ Fixed issue where user adjusted start and end slices would be ignored or reset in the "Read H5Ebsd" **Filter** GUI.
+ Fixed issue in EbsdLib where the BS, BC and Grain ID columns from CTF data were not being read correctly from the H5Ebsd file.
+ Fixed crashes in FilterParameterUtilities due to missing checks for NULL pointers.
+ Fixed error when trying to import EBSD files into an H5Ebsd file when the full output path was not available on the file system.
+ Fixed issue where some **Filter** parameters that represent extremely large numbers (> 2^33) were not read from the JSON files correctly.
+ Fixed issue where the red and blue channels were swapped when using the QImage based image importers.
+ Fixed issue where a DataArray object that was resized could potentially have its internal array marked as NOT ALLOCATED.
+ Added additional operators (<, >) to the "Neighbor CI Correlation" **Filter** and renamed the **Filter** to "Replace Element Attributes with Neighbor Values".
+ Added ability to build DREAM.3D *without* the use of QtWebEngine libraries. This allows DREAM.3D to be built on some HPC systems.
+ Fixed issue where erroneous coordinates would be returned for **Image Geometry** objects.
+ Added ability to have DREAM.3D open .json files on OS X.
+ Fix bug in erode/dilate mask **Filter**.
+ Added more data types that can be written to a VTK RectilinearGrid file
+ Added **Filter** to find the difference between two data arrays.
+ Added additional information to the **Triangle Geometry** export file.
+ Fixed crashing bugs and incorrect results for the "Feature Face Curvature" **Filter**.
+ Fixed 2D calculation issues with the "Find Shapes" **Filter**.
+ Added a **RectGrid Geometry** type, which represents an unstructured, rectilinear grid with variable resolution.
+ Added "Replace Value in Array" and "Replace Value in Array (Conditional)" **Filters**.
+ Added a "Find Feature Clustering" **Filter**.
+ Added additional capabilities to the "Generate Ensemble Statistics" **Filter**.  The **Filter** can now compute statistics for a matrix phase correctly and incorporate an RDF for a precipitate phase as calculated by the "Find Feature Clustering" **Filter**.

### Developer Notes ###
+ DREAM3DLib has changed names to SIMPLib which requires the minor version change from 6.1 to 6.2
+ FilterParameter API has changed with the ComboBox filtering.
+ Other API changes have also been added to DREAM.3D libraries.

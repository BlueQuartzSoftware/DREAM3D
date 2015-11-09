Version 6.2.0 Release Notes {#releasenotes_6_2_0}
===========


These release notes summarize the major changes for each officially released version of DREAM.3D. The official bug report is located at [https://github.com/DREAM3D/DREAM3D/issues](https://github.com/DREAM3D/DREAM3D/issues).

## Getting Help ##

Users:[https://groups.google.com/forum/?/dream3d-users#!forum/dream3d-users](https://groups.google.com/forum/?/dream3d-users#!forum/dream3d-users)

Developers:[https://groups.google.com/forum/?hl=en#!forum/dream3d-developers](https://groups.google.com/forum/?hl=en#!forum/dream3d-developers)

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

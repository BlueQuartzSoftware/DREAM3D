Adding Filters to DREAM3D {#addingfilters}
=========

## Adding Filter to Existing Category

+ Create the header and implementation files (.h and .cpp)
+ Add both files to the "SourceList.cmake" file that is in the same directory. 
+ Add a .md file for the documentation for the filter into DREAM3D/Documentation/ReferenceManual/Filters/{CATEGRORY}/FilterName.md


### Example
  We want to create a new filter called LaplacianSmoothing in the _SurfaceMeshFilters_ directory. We will do the following:

- Create LaplacianSmoothing.h inside of DREAM3D/Source/DREAM3DLib/SurfaceMeshFilters
- Create LaplacianSmoothing.cpp inside of DREAM3D/Source/DREAM3DLib/SurfaceMeshFilters
- Update the DREAM3D/Source/DREAM3DLib/SurfaceMeshFilters/SourceList.cmake file to include the file in the *_PublicFilters* list.
- Add a .md file for the documentation for the filter into DREAM3D Documentation/ReferenceManual/Filters/SurfaceMeshFilters/LaplacianSmoothing.md
- Open cmake-gui (or cmake); configure and generate
- Reopen (reload/update) project file (Visual Studio Solution)


## Adding New Category

Update DREAM3D/Source/DREAM3DLib/CMakeLists.txt file to add in the new filter
category.


    D3DCompileFilters(SurfaceMeshFilters ${AllFiltersHeaderFile})


Update DREAM3D/Source/FilterWidgets/CMakeLists.txt file to add in the new filter
category


    WidgetGeneration(SurfaceMeshFilters SurfaceMesh_Filters)



## Adding a Filter that reads data or Sets the Volume Dimensions
If you create a filter that reads in a volume of data or does something to initialize the voxel data container with a set of dimensions based from the user or from the file being read then during the dataCheck() method the programmer needs to remember to also set those dimensions into the Volume DataContainer. This may have to be done by reading the header of the file.

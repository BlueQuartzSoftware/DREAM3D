Adding Filters to DREAM3D {#addingfilters}
=========

## Adding Filter to Existing Category

Create the header and implementation files (.h and .cpp)
Add both files to the "SourceList.cmake" file that is in the same directory
If you do NOT add a .html file to the Documentation/Filters/[Filter Category]/[Your Filter Name].html
then a default file will be created for you the first time the code is compiled. The default file
has all the skeleton html in the file and the programmer just needs to document what actions
the filter performs.

### afexample Example
  We want to create a new filter called LaplacianSmoothing. We will do the following:

- Create LaplacianSmoothing.h inside of DREAM3D/Source/DREAM3DLib/SurfaceMeshFilters
- Create LaplacianSmoothing.cpp inside of DREAM3D/Source/DREAM3DLib/SurfaceMeshFilters
- Update the DREAM3D/Source/DREAM3DLib/SurfaceMeshFilters/SourceList.cmake file to include the
new header and implementation files in the list for the ${FILTER_CATEGORY}_FILTERS_HDRS and
DREAM3DLib_${FILTER_CATEGORY}_SRCS cmake variables.
- The first time the code is compiled the build system will add a template html documentation
file in the DREAM3D/Documentation/Filters/SurfaceMeshFilters/LaplacianSmoothing.html
- Update the DREAM3D/Documentation/Filters/index.html file with links to the new filter
- Update the DREAM3D/Documentation/Filters/FilterDocs.qrc file with a new entry for the filter

    Surface_MeshingFilters/LaplacianSmoothing.html



## Adding New Category

Update DREAM3D/Source/DREAM3DLib/CMakeLists.txt file to add in the new filter
category.


    D3DCompileFilters(SurfaceMeshFilters ${AllFiltersHeaderFile})


Update DREAM3D/Source/FilterWidgets/CMakeLists.txt file to add in the new filter
category


    WidgetGeneration(SurfaceMeshFilters SurfaceMesh_Filters)




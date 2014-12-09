Vtk Structured Points Reader  {#vtkstructuredpointsreader}
======

## Group (Subgroup) ##
IO Filters

## Description ##

This filter reads an STRUCTURED_POINTS type of 3D array from a legacy .vtk file. The
currently supported vtk dataset attribute types are SCALARS and VECTORS. More will
come eventually. The vtk data must be POINT_DATA and CELL_DATA

The reader should be able to handle both Binary and ASCII data.

The filter will create a new DataContainer with a Structured Points Geometry and will
then create a new AttributeMatrix inside the new Data Container.

### Sample File ###

    # vtk DataFile Version 2.0
    GrainIds Stored in Vtk File
    ASCII
    DATASET STRUCTURED_POINTS
    DIMENSIONS 3 4 6
    SPACING 1 1 1
    ORIGIN 0 0 0
    POINT_DATA 72
    SCALARS GrainIds char 1
    LOOKUP_TABLE default
    0 0 0 0 0 0 0 0 0 0 0 0
    0 5 10 15 20 25 25 20 15
    10 5 0 0 10 20 30 40 50
    50 40 30 20 10 0 0 10 20
    30 40 50 50 40 30 20 10 0
    0 5 10 15 20 25 25 20 15
    10 5 0 0 0 0 0 0 0 0 0 0 0 0 0


## Parameters ##

| Name | Type |
|------|------|
| Input Vtk File | Input File |
| DataContainer Name | String |
| Attribute Matrix Name | String |

## Required DataContainers ##
None. This filter will create one

## Required Arrays ##
None. This filter will create data arrays


## Created Arrays ##
One data array for each data set attribute in the vtk file.


## Authors ##

**Copyright:** 2014 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 2.0.0. Version 1.0.0 of this filter would ONLY read GrainIds or Feature Ids.

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


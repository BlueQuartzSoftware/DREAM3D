Read Vtk File (STRUCTURED_POINTS) Field Ids Only {#vtkgrainidreader}
======

## Group (Subgroup) ##
IO Filters

## Description ##

This filter reads an STRUCTURED_POINTS type of 3D array from a legacy .vtk file specifically looking for the **Field** Ids array but the user can set the name of the array to try and read.

    # vtk DataFile Version 2.0    GrainIds Stored in Vtk File    ASCII    DATASET STRUCTURED_POINTS    DIMENSIONS 3 4 6    SPACING 1 1 1    ORIGIN 0 0 0    POINT_DATA 72    SCALARS GrainIds char 1 
    LOOKUP_TABLE default 
    0 0 0 0 0 0 0 0 0 0 0 0    0 5 10 15 20 25 25 20 15
    10 5 0 0 10 20 30 40 50 
    50 40 30 20 10 0 0 10 20 
    30 40 50 50 40 30 20 10 0 
    0 5 10 15 20 25 25 20 15 
    10 5 0 0 0 0 0 0 0 0 0 0 0 0 0


## Parameters ##

| Name | Type |
|------|------|
| Input Vtk File | Input File |
| **Field** Id Scalar Name | String |

## Required DataContainers ##
Voxel

## Required Arrays ##
None


## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | The Default Scalars name is "GrainIds" but the user can chose another name |



## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL), 2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




Read Vtk File (STRUCTURED_POINTS) **Field** Ids Only {#vtkgrainidreader}
======

## Group (Subgroup) ##
IO Filters

## Description ##

This filter reads an IMAGE_DATA type of 3D array from a legacy .vtk file specifically looking for the **Field** Ids" array but the user can set the name of the array to try and read.


## Parameters ##

| Name | Type |
|------|------|
| Input Vtk File | Input File |
| **Field** Id Scalar Name | Unknown Type |

## Required Arrays ##
None


## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |



## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.




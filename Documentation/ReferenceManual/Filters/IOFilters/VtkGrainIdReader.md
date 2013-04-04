Read Vtk File (STRUCTURED_POINTS) Grain Ids Only {#vtkgrainidreader}
======

## Group/Subgroup ##
IO Filters


 This filter reads an IMAGE_DATA type of 3D array from a legacy .vtk file specifically looking for the 
"Grain Ids" array but the user can set the name of the array to try and read.


## Parameters ## 

| Name | Type |
|------|------|
| Input Vtk File | Input File |
| Grain Id Scalar Name | Unknown Type |

## Required Arrays ##
None



## Created Arrays ##

| Type | Name | Comment |
|------|------|---------|
| Cell | GrainIds |  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.




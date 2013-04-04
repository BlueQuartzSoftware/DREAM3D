Multi Threshold (Cell Data) {#multithresholdcells}
======

## Group (Subgroup) ##
Processing Filters

## Description ##
This filter is similar to the <a href="SingleThresholdCells.html">SingleThresholdCells</a> filter but allows the user
 to input more than a single cell array. Internally the algorithm creates the output boolean arrays for each comparison
 that the user creates. Then all the output arrays are compared and if __any__ of the values for a specific cell voxel
 is __false__ then the corresponding voxel cell in the final output array is marked as false. This is considered a
 logical 'or' operation.

  An exmaple of this filter's use would be after EBSD data is read into DREAM3D and the user wants to have DREAM3D consider
 voxels that the user considers 'good'. The user would insert this filter and select the criteria that makes a voxel 'good'.
 For the _Small IN100_ example data set the typical values are a _Confidence Index_ &gt 0.1 and an _Image Quality_
 &gt 120.


## Parameters ##

| Name | Type |
|------|------|
| Select Arrays to Compare | Defined by the user |

## Required Arrays ##
None



## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | User Defined | Boolean Array |


## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.





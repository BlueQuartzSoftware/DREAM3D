Multi Threshold (Cell Data) {#multithresholdcells}
======

## Group (Subgroup) ##
Processing Filters (Threshold)

## Description ##
This filter is similar to the [SingleThresholdCells](singlethresholdcells.html) filter but allows the user to input more than a single **Cell** array. Internally the algorithm creates the output boolean arrays for each comparison that the user creates. Then all the output arrays are compared and if __any__ of the values for a specific **Cell**  is __false__ then the corresponding **Cell** in the final output array is marked as false. This is considered a logical 'or' operation.

  An exmaple of this filter's use would be after EBSD data is read into DREAM3D and the user wants to have DREAM3D consider **Cells** that the user considers 'good'. The user would insert this filter and select the criteria that makes a **Cell** 'good'. For the _Small IN100_ example data set the typical values are a _Confidence Index_ > 0.1 and an _Image Quality_ > 120.


## Parameters ##

| Name | Type |
|------|------|
| Select Arrays to Compare | Defined by the user |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data
|------|--------------|-------------|---------|-----|
| Cell | *User Chosen* |  | Array(s) will always be present, since the user may only chose from existing arrays| N/A |




## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | User Defined | Boolean Array |


## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.





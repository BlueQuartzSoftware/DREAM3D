Multi Threshold (Field Data) {#multithresholdfields}
======

## Group (Subgroup) ##
Processing Filters (Threshold)

## Description ##
This filter is similar to the [SingleThresholdFields](singlethresholdfields.html) filter but allows the user to input more than a single **Field** array. Internally the algorithm creates the output boolean arrays for each comparison that the user creates. Then all the output arrays are compared and if __any__ of the values for a specific **Field**  is __false__ then the corresponding **Field** in the final output array is marked as false. This is considered a logical 'or' operation.

  An exmaple of this filter's use would be after statistics are calculated with DREAM3D and the user wants to have DREAM3D consider **Fields** that the user considers 'critical'. The user could insert this filter and select the criteria that makes a **Field** 'critical'. For example if **Fields** that are both large and preferably oriented for slip are of interest, then one could use _Equivalent Diameter_ > X and _Schmid Factor_ > 0.45 as good criteria.

## Parameters ##

| Name | Type |
|------|------|
| Output Array Name | Unknown Type |
| Select Arrays to Threshold | Unknown Type |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Field | *User Chosen* |  | Array(s) will always be present, since the user may only chose from existing arrays | N/A |


## Created Arrays ##
| Type | Name | Comment |
|------|------|---------|
| Field | Output | Boolean Array |


## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


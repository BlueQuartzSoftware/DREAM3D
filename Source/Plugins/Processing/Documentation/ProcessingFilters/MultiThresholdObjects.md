Multi Threshold Objects {#multithresholdobjects}
======

## Group (Subgroup) ##
Processing Filters (Threshold)

## Description ##
This filter allows the user to input single or multiple criteria for thresholding **Objects** in an **Attribute Matrix**. Internally the algorithm creates the output boolean arrays for each comparison that the user creates. Then all the output arrays are compared and if __any__ of the values for a specific **Object** is __false__ then the corresponding **Object** in the final output array is marked as false. This is considered a logical 'or' operation.
An example of this filter's use would be after EBSD data is read into DREAM3D and the user wants to have DREAM3D consider **Cells** that the user considers 'good'. The user would insert this filter and select the criteria that makes a **Cell** 'good'. All arrays **must** come from the same **AttributeMatrix** in order for the filter to execute. For example, an integer array contains the values 1,2,3,4,5. For a comparison value of 3 and the comparison operator greater than, the boolean threshold array produced will contain false, false, false, true, true.

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Select Arrays to Threshold By | Comparison List | This is the set of criteria applied to the **Objects** the selected arrays correspond to when doing the thresholding |

## Required DataContainers ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| *User Chosen* | *User Chosen* | Variable | Variable | Array(s) selected in criteria set |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| *User Chosen* | User Defined | Boolean | (1,1) | Specifies whether the **Object** passed the set of criteria applied during thresholding |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.





See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


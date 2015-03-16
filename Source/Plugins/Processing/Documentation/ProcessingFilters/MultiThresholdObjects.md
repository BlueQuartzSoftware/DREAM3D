Multi Threshold Objects {#multithresholdobjects}
======

## Group (Subgroup) ##
Processing Filters (Threshold)

## Description ##
This filter allows the user to input single or multiple criteria for thresholding **Objects** in an **Attribute Matrix**. Internally the algorithm creates the output boolean arrays for each comparison that the user creates. Then all the output arrays are compared and if __any__ of the values for a specific **Object** is __false__ then the corresponding **Object** in the final output array is marked as false. This is considered a logical 'or' operation.
An example of this filter's use would be after EBSD data is read into DREAM3D and the user wants to have DREAM3D consider **Cells** that the user considers 'good'. The user would insert this filter and select the criteria that makes a **Cell** 'good'. For the _Small IN100_ example data set the typical values are a _Confidence Index_ > 0.1 and an _Image Quality_ > 120.
All arrays **must** come from the same **AttributeMatrix** in order for the filter to execute.

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Select Arrays to Threshold By | Comparison List | This is the set of criteria applied to the **Objects** the selected arrays correspond to when doing the thresholding |

## Required DataContainers ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| *User Chosen* | *User Chosen* | Variable | Variable | Array(s) selected in criteria set |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| *User Chosen* | User Defined | Boolean | (1) | Specifies whether the **Object** passed the set of criteria applied during thresholding |

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.





See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


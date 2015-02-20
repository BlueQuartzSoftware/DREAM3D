Fill Bad Data {#fillbaddata}
======

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This Filter removes small "*noise*" in the data, but keeps larger regions that are possibly **Features**, e.g., pores or defects.
This Filter collects the *bad* **Cells** (*FeatureId = 0*) and _erodes_ them until none remain. However, contiguous groups of *bad* **Cells** that have at least as many **Cells** as the minimum allowed defect size enter by the user will not be *eroded*.

## Parameters ##

| Name | Type | Decision |
|------|------|------|
| Minimum Allowed Defect Size | Integer | The size at which a group of "bad" **Cells** are left unfilled as a "defect" |
| Replace Bad Data | Boolean | Tells the filter whether to replace all data or just **Feature Id** |
| Store Defects As New Phase | Boolean | Tells the filter whether to change the phase of "defects" larger than the minimum allowed size above |

## Required Geometry ##
Image/Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1,1) | Specifies to which **Feature** each **Cell** belongs. |
| Cell | CellPhases | Int | (1,1) | Specifies the phase each **Cell** - is only required if "Store Defects As New Phase" parameter is true |

## Created Arrays ##
None

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


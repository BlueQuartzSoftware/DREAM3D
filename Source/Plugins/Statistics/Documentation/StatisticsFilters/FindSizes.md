Find Feature Sizes {#findsizes}
=============

## Group (Subgroup) ##
Statistics (Morphological)

## Description ##
This **Filter** calculates the sizes of all **Features**.  The **Filter** simply iterates through all **Cells** querying for the **Feature** that owns them and keeping a tally for each **Feature**.  The tally is then stored as *NumCells* and the *Volume* and *EquivalentDiameter* are also calculated (and stored) by knowing the volume of each **Cell**.

## Parameters ##
None

## Required Geometry ##
Image 

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs |
| **Attribute Matrix** | CellFeatureData | Cell Feature | N/A | **Feature Attribute Matrix** of the selected _Feature Ids_ |

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Feature Attribute Array** | EquivalentDiameters | float | (1) | Diameter of a sphere with the same volume as the **Feature** |
| **Feature Attribute Array** | NumCells |  int32_t | (1) | Number of **Cells** that are owned by the **Feature**. This value does not place any distinction between **Cells** that may be of a different size |
| **Feature Attribute Array** | Volumes |  float | (1) | Volume of the **Feature**. This value may be "redundant" from the NumCells value if all **Cells** are the same size and is calculable from the EquivalentDiameters value |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users



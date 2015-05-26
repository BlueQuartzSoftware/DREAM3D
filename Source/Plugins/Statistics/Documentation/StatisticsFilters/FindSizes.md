Find Feature Sizes {#findsizes}
=============

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This Filter calculates the sizes of all **Features**.  The Filter simply iterates through all **Cells** querying for the **Feature** that owns them and keeping a tally for each **Feature**.  The tally is then stored as *NumCells* and the *Volume* and *EquivalentDiameter* are also calculated (and stored) by knowing the volume of each **Cell**.

## Parameters ##
None

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs. |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | EquivalentDiameters | Float | (1) | Diameter of a sphere with the same volume as the **Feature**. |
| Feature | NumCells |  Int | (1) | Number of **Cells** that are owned by the **Feature**. This value does not place any distinction between **Cells** that may be of a different size. |
| Feature | Volumes |  Float | (1) | Volume (in units^3) of the **Feature**. This value may be "redundant" from the NumCells value if all **Cells** are the same size and is calculable from the EquivalentDiameters value. |

## Authors ##
**Copyright:** 2014 Michael A. Groeber (AFRL), 2014 Michael A. Jackson (BlueQuartz Software), 2014 Joseph C. Tucker (UES, Inc.)

**Contact Info:** dream3d@bluequartz.net

**Version:** 5.1.0

**License:**  See the License.txt file that came with DREAM3D.




## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users



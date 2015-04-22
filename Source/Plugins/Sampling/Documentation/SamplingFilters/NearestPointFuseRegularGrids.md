Fuse Regular Grids (Nearest Points) {#nearestpointfuseregulargrids}
=============

## Group (Subgroup) ##
Sampling Filters (Fusion)

## Description ##
This Filter fuses two *Image / Rectilinear Grid* data sets together. The grid of **Cells** in the *Reference* **Data Container** is overlaid on the grid of **Cells** in the *Sampling* **Data Container**.  Each **Cell** in the *Reference* **Data Container** is associated with the nearest point in the *Sampling* **Data Container** (i.e. no *interpolation* is performed).  All the attributes of the **Cell** in the *Sampling* **Data Container** are then assigned to the **Cell** in the *Reference* **Data Container**.

*Note:* The *Sampling* **Data Container** remains identical after this filter, but the *Reference* **Data Container**, while "geometrically identical", gains all the attribute arrays from the *Sampling* **Data Container**.

## Parameters ##
None

## Required Geometry ##
Image / Rectilinear Grid (x2)

## Required Arrays ##
None

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | None | Variable | Variable | Depends on the contents of the *Sampling* **Data Container** - all arrays will have analogue created in the *Reference* **Data Container** |

## Authors ##
**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


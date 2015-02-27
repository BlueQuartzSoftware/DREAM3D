Remove Flagged Features {#removeflaggedfeatures}
=====

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This filter will remove **Features** that have been flagged by another filter from the structure.  The filter requires that the user point to a boolean array at the **Feature** level that tells the filter whether the **Feature** should remain in the structure.  If the boolean array is *false* for a **Feature**, then all **Cells** that belong to that **Feature** are temporarily *unassigned* and after all *undesired* **Features** are removed, the remaining **Features** are isotropically coarsened to fill in the gaps left by the removed **Features**.

## Parameters ##
None

## Required Geometry ##
Image/Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1,1) | Specifies which **Feature** each **Cell** belongs to |
| Feature | None | Boolean | (1,1) | Specifies whether the **Feature** will remain in the structure or not (ie "true" = remain and "false" = remove) |


## Created Arrays ##
None


## Authors ##

**Copyright** 2014 Michael A. Groeber (AFRL), 2014 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

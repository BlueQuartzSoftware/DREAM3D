Remove Flagged Features {#removeflaggedfeatures}
=============

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This Filter will remove **Features** that have been flagged by another Filter from the structure.  The Filter requires that the user point to a boolean array at the **Feature** level that tells the Filter whether the **Feature** should remain in the structure.  If the boolean array is *false* for a **Feature**, then all **Cells** that belong to that **Feature** are temporarily *unassigned* and after all *undesired* **Features** are removed, the remaining **Features** are isotropically coarsened to fill in the gaps left by the removed **Features**.

## Parameters ##
None

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1) | Specifies which **Feature** each **Cell** belongs to |
| Feature | None | Boolean | (1) | Specifies whether the **Feature** will remain in the structure or not (ie "true" = remain and "false" = remove) |

## Created Arrays ##
None

## Authors ##
**Copyright** 2014 Michael A. Groeber (AFRL), 2014 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.




## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


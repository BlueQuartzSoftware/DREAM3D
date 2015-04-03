Fuse Regular Grids {#nearestpointfuseregulargrids}
======

## Group (Subgroup) ##
Sampling Filters (Fusion)

## Description ##
This Filter fuses two *Image/Rectilinear Grid* data sets together. The grid of **Cells** in the *Reference DataContainer* is overlaid on the grid of **Cells** in the *Sampling DataContainer*.  Each **Cell** in the *Reference DataContainer** is associated with the nearest point in the *Sampling DataContainer* (ie there is currently no *interpolation* performed).  All the attributes of the **Cell** in the *Sampling DataContainer* are then assigned to the **Cell** in the *Reference DataContainer*.

**Note that the *Sampling DataContainer* remains identical after this filter, but the *Reference DataContainer*, while 'geometrically identical', gains all the attribute arrays from the *Sampling DataContainer*.


## Parameters ##
None

## Required Geometry ##
Image/Rectilinear Grid (x2)

## Required Arrays ##
None

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | None | Variable | Variable | Depends on the contents of the *Sampling DataContainer* - all arrays will have analogue created in the *Reference DataContainer* |

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


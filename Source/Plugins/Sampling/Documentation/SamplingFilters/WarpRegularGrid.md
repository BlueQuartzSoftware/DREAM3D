Warp Rectilinear Grid {#warpregulargrid}
=============

## Group (Subgroup) ##
Sampling Filters (Warping)

## Description ##
This Filter warps a grid of **Cells** by a polynomial function defined by user-set coefficients.  More specifically, the coefficients define the inverse polynomial, so a new rectilinear grid of **Cells** can be defined and mapped back into the reference grid of **Cells**.  The process is as listed below:

1. Generate a grid of **Cells** identical to the grid to be warped.  
2. Apply the polynomial function to each **Cell** in the grid generated in 1.
3. For each "warped" **Cell** location determined in 2., determine the **Cell** in the original grid closest to that location and assign all of its attributes to the **Cell** in the new grid. (*Note:* the new grid remains rectilinear, but it is sampling the original grid in a non-regular manner - this is "inverse" warping, which allows the output grid to be rectilinear)

It should be noted that because the duplicate grid that is "inverse" warped is identical to the original grid, depending on the polynomial, **Features** or data in the original grid could be warped to a point outside of the domain of the new grid and thus lost.  Alternatively, the grid could "shrink" and there would be extra *bad* data around the edges of the domain.

The user has the option to overwrite the original grid with the *warped* grid or save the *warped* grid as a new **Data Container**.

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Polynomial Order | Choice | Specifies the order of the polynomial function that will warp the **Cells** (2, 3 or 4) |
| nth-Order A Coefficients | Float | A (x) Coefficients of the nth-order polynomial that will warp the **Cells** (2nd order = 6, 3rd order = 10, 4th order = 15) |
| nth-Order B Coefficients | Float | B (y) Coefficients of the nth-order polynomial that will warp the **Cells** (2nd order = 6, 3rd order = 10, 4th order = 15) |
| Save as New DataContainer | Boolean | Specifies if the new grid of **Cells** should replace the current **Geometry** or if a new **Data Container** should be created to hold it |

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
None

## Created Arrays ##
None

## Authors ##
**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


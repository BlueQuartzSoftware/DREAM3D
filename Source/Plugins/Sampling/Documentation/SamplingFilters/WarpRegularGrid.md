Warp Rectilinear Grid {#warpregulargrid}
=============

## Group (Subgroup) ##
Sampling (Warping)

## Description ##
This **Filter** warps a grid of **Cells** by a polynomial function defined by user set coefficients.  More specifically, the coefficients define the inverse polynomial, so a new rectilinear grid of **Cells** can be defined and mapped back into the reference grid of **Cells**.  The process is as listed below:

1. Generate a grid of **Cells** identical to the grid to be warped
2. Apply the polynomial function to each **Cell** in the grid generated in 1
3. For each "warped" **Cell** location determined in 2., determine the **Cell** in the original grid closest to that location and assign all of its attributes to the **Cell** in the new grid. (*Note:* the new grid remains rectilinear, but it is sampling the original grid in a non-regular manner - this is "inverse" warping, which allows the output grid to be rectilinear)

It should be noted that because the duplicate grid that is "inverse" warped is identical to the original grid, depending on the polynomial, **Features** or data in the original grid could be warped to a point outside of the domain of the new grid and thus lost.  Alternatively, the grid could "shrink" and there would be extra *bad* data around the edges of the domain.

The user has the option to overwrite the original grid with the *warped* grid or save the *warped* grid as a new **Data Container**.

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Polynomial Order | Enumeration | Specifies the order of the polynomial function that will warp the **Cells** (2, 3 or 4) |
| nth-Order A Coefficients | float | A (x) Coefficients of the nth-order polynomial that will warp the **Cells** (2nd order = 6, 3rd order = 10, 4th order = 15) |
| nth-Order B Coefficients | float | B (y) Coefficients of the nth-order polynomial that will warp the **Cells** (2nd order = 6, 3rd order = 10, 4th order = 15) |
| Save as New DataContainer | bool | Specifies if the new grid of **Cells** should replace the current **Geometry** or if a new **Data Container** should be created to hold it |

## Required Geometry ##
Image

## Required Objects ##
None

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Data Container** | NewImageDataContainer | N/A | N/A | Created **Data Container** name with an **Image Geometry**. Only created if _Save as New Data Container_ is checked |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)



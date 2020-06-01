# Regular Grid Sample Rectilinear Grid #

## Group (Subgroup) ##

Sampling (Sampling)

## Description ##

This **Filter** will resample an existing RectilinearGrid onto a regular grid (Image Geometry) and copy cell data into the newly created Image Geometry Data Container during the process.

The sampling algorithm is a simple "last one wins" algorithm due to the likely hood that interpolating data is probably not the correct algorithm to bring in data to the new Image Geometry.

The algorithm logic is thus: If the ImageGeometry cell would contain multiple RectilinearGrid cells, then we select from the covered cells the cell with the largest X, Y and Z index and copy that data into the Image Geometry Cell Attribute Matrix.

The user can select which cell attribute matrix data arrays will be copied into the newly created Image Geometry Cell Attribute Matrix.

## Parameters ##

| Name | Type | Description |
|------|------|------|
| Input Rectilinear Grid| DataArray Path | Path to a Data Container containing the RectGrid Geometry |
| Selected Cell Attribute Arrays to Copy | Array of (DataArray Path) | Rect Grid Cell Data to possibly copy |
| Image Geometry Data Container Name |  |  |
| Image Geometry Cell Attribute Matrx Name |  |  |
| Image Geometry Voxel Dimensions | 3 x Int32  |  |

## Required Geometry ##

Rectilinear Grid Geometry

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Data Container** | Data Container Name | N/A | N/A | Data Container holding the RectGrid Geometry |
| **Attribute Matrix** | Attribute Matrix holding the Cell Data to copy to the new geometry | N/A | N/A | Data Container holding the RectGrid Geometry |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Data Container** | Data Container Name | N/A | N/A | Description of object... |
| **Attribute Matrix** | Attribute Matrix Name | Element/Feature/Ensemble/etc. | N/A | Description of object... |
| **Element/Feature/Ensemble/etc. Attribute Array** | Copied from the input Data Container | int32_t/float/etc. | (1)/(3)/etc. | Description of object... |

## Example Pipelines ##

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users
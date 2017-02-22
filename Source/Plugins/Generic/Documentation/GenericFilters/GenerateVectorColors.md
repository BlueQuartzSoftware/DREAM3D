Generate Vector Colors {#generatevectorcolors}
=============

## Group (Subgroup) ##
Generic (Crystallography)

## Description ##
This **Filter** generates a color for each **Element** based on the vector assigned to that **Element** in the input vector data.  The color scheme assigns a unique color to all points on the unit hemisphere using a HSV-like scheme. 

## Parameters ##
| Name             | Type | Description |
|------------------|------|---------|
| Apply to Good Voxels Only (Bad Voxels Will Be Black) | bool | Whether or not to assign colors to *bad* voxels or leave them black |

## Required Geometry ##
Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Element Attribute Array** | VectorData | float | (3) | Vectors the colors will represent |
| **Element Attribute Array** | Mask | bool | (1) | Used to define **Elements** as *good* or *bad*  |


## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Element Attribute Array** | Colors | uint8_t | (3) | RGB colors |


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)



Change Resolution {#changeresolution}
=============

## Group (Subgroup) ##
Sampling (Resolution)

## Description ##
This **Filter** changes the **Cell** spacing/resolution based on inputs from the user. The values entered are the desired new resolutions (not multiples of the current resolution).  The number of **Cells** in the volume will change when the resolution values are changed and thus the user should be cautious of generating "too many" **Cells** by entering very small values (i.e., very high resolution). Thus, this **Filter** will perform a down-sampling or up-sampling procedure.  

A new grid of **Cells** is created and "overlaid" on the existing grid of **Cells**.  There is currently no *interpolation* performed, rather the attributes of the old **Cell** that is closest to each new **Cell's** is assigned to that new **Cell**. 

*Note:* Present **Features** may disappear when down-sampling to coarse resolutions. If _Renumber Features_ is checked, the **Filter** will check if this is the case and resize the corresponding **Feature Attribute Matrix** to comply with any changes. Additionally, the **Filter** will renumber **Features** such that they remain contiguous. 

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Resolution | float (3x) | The new resolution values (dx, dy, dz) |
| Renumber Features | bool | Whether the **Features** should be renumbered |
| Save as New Data Container | bool | Whether the new grid of **Cells** should replace the current **Geometry** or if a new **Data Container** should be created to hold it |

## Required Geometry ##
Image 

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Attribute Matrix** | CellData | Cell | N/A | **Cell Attribute Matrix** that holds data for resolution change |
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs. Only required if _Renumber Features_ is checked |
| **Attribute Matrix** | CellFeatureData | Cell Feature | N/A | **Feature Attribute Matrix** that corresponds to the **Feature** data for the selected _Feature Ids_. Only required if _Renumber Features_ is checked |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Data Container** | NewImageDataContainer | N/A | N/A | Created **Data Container** name with an **Image Geometry**. Only created if _Save as New Data Container_ is checked |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)



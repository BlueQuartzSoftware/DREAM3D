Initialize Data {#initializedata}
=============

## Group (Subgroup) ##
Processing (Cleanup)

## Description ##
This **Filter** allows the user to define a subvolume of the data set in which the **Filter** will reset all data by writing *zeros (0)* into every array for every **Cell** within the subvolume.

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| X Min | int32_t | Minimum X bound in **Cells** |
| Y Min | int32_t | Minimum Y bound in **Cells** |
| Z Min | int32_t | Minimum Z bound in **Cells** |
| X Max | int32_t | Maximum X bound in **Cells** |
| Y Max | int32_t | Maximum Y bound in **Cells** |
| Z Max | int32_t | Maximum Z bound in **Cells** |

## Required Geometry ##
Image 

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Attribute Matrix** | CellData | Cell | N/A | **Cell** data in which to initialize a subvolume to zeros |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)



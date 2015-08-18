Remove Component From Array {#removecomponentfromarray}
=============

## Group (Subgroup) ##
Core (Memory/Management)

## Description ##
This **Filter** will remove a single component of a multicomponent array and put it in a new **Attribute Array**.

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Component Number to Extract | int32_t | The index of which component to extract |
| Save Removed Component in New Array | bool | Whether to save the extracted component in a new scalar **Attribute Array** |


## Required Geometry ##
Not Applicable

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Any **Attribute Array** | None | Any | >1 | Multicomponent **Attribute Array** to use as input |


## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Any **Attribute Array** | None | Any | 1 less than the required Multicomponent **Attribute Array**   | Reduced **Attribute Array** name |
| Any **Attribute Array** | None | Any | (1)  | Removed Component (Scalar) **Attribute Array** name |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)



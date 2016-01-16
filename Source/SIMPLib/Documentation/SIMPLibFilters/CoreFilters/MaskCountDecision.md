Mask Count Decision {#maskcountdecision}
=============

## Group (Subgroup) ##
Core (Misc)

## Description ##
This **Filter** emits a *true* or *false* signal based on the number of occurrences of *true* in the supplied boolean **Attribute Array**. If the user input *Number of True Instances* is less than the number of *true* values in the supplied array, a boolean false is emitted. Otherwise, a boolean true is emitted. If a negative number is entered for the *Number of True Instances*, then the **Filter** will emit true only if all values in the supplied array are true. 

*Note:* This **Filter** only provides use when DREAM.3D is engaging in network communications.  

## Parameters ##
| Name             | Type | Description |
|------------------|------| ------------|
| Number of True Instances | int32_t | The number of *true* instances for comparison |
 
## Required Geometry ##
Any

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Any **Attribute Array** | None | int32_t | (1) | Boolean array on which to apply decision   |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)








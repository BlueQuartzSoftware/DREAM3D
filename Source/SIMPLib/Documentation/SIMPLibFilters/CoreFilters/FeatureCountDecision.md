Feature Count Decision {#featurecountdecision}
=============

## Group (Subgroup) ##
Core (Misc)

## Description ##
This **Filter** emits a *true* or *false* signal based on the number of **Feature** Ids in the supplied **Ensemble Attribute Array**. If the user input *Maximum Number of Features* is less than the number of **Features**, a boolean false is emitted. Otherwise, a boolean true is emitted. This **Filter** assumes that only one **Ensemble** is present. 

*Note:* This **Filter** only provides use when DREAM.3D is engaging in network communications.  

## Parameters ##
| Name             | Type | Description |
|------------------|------| ------------|
| Maximum Number of Features | int32_t | The number of **Features** for comparison |
 
## Required Geometry ##
Any

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Cell Ensemble Attribute Array** | Number_of_grains | int32_t | (1) | FeatureIds |

## Created Objects ##
None

## License & Copyright ##
Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##
If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)








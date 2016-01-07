FeatureCountDecision {#featurecountdecision}
=====

## Group (Subgroup) ##
Core (Misc)

## Description ##
This filter makes a decision on the number of grains. If the user input MaxGrains is less than the FeatureIds, a boolean false is emitted. Note: Assumes a SINGLE Phase. This WILL BREAK if there is more than 1 phase. 

## Parameters ##
| Name             | Type | Description |
|------------------|------| ------------|
| Max Grains | int32_t | The number of grains for comparison |
 
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








Find Feature Phases {#findfeaturephases}
=============

## Group (Subgroup) ##
Generic (Misc)

## Description ##
This **Filter** determines the **Ensemble** of each **Feature** by querying the **Ensemble** of the **Elements** that belong to the **Feature**. Note that it is assumed that all **Elements** belonging to a **Feature** are of the same **Feature**, and thus any **Element** can be used to determine the **Ensemble** of the **Feature** that owns that **Element**.

## Parameters ##
None

## Required Geometry ##
Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Element Attribute Array** | Phases | int32_t | (1) | Specifies to which **Ensemble** each **Element** belongs |
| **Element Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Element** belongs |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Feature Attribute Array** | Phases | int32_t | (1) | Specifies to which **Ensemble** each **Feature** belongs  |


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)



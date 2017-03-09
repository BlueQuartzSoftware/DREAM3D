Find Average Value of Scalars For Feature {#findavgscalarvalueforfeatures}
=============

## Group (Subgroup) ##
Statistics (Misc)

## Description ##
This **Filter** determines the average value of a scalar **Attribute Array** for each **Feature** by the following algorithm:

1. Gather all **Elements** that belong to the **Feature**
2. Average the values of the scalar **Attribute Array** for all **Elements** and store as the average for the **Feature**

## Parameters ##
None

## Required Geometry ##
Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Element Attribute Array**  | None | Any | (1) | Scalar **Attribute Array** to average for each **Feature** |
| **Element Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Element** belongs |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Feature Attribute Array** | ScalarAverages | float | (1) | Average value of scalar **Attribute Array** for each **Feature** |


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)



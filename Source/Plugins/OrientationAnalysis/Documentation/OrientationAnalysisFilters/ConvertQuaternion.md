# Convert Quaternion Order #

## Group (Subgroup) ##

OrientationAnalysis (Conversions)

## Description ##

Internally DREAM.3D assumes that a quaternion is laid out in the order such that < x, y, z >, w or Vector-Scalar ordering. Codes and algorithms external to DREAM.3D may store quaternions in the opposite or Scalar-Vector order (w < x,y,z >). This filter will allow the user to easily convert imported Quaternions into the representation that DREAM.3D expects.

For Example if the user has imported quaternion data in the form of Scalar-Vector then they would run this filter using a conversion type of **ToVectorScalar** and using the generated quaternions in subsequent filters. If the user wanted to then write out the Quaternions in the Scalar-Vector form then could add this filter again to the end of the pipeline (but before writing out data) to convert the Vector-Scalar quaternions array (assuming something modified the array).

## Parameters ##

| Name | Type | Description |
|------|------|------|
| Delete Original Array | Boolean | Set this to TRUE/ON to have the original 3 component data array deleted at the end of the filter. |
| Conversion Type | Integer | 0=ToScalarVector, 1=ToVectorScalar |

## Required Geometry ##

Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Attribute Array** | Quaternion Input | float | (4) |  |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Attribute Array** | Quaternion Ouput | float | (4) |  |


## Example Pipelines ##


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users
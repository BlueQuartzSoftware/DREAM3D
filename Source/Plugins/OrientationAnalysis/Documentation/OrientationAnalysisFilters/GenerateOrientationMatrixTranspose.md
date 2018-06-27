# Generate Orientation Matrix Transpose #

## Group (Subgroup) ##

Processing (OrientationAnalysis)

## Description ##

This filter will generate the transpose of a [3x3] _Orientation Matrix_. This can be handy when the user wants to convert the orientation transformation to an opposite effect.

**NOTES**: Internally DREAM.3D assumes that the internal reference transformation is a **Sample to Crystal** transformation. If the incoming data was collected in such a way that the orientation representation that is stored (Quats, Eulers, Orientation Matrix, Rodrigues) was assumed to be a **Crystal to Sample** transformation then this filter can be applied to an orientation matrix to convert into a reference frame that DREAM.3D assumes.

## Parameters ##

| Name | Type | Description |
|------|------|------|
| Delete Original Array | Boolean | Set this to TRUE/ON to have the original 3 component data array deleted at the end of the filter. |

## Required Geometry ##

Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Attribute Array** | Orienation Matrix Input | float | (3)(3) |  |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Attribute Array** | Orienation Matrix Ouput | float | (3)(3) |  |


## Example Pipelines ##


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users
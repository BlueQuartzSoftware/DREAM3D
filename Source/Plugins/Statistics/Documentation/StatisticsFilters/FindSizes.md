Find Feature Sizes {#findsizes}
=============

## Group (Subgroup) ##
Statistics (Morphological)

## Description ##
This **Filter** calculates the sizes of all **Features**.  The **Filter** simply iterates through all **Elements** querying for the **Feature** that owns them and keeping a tally for each **Feature**.  The tally is then stored as *NumElements* and the *Volume* and *EquivalentDiameter* are also calculated (and stored) by knowing the volume of each **Element**.

During the computation of the **Feature** sizes, the size of each individual **Element** is computed and stored in the corresponding **Geometry**. By default, these sizes are deleted after executing the **Filter** to save memory. If you wish to store the **Element** sizes, select the *Save Element Sizes* option. The sizes will be stored within the **Geometry** definition itself, not as a separate **Attribute Array**.

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Save Element Sizes | bool | Whether the to store the individual **Element** sizes |

## Required Geometry ##
Not Applicable 

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Element Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Element** belongs |
| **Attribute Matrix** | FeatureData | Feature | N/A | **Feature Attribute Matrix** of the selected _Feature Ids_ |

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Feature Attribute Array** | EquivalentDiameters | float | (1) | Diameter of a sphere with the same volume as the **Feature** |
| **Feature Attribute Array** | NumElements |  int32_t | (1) | Number of **Elements** that are owned by the **Feature**. This value does not place any distinction between **Elements** that may be of a different size |
| **Feature Attribute Array** | Volumes |  float | (1) | Volume of the **Feature**. This value may be "redundant" from the NumElements value if all **Elements** are the same size and is calculable from the EquivalentDiameters value |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)



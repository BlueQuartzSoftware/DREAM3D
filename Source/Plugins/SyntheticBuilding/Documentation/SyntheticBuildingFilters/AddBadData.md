Add Bad Data {#addbaddata}
=============

## Group (Subgroup) ##
Synthetic Building (Misc)

## Description ##
This Filter adds "bad" data to an **Image Geometry**.  This Filter is intended to add "realism" (i.e., more representative of an experimental dataset) to synthetic structures that don not have any "bad" **Cells**.  The user can choose to add "random noise" and/or "noise" along **Feature** boundaries. For a given type of noise, the user must then set the volume fraction of **Cells** to set as "bad".  The volume fractions entered apply to only the set of **Cells** that the noise would affect.  For example, if the user chose *0.2* for the volume fraction of boundary "noise", then each boundary **Cell** would have a *20%* chance of being changed to a "bad" **Cell** and all other **Cells** would have a *0%* chance of being changed. In order to compute noise over the **Feature** boundaries, the **Filter** needs the [Manhattan distances](@ref findeuclideandistmap) for each **Cell** from the **Feature** boundaries

All **Attribute Arrays** that belong to the same **Attribute Matrix** as the selected _Feature Boundary Euclidean Distances_ array will have noise added to them. To flag a value as "noise", this **Filter** will initialize a selected _tuple_ in the **Attribute Array** to **0**. Note that a zero value _may not_ necessarily represent a "bad" data point in any kind of **Attribute Array**.

For more information on synthetic building, visit the [tutorial](@ref tutorialsyntheticsingle).

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Add Random Noise | Bool | Whether to add random Poisson noise to the whole volume |
| Volume Fraction Random Noise | Float | Fraction of noise to add over the whole volume |
| Add Boundary Noise | Bool | Whether to add noise to the boundary **Cells** |
| Volume Fraction Boundary Noise | Float | Fraction of noise to add to the boundary **Cells** |

## Required Geometry ##
Image

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | GBEuclideanDistances | Float | (1) | Manhattan distances of each **Cell** to the closest **Feature** boundary |

## Created Arrays ##
None


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users



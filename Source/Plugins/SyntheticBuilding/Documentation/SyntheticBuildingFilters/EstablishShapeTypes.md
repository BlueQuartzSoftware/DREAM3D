Establish Shape Types {#establishshapetypes}
=============

## Group (Subgroup) ##
Synthetic Building (Generation)

## Description ##
This Filter allows the user to assign a specific _shape type_ to each **Ensemble** of a synthetic structure. This Filter should be inserted **BEFORE** any packing **Filters** are added. The Filter is typically added just after the [initializing the synthetic volume](@ref initializesyntheticvolume). The various shape types are:

+ Ellipsoid
+ Super Ellipsoid
+ Cube Octahedron
+ Cylinder
+ Unknown Shape Type

The _Unknown Shape Type_ represents an "error" shape type that _should not_ be used to create normal synthetic structures.

For more information on synthetic building, visit the [tutorial](@ref tutorialsyntheticsingle).

## Parameters ##
None

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Ensemble | PhaseTypes | Int | (1) | Enumeration specifying the type of phase of each **Ensemble** |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Ensemble | ShapeTypes | Int | (1) | Enumeration specifying the type of shape for each **Ensemble** |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users



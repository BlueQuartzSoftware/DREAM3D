Establish Shape Types {#establishshapetypes}
=============

## Group (Subgroup) ##
Synthetic Building (Generation)

## Description ##
This **Filter** allows the user to assign a specific _shape type_ to each **Ensemble** of a synthetic structure. This **Filter** should be inserted **BEFORE** any packing **Filters** are added. The **Filter** is typically added just after the [initializing the synthetic volume](@ref initializesyntheticvolume). The various shape types are:

+ Ellipsoid
+ Super Ellipsoid
+ Cube Octahedron
+ Cylinder
+ Unknown Shape Type

The _Unknown Shape Type_ represents an "error" shape type that _should not_ be used to create normal synthetic structures.

For more information on synthetic building, visit the [tutorial](@ref tutorialsyntheticsingle).

## Parameters ##
None

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Ensemble Attribute Array** | PhaseTypes | uint32_t | (1) | Enumeration specifying the phase type of each **Ensemble** |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Ensemble Attribute Array** | ShapeTypes | uint32_t | (1) | Enumeration specifying the type of shape to place for each **Ensemble** |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)



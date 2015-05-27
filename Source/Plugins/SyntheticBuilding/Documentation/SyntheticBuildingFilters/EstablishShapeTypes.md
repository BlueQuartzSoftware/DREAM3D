Establish Matrix Phase {#establishshapetypes}
======

## Group (Subgroup) ##
Synthetic Builder Filters (Initialization)

## Description ##
This Filter allows the user to assign a specific shape type to each phase of their synthetic structure. This filter should
be inserted **BEFORE** any packing filters are added. The filter is typically added just after the Initialize Synthetic Volume
filter is added. The various shape types are
+ Ellipsoid
+ Super Ellipsoid
+ Cube Octahedron
+ Cylinder

## Parameters ##
| Name             | Type | Notes |
|------------------|------|-------|
| Phase Types Array | UInt32 Array | Any array that has the correct number of tuples will work, i.e., the number of tuples is the same as the number of phases |


## Required Arrays ##

An Array that has the correct number of phases indentified. For example, one could use the "Crystal Structures" array if needed

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Ensemble | PhaseTypes | Enumeration (int) specifying the type of phase of each Ensemble/phase (Primary=0, Precipitate=1, Transformation=2, Matrix=3, Boundary=4) |  | Intialize Synthetic Volume (SyntheticBuilding), Generate Ensemble Statistics (Statistics) |


## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Ensemble | ShapeTypes | UInt32 array of the shape types. Each value in the array represents one of the enumerations above. |  |

## Authors ##






## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users



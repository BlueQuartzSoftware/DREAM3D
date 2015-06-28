Find Feature Radial Distribution Function {#findradialdist}
======

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This Filter determines the number of **Features** whose *centroids* lie within a distance of one *Equivalent Sphere Diameter* from a each **Feature**.  The algorithm for determining the number of **Features** is given below:

1. Find the *centroid* of a **Feature**.
2. Define a sphere centered at the the **Feature**'s *centroid*  and with radius equal to the **Feature**'s *Equivalent Sphere Diameter*.
3. Check every other **Feature**'s centroid to see if it lies within the sphere and keep count of those that satisfy.
4. Repeat 1-3 for all **Features**.



## Parameters ##
None

## Required DataContainers ##
Voxel

## Required Objects ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**| | Find Feature Phases (Generic), Read Feature Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |

## Created Objects ##
None 


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)



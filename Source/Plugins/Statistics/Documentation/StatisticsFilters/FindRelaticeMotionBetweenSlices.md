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

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**| | Find Feature Phases (Generic), Read Feature Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |

## Created Arrays ##
None 

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


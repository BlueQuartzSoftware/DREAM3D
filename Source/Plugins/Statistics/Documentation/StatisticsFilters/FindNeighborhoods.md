Find Feature Neighborhoods {#findneighborhoods}
======

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This Filter determines the number of **Features**, for each **Feature**, whose *centroids* lie within a distance equal to a user defined multiple of the average *Equivalent Sphere Diameter* (*average of all **Features**).  The algorithm for determining the number of **Features** is given below:

1. Define a sphere centered at the **Feature**'s *centroid* and with radius equal to the average equivalent sphere diameter multiplied by the user defined multiple.
2. Check every other **Feature**'s *centroid* to see if it lies within the sphere and keep count and list of those that satisfy.
3. Repeat 1&2 for all **Features**.

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Multiples Of Average Diameter | Float | Defines the search radius to use when looking for "neighboring" **Features** |

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | Centroids | Float | (3) | X, Y, Z coordinates of **Feature** center of mass |
| Feature | EquivalentDiameters | Float | (1) | Diameter of a sphere with the same volume as the **Feature**. |
| Feature | FeaturePhases | Int | (1) | Specifies the phase of the **Feature** - Values will begin at 1 as there is no phase 0, which is used temporarily in some filters for bad data|

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | Neighborhoods | Int | (1) | Number of **Features** that have their centroid within the user specified multiple of equivalent sphere diameters from each **Feature** |
| Feature | NeighborhoodLists | List of Ints | (1) | List of the **Features** whose centroids are within the user specified multiple of equivalent sphere diameter from each **Feature** |

## Authors ##

**Copyright:** 2014 Joseph C. Tucker (UES), 2012 Michael A. Groeber (AFRL), 2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


Find Feature Neighborhoods {#findneighborhoods}
=============

## Group (Subgroup) ##
Statistics (Morphological)

## Description ##
This **Filter** determines the number of **Features**, for each **Feature**, whose *centroids* lie within a distance equal to a user defined multiple of the average *Equivalent Sphere Diameter* (*average of all **Features**).  The algorithm for determining the number of **Features** is given below:

1. Define a sphere centered at the **Feature**'s *centroid* and with radius equal to the average equivalent sphere diameter multiplied by the user defined multiple
2. Check every other **Feature**'s *centroid* to see if it lies within the sphere and keep count and list of those that satisfy
3. Repeat 1. & 2. for all **Features**

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Multiples of Average Diameter | float | Defines the search radius to use when looking for "neighboring" **Features** |

## Required Geometry ##
Image 

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Feature Attribute Array** | EquivalentDiameters | float | (1) | Diameter of a sphere with the same volume as the **Feature** |
| **Feature Attribute Array** | Phases | int32_t | (1) | Specifies to which **Ensemble** each **Feature** belongs |
| **Feature Attribute Array** | Centroids | float | (3) | X, Y, Z coordinates of **Feature** center of mass |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Feature Attribute Array** | Neighborhoods | int32_t | (1) | Number of **Features** that have their centroid within the user specified multiple of equivalent sphere diameters from each **Feature** |
| **Feature Attribute Array** | NeighborhoodLists | List of int32_t | (1) | List of the **Features** whose centroids are within the user specified multiple of equivalent sphere diameter from each **Feature** |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)



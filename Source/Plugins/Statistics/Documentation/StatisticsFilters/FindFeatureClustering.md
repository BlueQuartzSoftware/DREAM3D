Find Feature Clustering {#findfeatureclustering}
=============

## Group (Subgroup) ##
Statistics (Morphological)

## Description ##
This Filter determines the radial distribution function (RDF), as a histogram, of a given set of **Features**. Currently, the **Features** need to be of the same **Ensemble** (specified by the user), and the resulting RDF is stored as **Ensemble** data. This Filter also returns the clustering list (the list of all the inter-**Feature** distances) and the minimum and maximum separation distances. The algorithm proceeds as follows:

1. Find the Euclidean distance from the current **Feature** centroid to all other **Feature** centroids of the same specified phase
2. Put all caclulated distances in a clustering list
3. Repeat 1-2 for all **Features**
4. Sort the data into the specified number of bins, all equally sized in distance from the minimum distance to the maximum distance between **Features**. For example, if the user chooses 10 bins, and the minimum distance between **Features** is 10 units and the maximum distance is 80 units, each bin will be 8 units 
5. Normalize the RDF by the probability of finding the **Features** if distributed randomly in the given box 

*Note:* Because the algorithm iterates over all the **Features**, each distance will be double counted. For example, the distance from **Feature** 1 to **Feature** 2 will be counted along with the distance from **Feature** 2 to **Feature** 1, which will be identical. 

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Number of Bins for RDF | int32_t | Number of bins to split the RDF |
| Phase Index | int32_t | **Ensemble** number for which to calculate the RDF and clustering list |

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
| **Feature Attribute Array** | ClusteringList | float | (1) | Distance of each **Features**'s centroid to ever other **Features**'s centroid |
| **Ensemble Attribute Array** | RDF | float | (Number of Bins) | A histogram of the normalized frequency at each bin | 
| **Ensemble Attribute Array** | RDFMaxMinDistances | float | (2) | The max and min distance found between **Features** |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)



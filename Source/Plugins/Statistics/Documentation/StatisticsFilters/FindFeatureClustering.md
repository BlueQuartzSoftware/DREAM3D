Find Feature Clustering {#findfeatureclustering}
======

## Group (Subgroup) ##
Statistics Filters (Morphological)



## Description ##
This filter determines the radial distribution function (RDF), as a histogram, of a given set of **Features**. Currently, the features need to be of the same phase (specified by the user), and the resulting RDF is stored as **Ensemble** data. This filter also returns the clustering list (the list of all the inter**Feature** distances) and the min and max separation distances.


1. Find the distance from the current **Feature** to all other **Features** of the same, specified phase. 
2. Put all distances in a Clustering List. 
3. Repeat 1-2 for all **Features**.
3. Sort the data into the specified number of bins, all equally sized in distance from the minimum distance to the maximum distance between particles. For example, if the user chooses 10 bins, and the min distance between **Features** is 10 microns and the max distance is 80 microns, each bin will be 8 microns. 
4. Normalize the RDF by the probability of finding the features if distributed randomly in the given box. 

Note, because this iterates over all the features, each distance will be double counted. For example, the distance from **Feature** 1 to **Feature** 2 will be counted along with the distance from **Feature** 2 to **Feature** 1, which will be identical. 



## Parameters ##
| Name | Type | Description |
|------|------|-------------|
| Number of Bins for RDF | Integer | Number of bins to split the RDF into |
| Phase Number | Integer | Phase number to calculate the RDF and clustering list for |


## Required DataContainers ##
Volume

## Required Objects ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Feature | Centroids | X, Y, Z coordinates (floats) of **Feature** center of mass |  | Find Feature Centroids (Generic) |
| Feature | EquivalentDiameters | Diameter (float) of a sphere with the same volume as the **Feature**. | | Find Feature Sizes (Statistics) |
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**| | Find Feature Phases (Generic), Read Feature Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |

## Created Objects ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | Clustering List | Distance (float) of each **Features**'s centroid to ever other **Features**'s centroid. |  |
| Ensemble | RDF | A histogram (float vector) of the normalized frequency at each bin  |  |
| Ensemble | RDFMaxMinDistances | The max and min distance (float) found between particles  | This array will automatically be created and the named by appending “MaxMinDistances” to whatever the RDF array is named|


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)



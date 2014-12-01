Find Feature Clustering {#findfeatureclustering}
======

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This filter determines the radial distribution function (RDF), as a histogram, of a given set of **Features**. 


1. Find the distance from current **Feature** to all other Features of the same, specified phase. 
2. Put all distances in a Clustering List. 
3. Repeat 1-2 for all **Features**.
3. Sort the data into the specified number of bins, all equally sized in distance from the minimum distance to the maximum distance between particles.

Note, because this iterates over all the features, each distance will be double counted. For example, the distance from **Feature** 1 to **Feature** 2 will be counted along with the distance from **Feature** 2 to **Feature** 1, which will be identical. 



## Parameters ##
| Name | Type | Description |
|------|------|-------------|
| Number of Bins for RDF | Integer | Number of bins to split the RDF into |
| Phase Number | Integer | Phase number to calculate the RDF and clustering list for |


## Required DataContainers ##
Volume

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Feature | Centroids | X, Y, Z coordinates (floats) of **Feature** center of mass | Filter will calculate **Feature** centroids if not previously calculated | Find Feature Centroids (Generic) |
| Feature | EquivalentDiameters | Diameter (float) of a sphere with the same volume as the **Feature**. | Filter will calculate equivalent diameters for the **Features** if not already calculated | Find Feature Sizes (Statistics) |
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**| | Find Feature Phases (Generic), Read Feature Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | Clustering | Distance (float) of each **Features**'s centroid to ever other **Features**'s centroid. |


## Authors ##

**Copyright:** 2014 Joseph C. Tucker (UES), 2012 Michael A. Groeber (AFRL), 2012 Michael A. Jackson (BlueQuartz Software)

**Version:** 5.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


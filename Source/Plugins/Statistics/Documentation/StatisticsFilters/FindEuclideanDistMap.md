Find Euclidean Distance Map {#findeuclideandistmap}
=============

## Group (Subgroup) ##
Statistics (Morphological)

## Description ##
This **Filter** calculates the distance of each **Cell** from the nearest **Feature** boundary, triple line and/or quadruple point.  The following algorithm explains the process:

1. Find the **Feature** that owns each **Cell** and its six face-face neighbors of each **Cell**
2. For all **Cells** that have *at least 2* different neighbors, set their *GBEuclideanDistance* to *0*.  For all **Cells** that have *at least 3* different neighbors, set their *TJEuclideanDistance* to *0*.  For all **Cells** that have *at least 4* different neighbors, set their *QPEuclideanDistance* to *0*
3. For each of the three *EuclideanDistace* maps, iteratively "grow" out from the **Cells** identified to have a distance of *0* by the following sub-steps:

  - Determine the **Cells** that neighbor a **Cell** of distance *0* in the current map.
  - Assign a distance of *1* to those **Cells** and list the *0* **Cell** neighbor as their *nearest neighbor*
  - Repeat previous two sub-steps, increasing the distances by *1* each iteration, until no **Cells** remain without a distance and *nearest neighbor* assigned.

	*Note:* the distances calculated at this point are "city-block" distances and not "shortest distance" distances.

4. If the option *Calculate Manhattan Distance Only* is *false*, then the "city-block" distances are overwritten with the *Euclidean Distance* from the **Cell** to its *nearest neighbor* **Cell**


## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Calculate Manhattan Distance Only | bool | Whether the distance to boundaries, triple lines and quadruple points is stored as "city block" or "Euclidean" distances |
| Calculate Distance to Boundaries | bool | Whetherthe distance of each **Cell** to a **Feature** boundary is calculated |
| Calculate Distance to Triple Lines | bool | Whetherthe distance of each **Cell** to a triple line between **Features** is calculated |
| Calculate Distance to Quadruple Points | bool | Whetherthe distance of each **Cell** to a  quadruple point between **Features** is calculated |
| Store the Nearest Boundary Cells | bool | Whether to store the *nearest neighbors* of **Cell**  


## Required Geometry ##
Image 

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | GBEuclideanDistances | float | (1) | Distance the **Cells** are from the *boundary* of the **Feature** they belong to. Only created if _Calculate Distance to Boundaries_ is checked |
| **Cell Attribute Array** | TJEuclideanDistances | float | (1) | Distance the **Cells** are from a *triple junction* of **Features**. Only created if _Calculate Distance to Triple Lines_ is checked |
| **Cell Attribute Array** | QPEuclideanDistances | float | (1) | Distance the **Cells** are from a *quadruple point* of **Features**. Only created if _Calculate Distance to Quadruple Points_ is checked |
| **Cell Attribute Array** | NearestNeighbors | int32_t | (3) | Indices of the closest **Cell** that touches a boundary, triple and quadruple point for each **Cell**. Only created if _Store the Nearest Boundary Cells_ is checked |


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)



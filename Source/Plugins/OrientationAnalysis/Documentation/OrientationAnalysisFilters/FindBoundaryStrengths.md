Find Feature Boundary Strength Metrics {#findboundarystrengths}
======

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This filter calculates the same metrics as in the [Find Slip Transmission Metrics Filter](FindSlipTransmissionMetrics.html "").  However, this filter stores the values in the **Face** map in the SurfaceMesh DataContainer.  The algorithm the filter uses is below:

1. Find the two **Features** that are separated by a **Face** in the SurfaceMesh.
2. Get the average orientation of both of the **Features**.
3. Calculate the transmission metrics from both directions (i.e. **Feature** 1 to **Feature** 2 and **Feature** 2 to **Feature** 1).
4. Store the metrics in the **Face** map
5. Repeat for all **Faces**

Note: each metric is calculated twice for the two different directions slip could approach the boundary.  The values are stored on each **Face** in the **Face** map in a way that notes the direction (i.e. 1 to 2 vs. 2 to 1).

## Parameters ##

| Name | Type | Description |
|------|------| ----------- |
| Loading | Float Vector | The X, Y & Z Loading for the sample. |

## Required DataContainers ##

+ Voxel
+ SurfaceMesh

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Feature (Voxel) | AvgQuats | Five (5) values (floats) defining the average orientation of the **Feature** in quaternion representation | Filter will calculate average quaternions for **Features** if not already calculated. | Find Feature Average Orientations (Statistics) |
| Feature (Voxel) | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**| | Find Feature Phases (Generic), Read Feature Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Feature (Voxel) | Schmids |  |  | Find Schmid Factors (Statistics) |
| Feature (Voxel) | SlipSystems |  |  | Find Schmid Factors (Statistics) |

## Created Arrays ##
| Type | Default Name | Comment |
|------|--------------|---------|
| Face (SurfaceMesh) | SurfaceMeshF1s |  |
| Face (SurfaceMesh) | SurfaceMeshF1spts |  |
| Face (SurfaceMesh) | SurfaceMeshF7s |  |
| Face (SurfaceMesh) | SurfaceMeshmPrimes |  |


## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.


## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users



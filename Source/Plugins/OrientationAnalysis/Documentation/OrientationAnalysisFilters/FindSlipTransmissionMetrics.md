Find Neighbor Slip Transmission Metrics {#findsliptransmissionmetrics}
======

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This filter calculates a suite of *Slip Transmission Metrics* that are related to the alignment of slip directions and planes across **Feature** boundaries.  Equations for the metrics (F1, F1spt, F7 and mPrime) can be found in [1,2].  The algorithm for calculation of these metrics is as follows:

1. Get the average orientation of the **Feature**.
2. Get the **Feature**'s list of neighboring **Features**.
3. Get the average orientation of each neighboring **Feature**.
4. Calculate metrics given by equations in [1,2].
5. Store metrics in lists for the **Feature**.
6. Repeat for all **Features**.

Note: the transmission metrics are calculated using the average orientations for neighboring **Features** and not the local orientation near the boundary.

Note: the metrics are calculated twice (i.e. when **Feature** 1 has neighbor **Feature** 2 and when **Feature** 2 has neighbor **Feature 1) because the direction across the boundary between the **Features** affects the value of the metric. 
  
## Parameters ##
None

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Feature | NeighborLists | List of the contiguous neighboring **Features** for a given **Feature** |  | Find Feature Neighbors (Statistics) |
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**| | Find Feature Phases (Generic), Read Feature Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Feature | AvgQuats | Five (5) values (floats) defining the average orientation of the **Feature** in quaternion representation | Filter will calculate average quaternions for **Features** if not already calculated. | Find Feature Average Orientations (Statistics) |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |

## Created Arrays ##
| Type | Default Name | Comment |
|------|--------------|---------|
| Feature | F1 |  |
| Feature | F1spt |  |
| Feature | F7 |  |
| Feature | mPrime |  |


## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC, 2014 Joseph C. Tucker (UES, Inc)

**Contact Info:** dream3d@bluequartz.net

**Version:** 5.0

**License:**  See the License.txt file that came with DREAM3D.


## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users



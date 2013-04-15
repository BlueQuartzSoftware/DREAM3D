Find Neighbor Slip Transmission Metrics {#findsliptransmissionmetrics}
======

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This filter calculates a suite of *Slip Transmission Metrics* that are related to the alignment of slip directions and planes across **Field** boundaries.  Equations for the metrics (F1, F1spt, F7 and mPrime) can be found in [1,2].  The algorithm for calculation of these metrics is as follows:

1. Get the average orientation of the **Field**.
2. Get the **Field**'s list of neighboring **Fields**.
3. Get the average orientation of each neighboring **Field**.
4. Calculate metrics given by equations in [1,2].
5. Store metrics in lists for the **Field**.
6. Repeat for all **Fields**.

Note: the transmission metrics are calculated using the average orientations for neighboring **Fields** and not the local orientation near the boundary.

Note: the metrics are calculated twice (i.e. when **Field** 1 has neighbor **Field** 2 and when **Field** 2 has neighbor **Field 1) because the direction across the boundary between the **Fields** affects the value of the metric. 
  
## Parameters ##
None

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Field | NeighborLists | List of the contiguous neighboring **Fields** for a given **Field** |
| Field | FieldPhases |  |
| Field | AvgQuats |  |
| Ensemble | FieldPhases |  |

## Created Arrays ##
| Type | Default Name | Comment |
|------|--------------|---------|
| Field | F1 |  |
| Field | F1spt |  |
| Field | F7 |  |
| Field | mPrime |  |


## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.


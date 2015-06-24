Find Neighbor Slip Transmission Metrics {#findsliptransmissionmetrics}
=============

## Group (Subgroup) ##
Statistics (Crystallographic)

## Description ##
This Filter calculates a suite of *slip transmission metrics* that are related to the alignment of slip directions and planes across **Feature** boundaries.  The algorithm for calculation of these metrics is as follows:

1. Get the average orientation of the **Feature**
2. Get the **Feature**'s list of neighboring **Features**
3. Get the average orientation of each neighboring **Feature**
4. Calculate metrics given by equations in *slip transmission metrics*
5. Store metrics in lists for the **Feature**
6. Repeat for all **Features**

*Note:* The transmission metrics are calculated using the average orientations for neighboring **Features** and not the local orientation near the boundary. Also, the metrics are calculated twice (i.e., when **Feature** 1 has neighbor **Feature** 2 and when **Feature** 2 has neighbor **Feature** 1) because the direction across the boundary between the **Features** affects the value of the metric. 
  
## Parameters ##
None

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | NeighborList | List of Ints | (1) | List of the contiguous neighboring **Features** for a given **Feature** |
| Feature | AvgQuats | Float | (4) | Specifies the average orientation of each **Feature** in quaternion representation |
| Feature | Phases | Int | (1) | Specifies to which phase each **Feature** belongs |
| Ensemble | CrystalStructures | Int | (1) | Enumeration representing the crystal structure for each phase |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Face | F1s | Float | (2) | |
| Face | F1spts | Float | (2) | |
| Face | F7s | Float | (2) | |
| Face | mPrimes | Float | (2) | |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users



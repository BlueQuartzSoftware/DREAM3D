Find Feature Boundary Strength Metrics {#findboundarystrengths}
=============

## Group (Subgroup) ##
Statistics (Crystallographic)

## Description ##
This **Filter** calculates the same metrics as in the [Find Neighbor Slip Transmission Metrics](FindSlipTransmissionMetrics.html "") **Filter**.  However, this **Filter** stores the values in the **Face Attribute Matrix** of a **Triangle Geometry**.  The algorithm the **Filter** uses is as follows:

1. Find the two **Features** that are separated by a **Face** in the **Triangle Geometry**
2. Get the average orientation of both of the **Features**
3. Calculate the transmission metrics from both directions (i.e. **Feature** 1 to **Feature** 2 and **Feature** 2 to **Feature** 1)
4. Store the metrics in the **Face** map
5. Repeat for all **Faces**

*Note:* Each metric is calculated twice for the two different directions slip could approach the boundary.  The values are stored on each **Face** in the **Face** map in a way that notes the direction (i.e., when **Feature** 1 has neighbor **Feature** 2 and when **Feature** 2 has neighbor **Feature** 1) because the direction across the boundary between the **Features** affects the value of the metric. 

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Loading Direction | float (3x) | The loading axis for the sample |

## Required Geometry ##
Image + Triangle

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Face Attribute Array** | FaceLabels | int32_t | (2) | Specifies which **Features** are on either side of each **Face** |
| **Feature Attribute Array** | AvgQuats | float | (4) | Specifies the average orientation of each **Feature** in quaternion representation |
| **Feature Attribute Array** | Phases | int32_t | (1) | Specifies to which **Ensemble** each **Feature** belongs |
| **Ensemble Attribute Array** | CrystalStructures | uint32_t | (1) | Enumeration representing the crystal structure for each phase |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Face Attribute Array** | F1s | float | (2) | |
| **Face Attribute Array** | F1spts | float | (2) | |
| **Face Attribute Array** | F7s | float | (2) | |
| **Face Attribute Array** | mPrimes | float | (2) | |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)



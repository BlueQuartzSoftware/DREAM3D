Find Surface Area to Volume {#findsurfaceareatovolume}
=============

## Group (Subgroup) ##
Statistics (Morphological)

## Description ##
This **Filter** calculates the ratio of surface area to volume for each **Feature** in an **Image Geometry**. First, all the boundary **Cells** are found for each **Feature**. Next, the surface area for each face that is in contact with a different **Feature** is totalled. This number is divided by the volume of each **Feature**, calculated by taking the number of **Cells** of each **Feature** and multiplying by the volume of a **Cell**. 

*Note:* The surface area will be the surface area of the **Cells** in contact with the neighboring **Feature** and will be influenced by the aliasing of the structure.  As a result, the surface area to volume will likely be over-estimated with respect to the *real* **Feature**.

## Parameters ##
None

## Required Geometry ##
Image

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs |
| **Feature Attribute Array** | NumCells |  int32_t | (1) | Number of **Cells** that are owned by the **Feature**. This value does not place any distinction between **Cells** that may be of a different size |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Feature Attribute Array** | SurfaceAreaVolumeRatio | float | (1) | Ratio of surface area to volume for each **Feature**. The units are inverse length |


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)



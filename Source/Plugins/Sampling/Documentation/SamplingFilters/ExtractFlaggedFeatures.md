Extract Flagged Features (Rogues Gallery) {#extractflaggedfeatures}
=============

## Group (Subgroup) ##
Sampling (Cropping/Cutting)

## Description ##
This **Filter**, using a boolean array identifying **Features** to be *extracted*, crops out the smallest bounding box around each **Feature** of interest.  First, the **Filter** determines the bounding box ((xMin-xMax), (yMin-yMax), (zMin-zMax)) for each **Feature**.  Then, the **Filter** checks to see if the **Feature** has been "flagged" for extraction.  If the **Feature** is to be extracted, the bounding box is used to define a volume for cropping.  The cropped volume for each extracted **Feature** is stored as a new **Data Container**.  The cropped volumes will have their origins "updated" to ensure that the extracted **Features** remain in the same absolute position relative to each other.

The utility of this **Filter** is that complex thresholding based on **Feature** attributes can be difficult in ParaView (due to memory) and this can greatly reduce the amount of information that is loaded if only the extracted **Feature Data Containers** are loaded for viewing. 

*Note:* Storing each extracted volume as a new **Data Container** can result in a large number of **Data Containers** depending on how many **Features** are "flagged". 

## Parameters ##
None

## Required Geometry ##
Image 

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs |
| **Feature Attribute Array** | Active | bool | (1) | Specifies whether each **Feature** should be *extracted* or not |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


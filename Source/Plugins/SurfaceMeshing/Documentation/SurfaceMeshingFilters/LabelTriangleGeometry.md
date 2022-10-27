# Label Triangle Geometry #

## Group (Subgroup) ##

Surface Meshing (Geometry)

## Description ##

This **Filter** accepts a **Triangle Geometry** and checks the connectivity of **Triangles** and assigns them **region ID**s in a mask accordingly as well as providing a count for the number of **Triangles** in each **Region**.

## Required Geometry ###

**Triangle Geometry**

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Data Container** | CAD Geometry | **Triangle Geometry** | N/A | This is the **Triangle Geometry** to be operated on. |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Face Attribute Array** | Region Ids | int32_t | (1)/(1) | The mask list for **SharedTriList** that holds traingles respective **Region ID**. |
| **Attribute Matrix** | Cell Feature Attribute Matrix | FaceFeature | (1)/(0)  | A holding container for **NumTriangles** and respective masks. |
| **FaceFeature Attribute Array** | NumTriangles | int64_t | (1)/(1) | This is a mask for each **Region ID** that contains the amount of **Triangles** in each **Region**. |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users
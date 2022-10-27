# Remove Flagged Triangles #

## Group (Subgroup) ##

Surface Meshing (Misc)

## Description ##

This **Filter** removes **Triangles** from the supplied **Triangle Geometry** that are flagged by a boolean mask array.  Specifically, **Triangles** flagged as _true_ are removed from the **Geometry**.  A new reduced **Triangle Geometry** is created that contains all the remaining **Triangles**.  It is unknown until run time how many **Triangles** will be removed from the **Geometry**.  Therefore, this **Filter** requires that a new **Data Container** be created to contain the reduced **Triangle Geometry**.  This new **Data Container** will NOT contain copies of any **Feature** or **Ensemble** **Attribute Matrices** from the original **Data Container**.  Additionally, all **Vertex** data will be copied, with tuples _removed_ for any **Vertices** removed by the **Filter**.  The user must supply a name for the reduced **Data Container**.

_Note:_ Since it cannot be known before run time how many **Vertices** will be removed, the new **Vertex Geometry** and all associated **Vertex** data to be copied will be initialized to have size 0.

## Parameters ##

None

## Required Geometry ###

**Triangle Geometry**

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Data Container** | TriangleDataContainer | N/A | N/A | **Data Container** holding the **Triangle Geometry** to reduce |
| **RegionIDs Attribute Array** | Region IDs | int32_t | (1) | A **DataArray** containing the **Region IDs** for each 
**Triangle** |
| **Triangle Attribute Array** | Mask | bool | (1) | Mask array specifying which **Triangles** to remove |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| Reduced **Data Container** | ReducedVertexDataContainer | N/A | N/A | **Data Container** holding the reduced **Vertex Geometry** |

## Example Pipelines ##



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:

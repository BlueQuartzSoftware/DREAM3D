Generate Geometry Connectivity {#generategeometryconnectivity}
============

## Group (Subgroup) ##
Surface Meshing (Connectivity/Arrangement)

## Description ##
This **Filter** will generate the following connectivity data structures:

+ _Per Vertex Element List_: For each **Vertex** in the **Geometry**, a list of triangles to which the **Vertex** belongs

+ _Per Element Neighbors List_: For each primary unit **Element** in the **Geometry**, a list of **Elements** that share a common edge. If the mesh is non-manifold, each **Element** may have more than neighbors than **Edges**

Note that the resulting lists are stored with the **Geometry** object itself, not as separate **Attribute Arrays**. Some **Geometries**, such as a **Vertex Geometry**, may not have implemented the necessary connectivity functions, and will trigger an error when running the **Filter**.

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Generate Per Vertex Element List | bool | Whether to generate the per vertex element list |
| Generate Element Neighbors List | bool | Whether to generate the element neighbors list |

## Required Geometry ##
Any

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Data Container** | None | N/A | N/A | **Data Container** that holds the **Geometry** for which to generate the connectivity |

## Created Objects ##
The resulting connectivity lists are stored with the **Geometry** objects, _not_ in any separate **Attribute Arrays**.


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)



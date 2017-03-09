Insert Atoms {#insertatoms}
=============

## Group (Subgroup) ##
Synthetic Building (Packing)

## Description ##
This **Filter** creates a **Vertex** geometry where the **Vertices** represent atom positions.  The **Filter** requires a **Triangle** surface mesh that bounds **Features** of different crystallographic orientation.  The **Filter** then extracts the **Triangles** associated with each **Feature** and in parallel inserts atoms on the crystal lattice of each **Feature**.  The steps associated with inserting the atoms in each **Feature** are as follows:

1. Transform the coordinates of the **Triangles** into the reference frame of the **Feature's** crystallographic orientation using its stored orientation
2. Determine the minimum and maximum X, Y and Z coordinate of the transformed **Triangles**
3. Generate a grid of points starting at the minimum (X,Y,Z) coordinate using the lattice constants entered (with a||x, b||y and c||z) until reaching the maximum (X,Y,Z) coordinate. Add points at the proper positions given the crystal basis choosen by the user 
4. Transform list of points generated in 3 into the original **Triangle** reference frame using the inverse of the **Feature**'s crystallographic orientation
5. Check whether each point in the list falls inside of the n-sided polyhedron defined by the **Triangles** that bound the **Feature**.  Remove the point from the list if it falls outside of the **Feature**'s bounds. Assign the **Feature**'s number to the point if it does fall inside of the **Feature**

After all **Features** have had atoms inserted, combine the point lists for all the **Features**.

*Note:* Since each **Feature** is treated independently (in parallel), the interface between neighboring **Features** may not be "in equilibrium".  For example, at one point along the interface, each of the neighboring **Features** may have an atom fall just slightly outside its bounds.  In this case, there may not be an atom on the "ideal" lattice for both **Features**, but maybe there should be a single atom that sits at the midpoint between the two ideal positions.  The algorithm will instead just omit any atom from that area.

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Lattice Constants (Angstroms) | float (x3) | Lattice parameters (a, b, c) for the unit cell in Angstroms |
| Crystal Basis | Enumeration | Basis to be used when inserting atoms on lattice (currently Siple Cubic, Body-Centered Cubic and Face-Centered Cubic are available) |

## Required Geometry ##
Triangle

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Face Attribute Array** | FaceLabels | int32_t | (2) | Specifies which **Features** are on either side of each **Face** |
| **Feature Attribute Array** | AvgQuats | float | (4) | Specifies the average orientation of the **Feature** in quaternion representation |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Data Container** | VertexDataContainer | N/A | N/A | Created synthetic volume **Data Container** name with **Vertex Geometry** |
| **Attribute Matrix** | VertexData | Vertex | N/A | Created **Vertex Attribute Matrix** name |
| **Vertex Attribute Array** | AtomFeatureLabels | int32_t | (1) | Specifies to which **Feature** each **Vertex** (or atom) belongs. |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


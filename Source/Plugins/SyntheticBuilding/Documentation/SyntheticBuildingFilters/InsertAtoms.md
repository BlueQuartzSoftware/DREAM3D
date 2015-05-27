Insert Atoms {#insertatoms}
=============

## Group (Subgroup) ##
Synthetic Builder Filters (Packing)

## Description ##
This Filter creates a **Vertex** geometry where the **Vertices** represent atom positions.  The Filter requires a **Triangle** surface mesh that bounds **Features** of different crystallographic orientation.  The Filter then extracts the **Triangles** associated with each **Feature** and in parallel inserts atoms on the crystal lattice of each **Feature**.  The steps associated with inserting the atoms in each **Feature** are as follows:

1. Transform the coordinates of the **Triangles** into the reference frame of the **Feature**'s crystallographic orientation using its stored orientation.
2. Determine the minimum and maximum X, Y and Z coordinate of the transformed **Triangles**.
3. Generate a grid of points starting at the minimum (X,Y,Z) coordinate using the lattice constants entered (with a||x, b||y and c||z) until reaching the maximum (X,Y,Z) coordinate. Add points at the proper positions given the crystal basis choosen by the user. 
4. Transform list of points generated in 3 into the original **Triangle** reference frame using the inverse of the **Feature**'s crystallographic orientation.
5. Check whether each point in the list falls inside of the n-sided polyhedron defined by the **Triangles** that bound the **Feature**.  Remove the point from the list if it falls outside of the **Feature**'s bounds. Assign the **Feature**'s number to the point if it does fall inside of the **Feature**.

After all **Features** have had atoms inserted, combine the point lists for all the **Features**.

*Note:* Since each **Feature** is treated independently (in parallel), the interface between neighboring **Features** may not be "in equilibrium".  For example, at one point along the interface, each of the neighboring **Features** may have an atom fall just slightly outside its bounds.  In this case, there may not be an atom on the "ideal" lattice for both **Features**, but maybe there should be a single atom that sits at the midpoint between the two ideal positions.  The algorithm will instead just omit any atom from that area.

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| LatticeConstants | Float (x3) | Lattice parameters (a,b,c) for the unit cell in Angstroms |
| Crystal Basis | Choice | Basis to be used when inserting atoms on lattice (currently Siple Cubic, Body-Centered Cubic and Face-Centered Cubic are available) |

## Required Geometry ##
Triangle

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | AvgQuats | Float | (4) | Defines the average orientation of the **Feature** in quaternion representation  (<x,y,z>, w) |
| Triangle | SurfaceMeshFaceLabels | Int | (2) | Specifies which **Features** lie on either side of the **Triangle**.  The first value is the **Feature** the normal of the **Triangle** points towards using the right-hand rule for the given winding.  The second value is the **Feature** the normal points away from. |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Vertex | AtomFeatureLabels | Int | (1) | Specifies to which **Feature** each **Vertex** (or atom) belongs. |

## Authors ##
**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.




## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


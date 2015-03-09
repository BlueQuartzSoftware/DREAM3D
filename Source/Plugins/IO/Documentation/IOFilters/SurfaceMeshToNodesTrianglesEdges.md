Write Nodes/Triangles/Edges from Surface Mesh {#surfacemeshtonodestrianglesedges}
======

## Group (Subgroup) ##
IO Filters

## Description ##

This filter creates 3 files from the Nodes, Triangles and Edges data from a SurfaceMesh DataContainer. The three files are Nodes, Triangles and Edges:

**Nodes File**

The nodes file format is as follows:
-  first line = number of nodes
-  column 1 = node id, starts from zero.
-  column 2 = node type,
-  column 3 to 5 = coordinates of nodes, x, y, and z

**Node Types**

- 2 = on the **Feature** boundary
- 3 = on the triple line,
- 4 = quadruple point or on quadruple line if 10 is added, eg 12, 13, and 14, then they are on the surface of microstructure.
For the nodes on surfaces of microstructure, negative wrapper spins are treated as neighboring **Features**.
- 12 = on the **Feature** boundary trace area at the surface of microstructure
- 13 = on the **Feature** boundary trace line
- 14 = on the triple point, and so on

**Triangles File**
The triangles file format is as follows:

-  first line = number of triangles
-  column 1 = triangle id, starts from zero
-  column 2 to 4 = node1, node2 and node3 of individual triangles
-  column 5 to 7 = edge1 (from node1 and node2), edge2 (from node2 and node3) and edge3 (from node3 and node1) of individual triangle
-  column 8 and 9 = neighboring spins of individual triangles, column 8 = spins on the left side when following winding order using right hand.

**Edges File**

The edges file format is as follows:
-  first line = number of edges
-  column 1 = edge id, starts from zero
-  column 2 and 3 = node ids at the end of individual edges
-  column 4 = edge type, number of neighboring spins, including negative wrapper spins.
              if edge is on the surface of microstructure, 10 is added --> 12, 13, 14 and so on.
- column 5 to 8 = neighboring spins, zero is a dummy.


## Parameters ##

| Name | Type |
|------|------|
| Output Nodes File | Output File Path |
| Output Edges File | Output File Path |
| Output Triangles File | Output File Path |

## Required DataContainers ##
SurfaceMesh

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| NodeArray Attribute | SurfaceMeshNodeType | The node types values |   | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |
| FaceArray Attribute | SurfaceMeshTriangleLabels | The **Feature** Id Labels for each Face(Triangle) |   | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |
| NodeArray | | The shared node (vertex) list |   | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |
| FaceArray | | The list of all triangles (faces) in the mesh |   | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |
| EdgeArray | | The list of unique edges in the mesh |   | Generate Surface Mesh Connectivity (SurfaceMeshing) | 


## Created Arrays ##
None


## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


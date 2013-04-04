Write Nodes/Triangles/Edges from Surface Mesh {#surfacemeshtonodestrianglesedges}
======

## Group ##
IO Filters

## Description ##

This filter creates 3 files from the Nodes, Triangles and Edges data from a SurfaceMesh 
DataContainer. The three files are Nodes, Triangles and Edges:
The nodes file format is as follows:

-  first line = number of nodes
-  column 1 = node id, starts from zero.
-  column 2 = node type, 
	+ 2 = on the grain boundary
	+ 3 = on the triple line,
	+ 4 = quadruple point or on quadruple line if 10 is added, eg 12, 13, and 14, then they are on the surface of microstructure. 

For the nodes on surfaces of microstructure, negative wrapper spins are treated as neighboring grains. 
	+ 12 = on the grain boundary trace area at the surface of microstructure
	+ 13 = on the grain boundary trace line
	+ 14 = on the triple point, and so on
-  column 3 to 5 = coordinates of nodes, x, y, and z

The triangles file format is as follows:

-  first line = number of triangles
-  column 1 = triangle id, starts from zero
-  column 2 to 4 = node1, node2 and node3 of individual triangles
-  column 5 to 7 = edge1 (from node1 and node2), edge2 (from node2 and node3) and edge3 (from node3 and node1) of individual triangle
-  column 8 and 9 = neighboring spins of individual triangles, column 8 = spins on the left side when following winding order using right hand.

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
| Output Nodes File | Output File |
| Output Edges File | Output File |
| Output Triangles File | Output File |

## Required Arrays ##



## Created Arrays ##

| Type | Name | Comment |
|------|------|---------|
## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.




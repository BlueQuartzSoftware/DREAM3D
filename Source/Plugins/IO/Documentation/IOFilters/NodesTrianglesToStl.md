Nodes & Triangles To Stl File {#nodestrianglestostl}
======

## Group (Subgroup) ##
IO Filters

## Description ##
This filter will convert a pair of triangle and nodes files commonly used by CMU in their GBCD studies and output an STL File in Binary format for each unique **Feature** Id.

The nodes file format is as follows:

+  first line = number of nodes
+  column 1 = node id, starts from zero.
+  column 2 = node type,
  + 2 = on the **Feature** boundary
  + 3 = on the triple line,
  + 4 = quadruple point or on quadruple line if 10 is added, eg + 12, 13, and 14, then they are on the surface of microstructure.

For the nodes on surfaces of microstructure, negative wrapper spins are treated as neighboring **Features**.

+ 12 = on the **Feature** boundary trace area at the surface of microstructure
+ 13 = on the **Feature** boundary trace line
+ 14 = on the triple point, and so on
  - column 3 to 5 = coordinates of nodes, x, y, and z


The triangles file format is as follows:

-  first line = number of triangles
-  column 1 = triangle id, starts from zero
-  column 2 to 4 = node1, node2 and node3 of individual triangles
-  column 5 to 7 = edge1 (from node1 and node2), edge2 (from node2 and node3) and edge3 (from node3 and node1) of individual triangle
-  column 8 and 9 = neighboring spins of individual triangles, column 8 = spins on the left side when following winding order using right hand.

## Parameters ##

| Name | Type |
|------|------|
| Nodes File | The input nodes file |
| Triangles File | The input triangles file |
| Output STL Directory | The output Stl Directory |
| Output STL File Prefix | The prefix that will be added before the id number |


## Required DataContainers ##
SurfaceMesh

## Required Arrays ##
None

## Created Arrays ##
None

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


Convert Nodes & Triangles To STL Files {#nodestrianglestostl}
=============

## Group (Subgroup) ##
IO (Output)

## Description ##
This Filter will convert a pair of triangle and nodes files into an output STL File in Binary format for each unique **Feature** Id.  The input file formats were originally defined by Prof. Greg Rohrer (CMU), and are commonly used in studying Grain Boundary Character Distribution (GBCD). 

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

| Name | Type | Description |
|------|------|-------------|
| Nodes File | File Path | The input nodes file path |
| Triangles File | File Path | The input triangles file path |
| Output STL Directory | File Path | The output Stl directory path |
| STL File Prefix | String | The prefix that will be added to the STL files number |

## Required Geometry ##
Not Applicable

## Required Arrays ##
None

## Created Arrays ##
None


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users



Nodes & Triangles To Vtk File {#nodestrianglestovtk}
=============

## Group (Subgroup) ##
IO (Output)

## Description ##
This Filter will convert a pair of triangle and nodes files into a VTK PolyData file in either ASCII or binary format. Note if the file is in binary that the binary portions are written in big endian format as specified by the VTK file format. The input file formats were originally defined by Prof. Greg Rohrer (CMU), and are commonly used in studying Grain Boundary Character Distribution (GBCD). 

The nodes file format is as follows:

    +  first line = number of nodes
    +  column 1 = node id, starts from zero.
    +  column 2 = node type,
      + 2 = on the **Feature** boundary
      + 3 = on the triple line,
      + 4 = quadruple point or on quadruple line if 10 is added, eg 12, 13, and 14, then they are on the surface of microstructure.

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

If the user selects to __disable__ the **Conformal Mesh** option then 2 triangles will be written to the file and the scalar data will have 2 entries for the each **Feature**Id, one for each **Feature** that the triangle borders.

### Example Output ###
The vtk ASCII output file:     

    # vtk DataFile Version 2.0
    Data set from DREAM.3D Surface Meshing Module
    ASCII
    DATASET POLYDATA
    POINTS 346727 float
    -36.750000 10.250000 -29.000000
    -36.750000 10.500000 -29.000000
    -36.750000 10.250000 -28.750000
    -36.750000 10.500000 -28.750000
    -36.500000 10.250000 -29.000000
    -36.500000 10.250000 -28.750000
    -36.500000 10.500000 -29.000000
    -36.250000 10.250000 -29.000000
    -36.250000 10.250000 -28.750000
    -36.250000 10.500000 -29.000000
    -36.000000 10.250000 -29.000000
    -36.000000 10.250000 -28.750000
    -36.000000 10.500000 -29.000000
    -35.750000 10.250000 -29.000000
    …..


## Parameters ##
| Name | Type | Description |
|------|------|-------------|
| Nodes File | File Path | The input nodes file path |
| Triangles File | File Path | The input triangles file path |
| Output VTK File | File Path | The output VTK PolyData file path |
| Binary VTK File | Boolean | Whether to write the VTK file using binary instead of ASCII |
| Conformal Mesh | Boolean | Whether to write a conformal mesh |

## Required Geometry ##
Not Applicable

## Required Objects ##
None

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)



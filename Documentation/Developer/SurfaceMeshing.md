

surfacemeshing Surface Meshing


## sm1  Intermediate Files

The surface meshing module creates some intermediate files that are written in
binary form to help keep the memory to a minimum during the meshing process. Those
files are the _Nodes_ file and the _Triangles_ file. The format of each
one is as follows:

### sm1 Nodes File

The nodes file is made up of n number of node records where each node has the following
fields:

**The total byte count of each Node Record is 20 bytes**

  <table border>
  <tr>
      <td>** Node Id **</td>
      <td>** Node Kind **</td>
      <td>** X Position **</td>
      <td>** Y Position **</td>
      <td>** Z Position **</td>
   </tr>

   <tr>
      <td>4 Byte Unsigned Int</td>
      <td>4 Byte Unsigned Int</td>
      <td>4 Byte Floating Point</td>
      <td>4 Byte Floating Point</td>
      <td>4 Byte Floating Point</td>
   </tr>
 </table>

### sm3 Triangle File

The Triangle file is made up of Triangle Records where each record is make up of
the following fields:

  <table border>
  <tr>
      <td>** Triangle Id **</td>
      <td>** The first Node's index into the Nodes Array **</td>
       <td>** The second Node's index into the Nodes Array **</td>
   <td>** The third Node's index into the Nodes Array **</td>
      <td>** Grain ID 1 **</td>
      <td>** Grain ID 2 **</td>
   </tr>

   <tr>
      <td>4 Byte Unsigned Int</td>
      <td>4 Byte Unsigned Int</td>
      <td>4 Byte Unsigned Int</td>
      <td>4 Byte Unsigned Int</td>
      <td>4 Byte Unsigned Int</td>
      <td>4 Byte Unsigned Int</td>
   </tr>
 </table>

**The total byte count for each Triangle Record is 24 bytes**

Each Triangle is shared between 2 grains.


## sm_input Input File Format
The Surface Meshing module takes vtk structured grid files as input in either
ASCII or Binary formats. Although the VTK file format can be flexible the current
reader implementation being used is NOT. The following details the exact layout
of the header section of the VTK file. The Input file **must** share this exact
layout.
### ascii_input_header ASCII VTK File Header
@code
1: # vtk DataFile Version 2.0
2: NRL TiBeta Stack
3: BINARY
4: DATASET STRUCTURED_POINTS
5: DIMENSIONS 1670 770 201
6: ORIGIN 0.00 0.00 0.00
7: SPACING 0.665 0.665 1.48
8: POINT_DATA 258465900
9:
10: SCALARS GrainID int  1
11: LOOKUP_TABLE default
12...: [ Data ]
@endcode

### binary_input_header
@code
1: # vtk DataFile Version 2.0
2: data set from FFT2dx_GB
3: ASCII
4: DATASET STRUCTURED_POINTS
5: DIMENSIONS 189 201 100
6: ORIGIN 0.0 0.0 0.0
7: SPACING 0.25 0.25 0.25
8: POINT_DATA 3798900
9:
10: SCALARS GrainID int  1
11: LOOKUP_TABLE default
12: [ Data ]
@endcode




 

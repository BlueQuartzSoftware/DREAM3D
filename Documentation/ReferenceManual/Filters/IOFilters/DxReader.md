Read Dx File (Field Ids) {#dxreader}
======
## Group (Subgroup) ##
I/O Filters (Input)

## Description ##

~~~~~~~~~~~~~~~
This Filter Reads from a Dx file. This is a file in a format generated using the _OpenDX Source Software_ based on IBM's _Visualization Data Explorer_. 
An example of the format is show below:
object 1 class gridpositions counts 6 5 4
origin 0 0 0
delta  1 0 0
delta  0 1 0
delta  0 0 1

object 2 class gridconnections counts 6 5 4

object 3 class array type int rank 0 items 60 data follows
100 101 102 
103 104 105 
106 107 108 
109 110 111 
112 113 114 
115 116 117 
118 119 120 
121 122 123 
124 125 126 
127 128 129 
130 131 132 
133 134 135 
136 137 138 
139 140 141 
142 143 144 
145 146 147 
148 149 150 
151 152 153 
154 155 156 
157 158 159 

attribute "dep" string "connections"

object "DREAM3D Generated" class field
component  "positions"    value 1
component  "connections"  value 2
component  "data"         value 3

end
~~~~~~~~~~~~~~~

_See: www.opendx.org for more information._


## Parameters ##

| Name | Type |
|------|------|
| Input File | File Path |
| Resolution | X, Y, Z Resolution of the data |
| Origin     | The location in space coordinates of the (0,0,0) voxel |

## Required DataContainers ##
Voxel

## Required Arrays ##
None

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |



## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


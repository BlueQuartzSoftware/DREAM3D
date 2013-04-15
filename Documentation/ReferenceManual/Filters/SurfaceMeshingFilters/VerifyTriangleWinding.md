Verify Triangle Winding {#verifytrianglewinding}
======

## Group (Subgroup) ##
SurfaceMesh

## Description ##
This filter analyzes the mesh for consistent triangle winding and fixes any inconsistencies that are found. This algorithm can be memory intensive for larger meshes. The algorithm is an advancing wave front type of algorithm. The algorithm is know to have problems if there are disconnected **Fields** with the same ID in the surface mesh.


## Parameters ##
None

## Required DataContainers ##
SurfaceMesh - Valid Surface Mesh containing the shared vertex array and face list

## Required Arrays ##
None

## Created Arrays ##
None



## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




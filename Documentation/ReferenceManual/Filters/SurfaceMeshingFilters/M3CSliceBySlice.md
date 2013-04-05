Multi-Material Marching Cubes (Slice at a Time) {#m3cslicebyslice}
======

## Group (Subgroup) ##
SurfaceMesh

## Description ##
This filter creates a surface mesh using a MultiMaterial Marching Cubes (M3C) algorithm originally developed at Carnegie-Mellon University by Dr. Sukbin Lee in the Materials Engineering department based on the Wu/Sullivan algorithm. Heavy modifications were performed by M. Groeber and M. Jackson for the DREAM3D project.

This version of the code meshes by looking at 2 slices of **Cells** at a time. The temporary data is then serialized out to disk and is then gathered into the complete shared vertex list and triangle list at the conclusion of the filter. The ramifications of this means that the working amount of RAM during the main part of the algorithm is much lower than the _Volume at a Time_ version of the M3C algorithm but does involve potentially a large amount of disk activity. At the conclusion of the filter the entire mesh is then read into memory which means that the user's computer must still have enough RAM to hold the final mesh in memory.
 
This version of the code does not have any restrictions on the wrapping of the **Cell** volume with a ghost layer of **Cells**. If the user's volume does have a ghost layer then those **Cells** should have a value that is __NEGATIVE__. This is very important as the algorithm that determines if a layer needs to be added looks specifically for negative values along the outside of the volume. __Other Considerations__ If you have created your **Cell** volume outside of DREAM3D and have imported it into DREAM3D then the user should take note that **Field**/regions with an ID=0 are a special case inside of DREAM3D therefor the user should start their **Field** numbering from 1 and be contiguous in numbers to the maximum number of **Fields**. An effort is made to renumber **Cells** with a value of Zero (0) to Max + 1 during the meshing and then the **Cell** array is reset back to its pre-surface meshing input.
 
The values for the __Node Type__ array can take one of the following values.

    namespace SurfaceMesh {<br/>
      namespace NodeType {<br/>
        const int8_t Unused = -1;<br/>
        const int8_t Default = 2;<br/>
        const int8_t TriplePoint = 3;<br/>
        const int8_t QuadPoint = 4;<br/>
        const int8_t SurfaceDefault = 12;<br/>
        const int8_t SurfaceTriplePoint = 13;<br/>
        const int8_t SurfaceQuadPoint = 14;<br/>
      }
    }

## Parameters ##

| Name | Type |
|------|------|
| Delete Temp Files | Boolean: Should the temporary files that are generated be deleted at the end of the filter. This is mostly for debugging. |

## Required Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | GrainIds |  |

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Vertex Array | SurfaceMeshNodes | The shared list of nodes that make up the mesh |
| Triangle Array | SurfaceMeshTriangles | The List of triangles in the Surface Mesh |
| Vertex Attribute Array | SurfaceMeshNodeType | N x 1 Col of unsigned char |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.




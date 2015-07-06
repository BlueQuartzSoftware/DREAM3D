Moving Finite Element Smoothing {#movingfiniteelementsmoothing}
======

## Group (Subgroup) ##
SurfaceMesh

## Description ##
_Kuprat, A., 2000. Modeling microstructure evolution using gradient-weighted moving
finite elements. SIAM Journal of Scientific Computing 22, 535-560._

The Moving Finite Element (MFE) algorithm is based on Kuprat's work on Gradient Weighted Moving Finite Element method [SIAM].  A conformal triangular surface mesh represents the **Feature** boundary network.  The curvature at each node is computed and used as the driving force for nodal motion. Additional forces are computed based on element quality. The user can choose a balance between the curvature and element quality forces to find a compromise between smoothness and final element quality.  Certain node types can be constrained in their motion.
 
- Node Contraints .. 
-  Nodes on an external surface can be constrained to move in the plane of the surface.
-  Triple Lines can be constrained
-  Quad point nodes can be prevented from moving.
 


## Parameters ##

| Name | Type |
|------|------|
| Iteration Steps | Integer |
| Apply Node Contraints | Boolean (On or Off) |
| Constrain Surface Nodes | Boolean (On or Off) |
| Constrain Quad Points | Boolean (On or Off) |
| Smooth Triple Lines | Boolean (On or Off) |


## Required DataContainers ##
SurfaceMesh - Valid Surface Mesh containing the shared vertex array and face list

## Required Objects ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Vertex | SurfaceMeshNodeType | N x 1 Col of unsigned char |  | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |


## Created Objects ##
None

## Authors ##






## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)



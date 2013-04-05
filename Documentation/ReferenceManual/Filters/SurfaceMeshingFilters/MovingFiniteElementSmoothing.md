Moving Finite Element Smoothing {#movingfiniteelementsmoothing}
======

## Group (Subgroup) ##
SurfaceMesh

## Description ##
_Kuprat, A., 2000. Modeling microstructure evolution using gradient-weighted moving
finite elements. SIAM Journal of Scientific Computing 22, 535-560._
The Moving Finite Element (MFE) algorithm is based on Kuprat's work on Gradient Weighted Moving Finite Element method [SIAM].  A conformal triangular surface mesh represents the **Field** boundary network.  The curvature at each node is computed and used as the driving force for nodal motion. Additional forces are computed based on element quality. The user can choose a balance between the curvature and element quality forces to find a compromise between smoothness and final element quality.  Certain node types can be constrained in their motion.
 
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


## Required Arrays ##

| Type | Name | Comment |
|------|------|---------|
| Triangle List | Core Property of the Surface Mesh Data Container | The List of triangles in the Surface Mesh |
| Node List | Core Property of the Surface Mesh Data Container | The shared list of nodes that make up the mesh |
| Node Type | Optional | The array that describes the type of node. (See table in description above) |

## Created Arrays ##
None

## Authors ##

**Copyright** 2012 Dr. Anthony Rollet, Dr. Jason Gruber, Dr. Stephen Sintay, Dr. Sukbin Lee

**Contact Info:** Carnegie-Mellon University, School of Materials Engineering

**Version:** 1.0.0

**License:** BSD


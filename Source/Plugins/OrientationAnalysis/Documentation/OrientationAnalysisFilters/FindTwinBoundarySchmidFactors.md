Find Twin Boundary Schmid Factors {#findtwinboundaryschmidfactors}
==========

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This filter calculates the *Schmid Factors* for the 3 slip systems in the plane of each twin boundary.  The filter only calculates the values for **Faces** already identified as twins.

## Parameters ##
| Output File |  |
| Loading Direction | 3 x float |

## Required DataContainers ##
+ Voxel
+ SurfaceMesh


## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Face   | SurfaceMeshFaceLabels | N x 2 Col of signed integer |  | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |
| Face   | SurfaceMeshFaceNormals | N x 3 Col of floats |  | Generate Triangle Normals Filter (SurfaceMeshing) |
| Feature (Voxel) | AvgQuats | Five (5) values (floats) defining the average orientation of the **Feature** in quaternion representation | Filter will calculate average quaternions for **Features** if not already calculated. | Find Feature Average Orientations (Statistics) |
| Feature (Voxel) | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**| | Find Feature Phases (Generic), Read Feature Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Ensemble (Voxel) | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Face   | SurfaceMeshTwinBoundarySchmidFactors | 3 Schmid factors on the plane of the twin boundary for the user defined loading direction |  |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users



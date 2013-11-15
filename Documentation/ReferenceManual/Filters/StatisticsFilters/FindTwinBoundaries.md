Find Twin Boundaries {#findtwinboundaries}
==========

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This filter identifies all **Faces** between neighboring **Features** that have a Sigma 3 twin relationship.  The filter uses the average orientation of the **Features** on either side of the **Face** to determine the *misorientation* between the **Features**.  If the *axis-angle* that describes the *misorientation* is within a user-defined tolerance, then the **Face** is flagged as being a twin.  After the **Face** is flagged as a twin, the crystal direction parallel to the **Face** normal is determined and compared with the *misorientation axis*.  The misalignment of these two crystal directions is stored as the incoherence value for the **Face** (the value is in degrees).   

## Parameters ##

| Name | Type |
|------|------|
| Axis Tolerance | Double |
| Angle Tolerance | Double |

## Required DataContainers ##
+ Voxel
+ SurfaceMesh


## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Face   | SurfaceMeshFaceLabels | N x 2 Col of signed integer |  | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |
| Face   | SurfaceMeshFaceNormals | N x 3 Col of floats |  | Generate Triangle Normals Filter (SurfaceMeshing) |
| Face   | SurfaceMeshTwinBoundary | boolean value equal to 1 for twin and 0 for non-twin |  | Find Twin Boundaries (Statistics) |
| Feature (Voxel) | AvgQuats | Five (5) values (floats) defining the average orientation of the **Feature** in quaternion representation | Filter will calculate average quaternions for **Features** if not already calculated. | Find Feature Average Orientations (Statistics) |
| Feature (Voxel) | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**| | Find Feature Phases (Generic), Read Feature Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Ensemble (Voxel) | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Face   | SurfaceMeshTwinBoundary | boolean value equal to 1 for twin and 0 for non-twin |  |
| Face   | SurfaceMeshTwinBoundaryIncoherence | Angle in degrees between crystal direction parallel to **Face** normal and misorientation axis |  |

## Authors ##

**Copyright:** 2013 Michael A. Groeber (AFRL)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


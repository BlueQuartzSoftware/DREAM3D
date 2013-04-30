Find Field Reference C-Axis Misorientations {#findgrainreferencecaxismisorientations}
======

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This filter is essentially the same as the [Find Field Reference Misorientations](FindGrainReferenceMisorientations.html "") filter, but with the misalignment between c-axes of interest, rather than full misorientations.  The average c-axis orientation for each **Field** is compared with the c-axis orientation of each of the **Cells** within the **Field**. 

## Parameters ##
None

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Fields (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Cell | Quats | Five (5) values (floats) that specify the orientation of the **Cell** in quaternion representation | Filter will calculate the quaternion for each **Cell** if it is not already calculated. | Find Cell Quaternions (Generic) |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |
| Field | AvgCAxes | This array is the direction <u,v,w> of the c-axis in the sample reference frame |  | Find Average C-Axis Orientations (Statistics) |

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | GrainReferenceCAxisMisorientations |  |
| Field | GrainAvgCAxisMisorientations |  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




Match Crystallography {#matchcrystallography}
======

## Group (Subgroup) ##
Synthetic Builder Filters (Crystallography)

## Description ##
This Filter iteratively either swaps out the orientation of a **Feature** (selected at random) for a new orientation (sampled from the goal Orientation Distribution Function) or switches the orientations of two **Features** (selected at random).  The switch or swap is accepted if it lowers the error of the current ODF and Misorientation Distribution Function from the goal.  This is done for a user defined number of iterations.  

## Parameters ##

| Name | Type |
|------|------|
| Maximum Number of Iterations (Swaps) | Integer |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**| | Find Feature Phases (Generic), Read Feature Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Feature | SurfaceFeatures |  |  | Find Surface Features (Generic)
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |
| Ensemble | NumFeatures |  |  | Find Number of Features (Statistics) |
| Ensemble | Statistics |  |  | Generate Ensemble Statistics (Statistics), StatsGenerator Application |

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | CellEulerAngles |  |
| Feature | AvgQuats |  |
| Feature | FeatureEulerAngles |  |
| Feature | Volumes |  |
| Ensemble | CrystalStructures | This filter copies over the crystal structures from the Stats file into a new crystal structures array in the ensemble attribute matrix for the synthetic structure. The user currently has no choice in the name of this array|

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


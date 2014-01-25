Find Deformation Statistics {#finddeformationstatistics}
======

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This Filter looks for correlations between various local deformation metrics (i.e. Kernel Average Misorientation (KAM) of Grain Reference Misorientation (GRM)) and distance from a **Feature** boundary.  The filter builds histograms of these metrics as a function of distance from a **Feature** boundary and writes them to a user specified output file.  

Note: this filter was developed for analyzing microtension specimens that had been deformed and then serial-sectioned at AFRL.

## Parameters ##

| Name | Type |
|------|------|
| Deformation Statistics File | Output File |
| VTK Output File | Output File |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Cell | GrainReferenceMisorientations |  |  | Find Feature Reference Misorientations (Statistics) |
| Cell | KernelAverageMisorientations |  |  | Find Kernel Average Misorientations (Statistics) |
| Cell | GBEuclideanDistances |  |  | Find Euclidean Distance Map (Statistics) |
| Cell | TJEuclideanDistances |  |  | Find Euclidean Distance Map (Statistics) |
| Cell | QPEuclideanDistances |  |  | Find Euclidean Distance Map (Statistics) |
| Cell | NearestNeighbors |  |  | Find Euclidean Distance Map (Statistics) |
| Feature | AvgQuats | Five (5) values (floats) defining the average orientation of the **Feature** in quaternion representation | Filter will calculate average quaternions for **Features** if not already calculated. | Find Feature Average Orientations (Statistics) |
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**| | Find Feature Phases (Generic), Read Feature Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Feature | GrainAvgMisorientations |  |  | Find Feature Reference Misorientations (Statistics) |
| Feature | Poles |  |  | Find Schmid Factors (Statistics) |
| Feature | Schmids |  |  | Find Schmid Factors (Statistics) |
| Feature | F1 |  |  | Find Slip Neighbor Transmission Metrics (Statistics) |
| Feature | F1spt |  |  | Find Slip Neighbor Transmission Metrics (Statistics) |
| Feature | F7 |  |  | Find Slip Neighbor Transmission Metrics (Statistics) |
| Feature | mPrime |  |  | Find Slip Neighbor Transmission Metrics (Statistics) |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |

## Created Arrays ##
None

## Authors ##


**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


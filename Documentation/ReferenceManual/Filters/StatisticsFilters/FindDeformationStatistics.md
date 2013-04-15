Find Deformation Statistics {#finddeformationstatistics}
======

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This Filter looks for correlations between various local deformation metrics (i.e. Kernel Average Misorientation (KAM) of Grain Reference Misorientation (GRM)) and distance from a **Field** boundary.  The filter builds histograms of these metrics as a function of distance from a **Field** boundary and writes them to a user specified output file.  

Note: this filter was developed for analyzing microtension specimens that had been deformed and then serial-sectioned at AFRL.

## Parameters ##

| Name | Type |
|------|------|
| Deformation Statistics File | Output File |
| VTK Output File | Output File |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | GrainIds |  |
| Cell | GrainMisorientations |  |
| Cell | KernelAverageMisorientations |  |
| Cell | MisorientationGradients |  |
| Cell | NearestNeighborDistances |  |
| Cell | NearestNeighbors |  |
| Field | AvgQuats |  |
| Field | FieldPhases |  |
| Field | GrainAvgMisorientations |  |
| Field | Poles |  |
| Field | Schmids |  |
| Field | SlipSystems |  |

## Created Arrays ##
None

## Authors ##


**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




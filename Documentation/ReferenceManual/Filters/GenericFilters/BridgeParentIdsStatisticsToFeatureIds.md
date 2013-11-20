Bridge ParentIds Statistics To GrainIds {#bridgeparentidsstatisticstofeatureids}
======

## Group (Subgroup) ##
Generic Filters (Memory Management)

## Description ##
This Filter makes interior calls to Rename Cell Array and Link Feature Data To Cell Array.  It is meant as a temporary solution to the need for two data containers at the same time.


## Parameters ##

| Name | Type |
|------|------|
| C-Axis Alignment Tolerance | Double |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**| | Find Feature Phases (Generic), Read Feature Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Feature | AvgQuats | Five (5) values (floats) defining the average orientation of the **Feature** in quaternion representation | Filter will calculate average quaternions for **Features** if not already calculated. | Find Feature Average Orientations (Statistics) |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | Active | Boolean value specifying if the **Feature** is still in the sample (1 if the **Feature** is in the sample and 0 if it is not). | At the end of the filter, all **Features** will be "Active" as the "Inactive" **Features** will have been removed.  |
| Feature | Avg | Float of the microtexture region "grain"-kernel average c-axis misalignment. | Only calculated if "Use Non-Contiguous Neighbors" is UNCHECKED. |
| Cell | MTRdensity | Float of the voxel fraction of "flipped" microtexture against the region searched in Find Neighborhoods filter. | To convert to a **Feature**, use Create Feature Array From Cell Array Filter. |
| Feature | ParentIds | List of grouped microtexture region **Features**. |  |
| Cell | ParentIds | List of grouped microtexture region **Cells**.  |  |
| Cell | OldGrainIds | List of original grain IDs mapped over to the new parent IDs which are now called grain IDs. | |

## Authors ##

**Copyright:** 2013 Joseph C. Tucker (UES), 2012 Michael A. Groeber (AFRL), 2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


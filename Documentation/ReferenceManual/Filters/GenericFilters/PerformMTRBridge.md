Perform MTR Bridge {#performmtrbridge}
======

## Group (Subgroup) ##
Generic Filters (Memory Management)

## Description ##
This Filter makes interior calls to Rename Cell Array and Link Field Data To Cell Array.  It is meant as a temporary solution to the need for two data containers at the same time.


## Parameters ##

| Name | Type |
|------|------|
| C-Axis Alignment Tolerance | Double |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Fields (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Field | FieldPhases | Phase Id (int) specifying the phase of the **Field**| | Find Field Phases (Generic), Read Field Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Field | AvgQuats | Five (5) values (floats) defining the average orientation of the **Field** in quaternion representation | Filter will calculate average quaternions for **Fields** if not already calculated. | Find Field Average Orientations (Statistics) |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | Active | Boolean value specifying if the **Field** is still in the sample (1 if the **Field** is in the sample and 0 if it is not). | At the end of the filter, all **Fields** will be "Active" as the "Inactive" **Fields** will have been removed.  |  
| Field | MTRgKAM | Float of the microtexture region "grain"-kernel average c-axis misalignment. | Only calculated if "Use Non-Contiguous Neighbors" is UNCHECKED. |  
| Cell | MTRdensity | Float of the voxel fraction of "flipped" microtexture against the region searched in Find Neighborhoods filter. | To convert to a **Field**, use Create Field Array From Cell Array Filter. |  
| Field | ParentIds | List of grouped microtexture region **Fields**. |  |
| Cell | ParentIds | List of grouped microtexture region **Cells**.  |  |  

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


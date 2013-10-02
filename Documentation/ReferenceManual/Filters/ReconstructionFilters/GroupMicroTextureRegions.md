Identify MicroTexture (C-Axis Misorientation) {#groupmicrotextureregions}
======

## Group (Subgroup) ##
Reconstruction Filters (Grouping)

## Description ##
This Filter groups neighboring **Fields** that have c-axes aligned within a user defined tolerance.  The algorithm for grouping the **Fields** is analogous to the algorithm for segmenting the **Fields** - only the average orientation of the **Fields** are used instead of the orientations of the individual **Cells** and the criterion for grouping only considers the alignment of the c-axes.  The user can specify a tolerance for how closely aligned the c-axes must be for neighbor **Fields** to be grouped.


NOTE: This filter is intended for use with *Heaxgonal* materials.  While the c-axis is actually just referring to the <001> direction and thus will operate on any symmetry, the utility of grouping by <001> alignment is likely only important/useful in materials with anisotropy in that direction (like materials with *Hexagonal* symmetry). 


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


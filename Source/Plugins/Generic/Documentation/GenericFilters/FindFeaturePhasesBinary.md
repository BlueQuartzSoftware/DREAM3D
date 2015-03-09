Find Feature Phases Binary {#findfeaturephasesbinary}
==========

## Group (Subgroup) ##

Generic Filters (Misc)

## Description ##

This filter assigns a phase number to binary data. The good voxels will be phase 1, and remaining voxels will be phase 0. It is generally for use when the cell phases weren't known ahead of time. For example, if an image is segmented into precipitates and non-precipitates, this filter will assign the precipitates to phase 1, and the non-precipitates to phase 0.

Additionally, this filter creates a Cell Ensemble Attribute Matrix to hold ensemble data. 

## Parameters ##

None


## Required Geometry ##

Image / Rectilinear Grid

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GoodVoxels | Good Voxels (bool) specifying if the voxel is to be counted as a phase or not | Values should be present from thresholding data and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | |
| Cell | FeatureIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |


## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature** | Values will begin at 1 as there is no phase 0, which is used temporarily in some filters for bad data|

## Created Attribute Matrix ##

| Default Name | Description | Comment |
|--------------|-------------|---------|
| CellEnsembleData | This is created to hold any ensemble data generated in future filters based on phase data. | |

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


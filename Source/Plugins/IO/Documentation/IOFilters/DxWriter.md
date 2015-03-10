Write Dx File (Feature Ids) {#dxwriter}
======

## Group (Subgroup) ##
I/O Filters (Output)


## Description ##

This Filter Writes to a Dx file. The file format is that used by the _OpenDX Source Software_ based on IBM's _Visualization Data Explorer_. 
For an example format, see the DXReader filter descriptions.

_See: www.opendx.org for more information._


## Parameters ##

| Name | Type | Comment |
|------|------|------|
| Output File | Output File Path | |
| Add Surface Layer | Boolean (On or Off) | This adds and "extra layer" of **Cells** around the outside of the structure and assigns them a *GrainId* of *-3*. |


## Required DataContainers ##

Image Geometry Required

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |

## Created Arrays ##
None


## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


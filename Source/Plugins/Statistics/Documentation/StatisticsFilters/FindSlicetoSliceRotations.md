Find Slice to Slice Crystallographic Misalignments {#findslicetoslicerotations}
======

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This filter is designed to quantify the amount of "misalignment" between sections in a serial-sectioning EBSD dataset.  The filter assumes that the **Features** in the volume are roughly equiaxed and randomly textured.  The filter calculates the *misorientation* between each **Cell** in a given section and the **Cell** directly above it in the next section.  The *misorientation* axis for each **Cell** pair is represented in the *sample reference frame* and averaged for all **Cells**.  If the *misorientations* are random (which would be expected for random, equiaxed **Features**), then the average misorientation axis would be <001>.  However, if the *misorientation* axis is some other direction, then it is likely that the *misorientations* are not random and may be due to the sections being *tilted* relative to each other, causing the orientations measured by the collection software to differ between sections.

Note: this filter should not be treated as an absolute measure of slice-slice misalignment, but rather a qualitative assessment of possible misalignment.

## Parameters ##

| Name | Type |
|------|------|
| Slice to Slice Rotations File | Output File |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Cell | GoodVoxels | Boolean values used to define "regions" to be aligned | Values are not required to be based on "good" or "bad" data, rather must only correspond to some identified "regions"  | Single Threshold (Cell Data) (Processing), Multi Threshold (Cell Data) (Processing) |
| Cell | Quats | Five (5) values (floats) that specify the orientation of the **Cell** in quaternion representation | Filter will calculate the quaternion for each **Cell** if it is not already calculated. | Find Cell Quaternions (Generic) |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |

## Created Arrays ##
None

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users



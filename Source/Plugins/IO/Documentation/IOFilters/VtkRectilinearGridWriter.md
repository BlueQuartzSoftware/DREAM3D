Write Vtk File (Rectilinear Grid) {#vtkrectilineargridwriter}
======

## Group (Subgroup) ##
I/O Filters


## Description ##
This Filter reads the **Feature** and phase ids together with image parameters required by Vtk to an output file named by the user. The file is used to generate the image of the **Features** and phases of the **Features**.


## Parameters ##

| Name | Type |
|------|------|
| Output File | Output File |
| Write **Feature** Ids | Boolean (On or Off) |
| Write Phase Ids | Boolean (On or Off) |
| Write Band Contrasts | Boolean (On or Off) |
| Write KAM Value | Boolean (On or Off) |
| Write GAM Values | Boolean (On or Off) |
| Write LMG Values | Boolean (On or Off) |
| Write Good **Cells** | Boolean (On or Off) |
| Write IPF Colors | Boolean (On or Off) |
| Write Binary File | Boolean (On or Off) |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding)

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



Find Feature Surface Voxel Fractions {#findsurfacevoxelfractions}
======

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This filter calculates the fraction of **Cells** of each **Features** that are on the "surface" of that **Feature**.  The filter simply iterates through all **Cells** asking for the **Feature** that owns them and if it is a "surface" **Cell**.  Each **Feature** counts the total number of **Cells** it owns and the number of those **Cells** that are "surface" **Cells**.  The fraction is then stored for each **Feature**.

## Parameters ##

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Cell | SurfaceVoxels | Value (int) equal to the number of neighboring **Cells** of a given **Cell** that belong to a different **Feature** than itself. Values will range from *0* to *6* |  | Find Surface Cells (Generic) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | SurfaceVoxelFractions | Fraction (float) of a **Cells** belonging to the **Feature** that are "surface" **Cells. |  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


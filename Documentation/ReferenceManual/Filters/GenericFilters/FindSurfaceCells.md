Find Boundary Cells {#findsurfacecells}
=============
## Group (Subgroup) ##
Generic Filters (Spatial)

## Description ##
This Filter determines, for each **Cell**, the number of neighboring **Cells** that are owned by a different **Field**.  The algorithm for determining this is as follows: 

1. Identify the **Field** to which a **Cell** belongs
2. Identify the **Fields** to which each of the current **Cell**'s six (6) face-face neighboring **Cells** (front, back, left, right, up, down) belong
3. Determine the number of those neighboring **Cells** belonging to a different **Field** than the current **Cell**. 
4. Repeat 1-3 for all **Cells**

## Parameters ##
None


## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Fields (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | SurfaceVoxels | Value (int) equal to the number of neighboring **Cells** of a given **Cell** that belong to a different **Field** than itself. Values will range from *0* to *6* |  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




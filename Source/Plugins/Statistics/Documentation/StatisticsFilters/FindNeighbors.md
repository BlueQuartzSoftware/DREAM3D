Find Feature Neighbors {#findneighbors}
==========

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This Filter determines, for each **Feature**, the number of other **Features** that are in contact with it.  The algorithm for determining the number of "contiguous" neighbors of each **Feature** is as follows:

1) Identify the **Feature** to which a **Cell** belongs
2) Identify the **Features** to which each of the current **Cell**'s six (6) face-face neighboring **Cells** (front, back, left, right, up, down) belong
3) If a neighboring **Cell** belongs to a different **Feature** than the current **Cell**, then that **Feature** (owner of the neighboring **Cell**) is added to the list of contiguous neighbors of the **Feature** that owns the current **Cell**.
4) Repeat 1-3 for all **Cells**

While performing the above steps, the number of neighboring **Cell**(s)/datapoint(s) with different **Feature** owner(s) than a given **Cell** is stored, which identifies whether a **Cell** lies on the surface of a **Feature** (i.e. the **Feature** boundary).
Additionally, the surface area shared between each set of contiguous **Features** is calculated by tracking the number of times two neighboring **Cells** correspond to a contiguous **Feature** pair.
The filter also notes which **Features** touch the outer surface of the sample (this is obtianed for "free" while performing the above algorithm).

## Parameters ##
None

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**| | Find Feature Phases (Generic), Read Feature Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | Surface**Cells** | Value (int) equal to the number of neighboring **Cells** of a given **Cell** that belong to a different **Feature** than itself. Values will range from *0* to *6* |  |
| Feature | NumNeighbors | Value (int) equal to the number of contiguous neighboring **Features** for a given **Feature** |  |
| Feature | NeighborLists | List of the contiguous neighboring **Features** for a given **Feature** |  |
| Feature | SharedSurfaceAreaLists | List of the area shared between contiguous neighboring **Features** for a given **Feature** |  |
| Feature | SurfaceFeatures | Boolean flag equal to 1 if the **Feature** touches an outer surface of the sample and equal to 0 if it does not. |  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


Find Field Neighbors {#findneighbors}
==========

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This Filter determines, for each **Field**, the number of other **Fields** that are in contact with it.  The algorithm for determining the number of "contiguous" neighbors of each **Field** is as follows:

1) Identify the **Field** to which a **Cell** belongs
2) Identify the **Fields** to which each of the current **Cell**'s six (6) face-face neighboring **Cells** (front, back, left, right, up, down) belong
3) If a neighboring **Cell** belongs to a different **Field** than the current **Cell**, then that **Field** (owner of the neighboring **Cell**) is added to the list of contiguous neighbors of the **Field** that owns the current **Cell**.
4) Repeat 1-3 for all **Cells**

While performing the above steps, the number of neighboring **Cell**(s)/datapoint(s) with different **Field** owner(s) than a given **Cell** is stored, which identifies whether a **Cell** lies on the surface of a **Field** (i.e. the **Field** boundary).
Additionally, the surface area shared between each set of contiguous **Fields** is calculated by tracking the number of times two neighboring **Cells** correspond to a contiguous **Field** pair.
The filter also notes which **Fields** touch the outer surface of the sample (this is obtianed for "free" while performing the above algorithm).

## Parameters ##
None

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Fields (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Field | FieldPhases | Phase Id (int) specifying the phase of the **Field**| | Find Field Phases (Generic), Read Field Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | Surface**Cells** | Value (int) equal to the number of neighboring **Cells** of a given **Cell** that belong to a different **Field** than itself. Values will range from *0* to *6* |  |
| Field | NumNeighbors | Value (int) equal to the number of contiguous neighboring **Fields** for a given **Field** |  |
| Field | NeighborLists | List of the contiguous neighboring **Fields** for a given **Field** |  |
| Field | SharedSurfaceAreaLists | List of the area shared between contiguous neighboring **Fields** for a given **Field** |  |
| Field | SurfaceFields | Boolean flag equal to 1 if the **Field** touches an outer surface of the sample and equal to 0 if it does not. |  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


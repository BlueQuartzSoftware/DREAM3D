Identify MicroTexture (C-Axis Misalignment) {#identifymicrotextureregions}
======

## Group (Subgroup) ##
Reconstruction Filters (Grouping)

## Description ##
This Filter groups neighboring **Features** that have c-axes aligned within a user defined tolerance.  The algorithm for grouping the **Features** is analogous to the algorithm for segmenting the **Features** - only the average orientation of the **Features** are used instead of the orientations of the individual **Cells** and the criterion for grouping only considers the misalignment of the c-axes.  The user can specify a tolerance for how closely aligned the c-axes must be for neighbor **Features** to be grouped.

A user defined patch size is rastered over the domain.  When a given patch contains a volume fraction of **Features** with a user defined c-axis misalignment above a user defined volume fraction, then that patch is flagged as an microtexture region and the growth algorithm commences.  For the growth algorithm, regions within the average diameter of the **Features** are searched and compared with **Features** for c-axis misalignments within the user defined tolerance.  If the **Feature** c-axis is aligned within the tolerance, it is added to the microtexture region.  This search and growth algorithm continues until none of the surrounding **Features** satisfies the criteria, at which point the next patch is executed along the raster.

NOTE: This filter is intended for use with *Hexagonal* materials.  While the c-axis is actually just referring to the <001> direction and thus will operate on any symmetry, the utility of grouping by <001> alignment is likely only important/useful in materials with anisotropy in that direction (like materials with *Hexagonal* symmetry).


## Parameters ##

| Name | Type |
|------|------|
| C-Axis Misalignment Tolerance | Float |
| Minimum MicroTextured Region Size (Diameter) | Float |
| Minimum Volume Fraction In MTR | Float |

## Required DataContainers ##
Volume

## Required Objects ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| -- | C-Axis Locations | | | |
| Cell | Cell Phases | | | |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |

## Created Objects ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | Active | Boolean value specifying if the **Feature** is still in the sample (1 if the **Feature** is in the sample and 0 if it is not). | At the end of the filter, all **Features** will be "Active" as the "Inactive" **Features** will have been removed.  |
| Feature | MTR Ids | | |
| Cell | New Cell Feature Attribute Matrix Name | | |


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)



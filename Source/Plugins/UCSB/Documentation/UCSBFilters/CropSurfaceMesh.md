Crop Surface Mesh {#cropsurfacemesh}
======

## Group (Subgroup) ##
Sampling Filters (Cropping/Cutting)

## Description ##
This Filter allows the user to crop a surface mesh of interest.  The input parameters are in units of **Cells**.  For example, if a volume was 100x100x100 **Cells** and each **Cell** was 0.25x0.25x0.25 microns, then if the user wanted to crop the last 5 microns in the X-direction, then the user would enter the following:

Xmin = 80,
Xmax = 99,
Ymin = 0,
Ymax = 99,
Zmin = 0,
Zmax = 99

Note: the input parameters are inclusive and begin at *0*, so in the above example *0-99* covers the entire range of **Cells** in a given dimension.

Face and Vertex data will be copied.

Normally this filter will leave the origin of the volume set at (0, 0, 0) which means output files like the XDMF file will have the same (0,0,0) origin. When viewing both the original larger volume and the new cropped volume simultaneously the cropped volume and the original volume will have the same origin which makes the cropped volume look like it was shifted in space. In order to keep the cropped volume at the same absolute position in space the user should turn **ON** the _Update Origin_ check box.

## Parameters ##

| Name | Type |
|------|------|
| X Min | Integer |
| Y Min | Integer |
| Z Min | Integer |
| X Max | Integer |
| Y Max | Integer |
| Z Max | Integer |
| Update Origin | Boolean |

## Required DataContainers ##
Voxel, SurfaceMesh

## Required Arrays ##
None

## Created Arrays ##
None

## Authors ## Will Lenthe

**Contact Info:** willlenthe@gmail.com


See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


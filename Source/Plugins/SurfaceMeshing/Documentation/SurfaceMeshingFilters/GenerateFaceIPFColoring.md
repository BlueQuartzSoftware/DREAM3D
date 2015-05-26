Generate Face IPF Coloring {#generatefaceipfcoloring}
======================

## Group (Subgroup) ##
SurfaceMesh

## Description ##

This filter generates a per triangle pair of colors based on the Inverse Pole Figure (IPF) color triangle for the crystal structure of the grain. Each face has 2 colors because each face is the boundary between 2 Grains or **Features**. The reference direction used for the IPF color generation is the Normal of the face.

_The user will need to calculate the Face Normals before this filter is run._

After running this filter visualizing the grains should result in something like the following image.

------------

![Example Output Data Generated with this filter.](GenerateFaceIPFColoring.png)
@image latex GenerateFaceIPFColoring.png " " width=6in

------------

## Parameters ##

## Required DataContainers ##
SurfaceMesh - Valid Surface Mesh containing the shared vertex array and face list


## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Face   | SurfaceMeshFaceLabels | N x 2 Col of signed integer |  | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |


## Created Arrays ##
| Type | Name | Comment |
|------|------|---------|
| Triangle Attribute Array | SurfaceMeshTriangleIPFColors | N X 6 Array of Unsigned Chars that are the RGB colors for each **Feature** Id Associated with the triangle. |



## Authors ##

**Copyright:** 2013 Michael A. Groeber (AFRL), 2013 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users



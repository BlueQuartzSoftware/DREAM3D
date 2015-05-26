Generate Face Misorientation Coloring {#generatefacemisorientationcoloring}
======================

## Group (Subgroup) ##
SurfaceMesh (Misc)

## Description ##

This filter generates a 3 component vector per **Face** that is the axis-angle of the misorientation associated with the **Features** that lie on either side of the **Face**.  The axis is normalized, so if the magnitude of the vector is viewed, it will be the *misorientation angle* in degrees.


## Parameters ##

## Required DataContainers ##

SurfaceMesh - Valid Surface Mesh containing the shared vertex array and face list


## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-------|
| Face | SurfaceMeshFaceLabels | N x 2 Col of signed integer |  | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |


## Created Arrays ##
| Type | Name | Comment |
|------|------|---------|
| Face | SurfaceMeshTriangleIPFColors | N X 3 Array of floats that is the axis-angle of the misorientation across each **Face** |


## Authors ##

**Copyright:** 2013 Michael A. Groeber (AFRL), 2013 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users



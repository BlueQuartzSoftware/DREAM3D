Rotate Sample Reference Frame {#rotatesamplerefframe}
======

## Group (Subgroup) ##
Sampling Filters (Rotating/Transforming)

## Description ##
This filter will rotate the *spatial reference frame* around a user defined axis, by a user defined angle.  The filter will modify the (x,y,z) positions of each **Cell** to correctly represent where the **Cell** sits in the newly defined reference frame.  

Example: 
If a user selected a *Rotation Angle* of *90 degrees* and a *Rotation Axis* of *(001)*, then a **Cell** sitting at (10,0,0) would be transformed to (0,-10,0), since the new *reference frame* would have x'=y and y'=-x.   

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Rotation Axis | Float (x3) | Vector in sample reference frame to rotate about |
| Rotation Angle | Float | Magnitude of rotation (in degrees) about the rotation axis |

## Required Geometry ##
Image/Rectilinear Grid

## Required Arrays ##
None

## Created Arrays ##
None

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


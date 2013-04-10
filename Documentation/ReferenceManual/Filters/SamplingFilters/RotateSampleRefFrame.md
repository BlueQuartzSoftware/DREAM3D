Rotate Sample Reference Frame {#rotatesamplerefframe}
======

## Group (Subgroup) ##
Sampling Filters (Rotating/Transforming)

## Description ##
This filter will rotate the *spatial reference frame* around a user defined axis, by a user defined angle.  The filter will modify the (x,y,z) positions of each **Cell** to correctly represent where the **Cell** sits in the newly defined refernece frame.  

Example: 
If a user selected a *Rotation Angle* of *90 degrees* and a *Rotation Axis* od *(001)*, then a **Cell** sitting at (10,0,0) would be transformed to (0,-10,0), since the new *reference frame* would have x'=y and y'=-x.   

## Parameters ##

| Name | Type |
|------|------|
| Rotation Axis | Vector (3) of Floats |
| Rotation Angle | Float |

## Required DataContainers ##
Voxel

## Required Arrays ##
None

## Created Arrays ##
None

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




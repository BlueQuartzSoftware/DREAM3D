Update Cell Quaternions {#updatecellquats}
=======

## Group (Subgroup) ##
Generic Filters (Crystallography)

## Description ##

This filter updates the *quaternions* for each **Cell** to have 4 components rather than the 5 components used in earlier versions of DREAM.3D.   
 


## Parameters ##
None

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | Quats | Five (5) values (floats) that specify the orientation of the **Cell** in quaternion representation | The first value is a dummy value, so each **Cell** has quat = {dummy, q1, q2, q3, q4} - where q1, q2, and q3 contain the axis information and q4 contains the angle information of the quaternion | Find Cell Quaternions (Generic) |


## Created Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| Cell | Quats | Four (4) values (floats) that specify the orientation of the **Cell** in quaternion representation | The first value is a dummy value, so each **Cell** has quat = {qx, qy, qz, qw} - where qx, qy, and qz contain the axis information and qw contains the angle information of the quaternion |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




Add Orientation Noise {#addorientationnoise}
======

## Group (Subgroup) ##
Synthetic Builder Filters (Crystallography)

## Description ##
This filter adds "noise" to the **Cell** orienations.  This filter is intended to add "realism" to synthetic structures that have single orientations assigned to **Fields** (an in turn, all of the **Cells** they own).  The user can specify a magnitude of the "noise", which refers to the maximum rotation angle that is applied to the existing **Cells**.  The algorithm for applying the orientation "noise" is as follows:

1. Obtain the orientation of the **Cell**.
2. Generate a random number between *0* and the user defined magnitude. This value is the rotation angle.
3. Generate a random unit vector.  This is the rotation axis.
4. Calculate the rotation corresponding to the axis-angle pair generated in Steps 2-3 and apply it to the orientation of the **Cell** to obtain a new orientation.
5. Repeat for all **Cells**



## Parameters ##

| Name | Type |
|------|------|
| Magnitude of Orientation Noise | Double |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | CellEulerAngles | Three (3) angles (floats) defining the orientation of the **Cell** in Bunge convention (Z-X-Z) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Match Crystallography (SyntheticBuilding) |

## Created Arrays ##
None

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


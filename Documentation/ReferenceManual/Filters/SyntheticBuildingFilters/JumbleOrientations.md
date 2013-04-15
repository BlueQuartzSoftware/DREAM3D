Jumble Orientations {#jumbleorientations}
======

## Group (Subgroup) ##
Synthetic Builder Filters (Crystallography)

## Description ##
This filter "jumbles" the orientations of the **Fields** amongst each other.  The filter cycles through all the **Fields**, randomly choosing another **Field** to swap orientations with.  On each swap, it is checked that the randomly selected **Field** is of the same phase.

Note: by cycling through the **Fields**, it is garaunteed that each **Field** will be swapped at least once, but it could actually be swapped multiple times if a later **Field** randomly selects it to swap with.


## Parameters ##
None

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | GrainIds |  |
| Field | FieldPhases |  |

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | CellEulerAngles |  |
| Field | AvgQuats |  |
| Field | FieldEulerAngles |  |

##Authors##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.


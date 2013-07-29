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

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Fields (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Field | FieldPhases | Phase Id (int) specifying the phase of the **Field**| | Find Field Phases (Generic), Read Field Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |

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


See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


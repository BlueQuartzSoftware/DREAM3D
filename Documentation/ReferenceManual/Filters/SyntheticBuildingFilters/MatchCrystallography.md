Match Crystallography {#matchcrystallography}
======

## Group (Subgroup) ##
Synthetic Builder Filters (Crystallography)

## Description ##
This Filter searches for the crystal structures present and finds where matches occur in the synthetic volume. The _Initialize Synthetic Volume_ Filter must be run before this Filter will function properly, because that Filter generates the Statistics needed by this Filter. *GrainIds* are also needed and must have been generated in other Synthetic Builder Filters.

## Parameters ##

## Required Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | GrainIds |  |
| Field | FieldPhases |  |
| Field | SurfaceFields |  |
| Ensemble | CrystalStructures |  |
| Ensemble | NumFields |  |
| Ensemble | TotalSurfaceAreas |  |

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | CellEulerAngles |  |
| Field | AvgQuats |  |
| Field | FieldEulerAngles |  |
| Field | Volumes |  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.




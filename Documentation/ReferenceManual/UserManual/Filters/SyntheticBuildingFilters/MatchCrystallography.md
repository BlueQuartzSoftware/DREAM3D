Match Crystallography {#matchcrystallography}
======

## Group (Subgroup) ##
Synthetic Builder Filters (Crystallography)

## Description ##
This Filter iteratively either swaps out the orientation of a **Field** (selected at random) for a new orientation (sampled from the goal Orientation Distribution Function) or switches the orientations of two **Fields** (selected at random).  The switch or swap is accepted if it lowers the error of the current ODF and Misorientation Distribution Function from the goal.  This is done for a user defined number of iterations.  

## Parameters ##
| Name | Type |
|------|------|
| Maximum Number of Iterations (Swaps) | Integer |

## Required DataContainers ##
Voxel

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

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




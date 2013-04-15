Pack Primary Phases {#packprimaryphases}
======

## Group (Subgroup) ##
Synthetic Builder Filters (Packing)

## Description ##
If there are primary phases in the Statistics file, then this filter will place primary **Fields** with the sizes, shapes, physical orientations and locations corresponding to the goal statistics.  The user can specify if they want *periodic boundary conditions* and whether they would like to write out the goal attributes of the generated **Fields**. 


## Parameters ##

| Name | Type |
|------|------|
| Periodic Boundary | Boolean (On or Off) |
| Write Goal Attributes | Boolean (On or Off) |
| Goal Attributes CSV File | Output File |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | CellPhases |  |
| Cell | GrainIds |  |
| Ensemble | PhaseTypes |  |
| Ensemble | ShapeTypes |  |

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Field | Active |  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




Pack Primary Phases {#packprimaryphases}
======

## Group (Subgroup) ##
Synthetic Builder Filters (Packing)

## Description ##
This Filter generates **Fields** and then adds them to a volume at random positions. The **Fields** are then moved within the volume such that the volume is maximally filled by the **Fields** and still satisfies the neighborhood input criterion.
Because there are two criteria (space filling and neighborhood error weight) to be satisfied, this Filter uses the _space filling_ criterion as the primary control on the filling actions. Knowledge of neighborhoods is often not detailed enough to use.
 The __Neighborhood Error Weight__ is a number in the range [0, 1]. Hence,a small __Neighborhood Error Weight__ favors the space filling criterion. A large number, e.g., 1.0, means space filling and neighborhoods are equally important.
The user needs to keep in mind that large values of __Neighborhood Error Weight__ can result in changes in the **Fields** as the Filter works to accomplish the space filling goal. The **Fields** will overlap each other and be changed to accommodate that overlap, which means the user does not have the **Fields** that were placed in the volume initially.


## Parameters ##

| Name | Type |
|------|------|
| Neighborhood Error Weight | Double |
| Periodic Boundary | Boolean (On or Off) |

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
| Field | AxisEulerAngles |  |
| Field | AxisLengths |  |
| Field | Centroids |  |
| Field | EquivalentDiameters |  |
| Field | FieldPhases |  |
| Field | Neighborhoods |  |
| Field | Omega3s |  |
| Field | Volumes |  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




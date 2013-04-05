Merge Twins {#mergetwins}
======

## Group (Subgroup) ##
Reconstruction Filters (Grouping)

## Description ##
The Merge Twins Filter is essentially identical to the Merge Colonies Filter, except the special misorientations used to determine if grains are grouped together are those misorientations that correspond to twins in an fcc (only fcc) crystal, specifically the Σ 3,5,7,9,11 twins.


## Parameters ##

| Name | Type |
|------|------|
| Axis Tolerance | Double |
| Angle Tolerance | Double |

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Field | AvgQuats | Five (5) values (floats) defining the average orientation of the **Field** in quaternion representation | Filter will calculate average quaternions for **Fields** if not already calculated. |
| Field | FieldPhases | Phase Id (int) specifying the phase of the **Field** | Filter will determine **Field** phases if not previously determined |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | Active | Boolean value specifying if the **Field** is still in the sample (1 if the **Field** is in the sample and 0 if it is not) | At the end of the filter, all **Fields** will be "Active" as the "Inactive" **Fields** will have been removed.  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.




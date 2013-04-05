Segment **Fields** (Misorientation) {#ebsdsegmentgrains}
======

## Group (Subgroup) ##
Reconstruction Filters (Segmentation)

## Description ##
This Filter segments the **Fields** by grouping neighboring **Cells** that satisfy the _misorientation tolerance_, i.e., have _misorientation angle_ less than the value set by the user. The __Cell__  _GrainIds_ established by this filter are used by the other Filters. 


## Parameters ##

| Name | Type | Comment |
|------|------|------|
| Misorientation Tolerance | Double | Value is in degress |

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Cell | Good**Cells** | Boolean values used to define "regions" to be aligned | Values are not required to be based on "good" or "bad" data, rather must only correspond to some identified "regions"  |
| Cell | Quats | Five (5) values (floats) that specify the orientation of the **Cell** in quaternion representation | Filter will calculate the quaternion for each **Cell** if it is not already calculated. |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. |  |
| Field | Active | Boolean value specifying if the **Field** is still in the sample (1 if the **Field** is in the sample and 0 if it is not) | At the end of the filter, all **Fields** will be "Active" as the "Inactive" **Fields** will have been removed.  |
| Field | FieldPhases | Phase Id (int) specifying the phase of the **Field** |  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.




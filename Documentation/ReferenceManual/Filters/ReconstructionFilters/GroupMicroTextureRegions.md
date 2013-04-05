Identify MicroTexture (C-Axis Misorientation) {#groupmicrotextureregions}
======

## Group (Subgroup) ##
Reconstruction Filters (Grouping)

## Description ##
This Filter searches for neighboring **Fields** that have c-axes commonly aligned and merges them. The user needs to first segment the **Fields** (_EbsdSegmentGrains_ or _CAxisSegmentGrains_ Filters). Once segmented, the **Fields** are examined for c-axis orientation and neighboring **Fields** that have alignment within the misorientation tolerance entered by the user (1-10 degrees). After the **Fields** are grouped together, they are merged into a microtextured region encompassing these **Fields**.
This Filter is similar to the _TwinMisorientation_ and _ColonyMisorientation_ Filters in the sense that **Fields** are grouped according to some criterion.


## Parameters ##

| Name | Type |
|------|------|
| C-Axis Alignment Tolerance | Double |

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Field | FieldPhases | Phase Id (int) specifying the phase of the **Field** | Filter will determine **Field** phases if not previously determined |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Field | AvgQuats | Five (5) values (floats) defining the average orientation of the **Field** in quaternion representation | Filter will calculate average quaternions for **Fields** if not already calculated. |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | Active | Boolean value specifying if the **Field** is still in the sample (1 if the **Field** is in the sample and 0 if it is not) | At the end of the filter, all **Fields** will be "Active" as the "Inactive" **Fields** will have been removed.  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.




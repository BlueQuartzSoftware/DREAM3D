Minimum Size Filter (All Phases) {#minsize}
======

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This filter removes **Fields** that have a total number of **Cells** below the minimum threshold defined by the user. If the user chooses a
value larger than the largest **Field**, then all **Fields** will be eliminated and the filter will become stuck in an inifinite loop trying to 
fill the eliminated **Fields**. 
*The number of **Cells** in the largest **Field** is determined during the running of this filter. Hence, a choice of threshold should be carefully be choosen
if it is not known how many **Cells** are in the largest **Fields**.
After removing all the small **Fields**, the remaining **Fields** are isotropically coarsened to fill the gaps left by the small **Fields**.

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Minimum Allowed **Field** Size | Integer | Number of **Cells** that must be present in the **Field** for it to remain in the sample |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | Active | Boolean value specifying if the **Field** is still in the sample (1 if the **Field** is in the sample and 0 if it is not) | At the end of the filter, all **Fields** will be "Active" as the "Inactive" **Fields** will have been removed.  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




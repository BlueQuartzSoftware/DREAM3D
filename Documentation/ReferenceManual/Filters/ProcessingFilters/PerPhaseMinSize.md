Minimum Size Filter (Per Phase) {#perphaseminsize}
======

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This Filter removes **Fields** that have a total number of **Cells** below the _minimum number_ input by the user. The _minimum_ is 1. There is no _maximum_, but entering a number larger than the number of **Cells** in the largest **Field** will cause all **Fields** to be removed and THE PIPELINE WILL EXIT DURING THE FILTER!
Hence, a conservative approach to selecting the minimum number of **Cells** is strongly recommended. 

After removing all the small **Fields**, the remaining **Fields** are _coarsened_ to fill the gaps left by the small **Fields**.  The iteration _coarsens_ the **Fields** by one **Cell** each iteration. 

This filter has the additional ability to
only apply the filter to a single phase of the users choosing.


## Parameters ##

| Name | Type | Description |
|------|------| ------- |
| Minimum Allowed **Field** Size | Integer | Number of **Cells** |
| Phase Number | Integer | The phase number to apply this filter to. |

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute.|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute.|
| Field | FieldPhases | Phase Id (int) specifying the phase of the **Field**| |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | Active |      |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.




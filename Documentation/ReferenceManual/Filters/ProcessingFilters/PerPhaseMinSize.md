Minimum Size Filter (Per Phase) {#perphaseminsize}
======

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This filter is identical to the [Minimum Size Filter (All Phases)](MinSize.html ""), but with the additional ability to only apply the filter to a single phase of the users choosing.


## Parameters ##

| Name | Type | Description |
|------|------| ------- |
| Minimum Allowed **Field** Size | Integer | Number of **Cells** |
| Phase Number | Integer | The phase number to apply this filter to. |

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute.|
| Field | FieldPhases | Phase Id (int) specifying the phase of the **Field**| |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | Active | |  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.




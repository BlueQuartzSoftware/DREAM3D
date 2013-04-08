Find Field Neighbor Misorientations {#findmisorientations}
==========

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This Filter determines, for each **Field**, the misorientations with the **Fields** that are in contact with it.  

## Parameters ##

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Field | FieldPhases | Phase Id (int) specifying the phase of the **Field** | Filter will determine **Field** phases if not previously determined |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | Surface**Cells** | Value (int) equal to the number of neighboring **Cells** of a given **Cell** that belong to a different **Field** than itself. Values will range from *0* to *6* |  |
| Field | NumNeighbors | Value (int) equal to the number of contiguous neighboring **Fields** for a given **Field** |  |
| Field | SurfaceFields | Boolean flag equal to 1 if the **Field** touches an outer surface of the sample and equal to 0 if it does not. |  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.




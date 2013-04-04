Minimum Number of Neighbors Filter {#minneighbors}
======

## Group (Subgroup) ##
Processing Filters

## Description ##
This Filter sets the minimum number (an integer) of __Field__ __Neighbors__ a grain must have to remain in the structure. Entering 0 (zero) is valid because the grain may occupy the whole volume and would therefore have no __Neighbors__. Also, entering zero results in nothing changing in other grains.
Entering a number larger than the grain that has the maximum number of __Neighbors__ generates an _error_ (all grains have been removed!). The user needs to proceed conservatively here when choosing the value for the minimum to avoid accidentally exceeding the max and generating an error later in the pipeline.
Using minimum number entries between 1 and a value less than the _maxmimum_ number of __Neighbors__ associated with a grain results in removal of those grains that have the entered number of __Neighbors__ or fewer. After these grains are removed, the remaining grains are _coarsened_ iteratively, one _voxel_ per iteration, until the gaps left by the removed grains are filled.

## Parameters ##

| Name | Type |
|------|------|
| Minimum Number Neighbors | Integer |

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Field | FieldPhases | Phase Id (int) specifying the phase of the **Field** | Filter will determine **Field** phases if not previously determined |
| Field | NumNeighbors | Value (int) equal to the number of contiguous neighboring features/grains for a given **Field** | Filter will calculate the number of neighbors for each **Field** if not already calculated |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | Active | Boolean value specifying if the **Field** is still in the sample (1 if the **Field** is in the sample and 0 if it is not) | At the end of the filter, all features/grains will be "Active" as the "Inactive" features/grains will have been removed.  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.




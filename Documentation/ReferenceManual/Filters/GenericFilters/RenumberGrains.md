Renumber Fields {#renumbergrains}
========

## Group (Subgroup) ##
Generic Filters (Memory/Management)

## Description ##
This filter will renumber the GrainIds array to make it contiguous from *0* to *N-1*.  This is necessary if some prior filter has removed a **Field** for some reason and it is desired to have a continuous number of the **Fields**.  The filter cues off a boolean **Field** array called *Active* to know whether a given **Field** is in existance.  The **Field** array map is rescaled to the number of "active" **Fields** and the **Cell** array called *GrainIds* is updated to reflect the new numbers of the **Fields** after the "inactive" **Fields** are removed.
 
## Parameters ##
 None

## Required Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Field | Active | This is a boolean array that states *true* if the **Field** exists and states *false* if the **Field** does not exist | This array must be present for this filter to run.  The values cannot be determined by this filter.


## Created Arrays ##
None



## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.





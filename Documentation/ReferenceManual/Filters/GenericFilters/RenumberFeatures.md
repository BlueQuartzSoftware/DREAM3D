Renumber Features {#renumbergrains}
========

## Group (Subgroup) ##
Generic Filters (Memory/Management)

## Description ##
This filter will renumber the GrainIds array to make it contiguous from *0* to *N-1*.  This is necessary if some prior filter has removed a **Feature** for some reason and it is desired to have a continuous number of the **Features**.  The filter cues off a boolean **Feature** array called *Active* to know whether a given **Feature** is in existance.  The **Feature** array map is rescaled to the number of "active" **Features** and the **Cell** array called *GrainIds* is updated to reflect the new numbers of the **Features** after the "inactive" **Features** are removed.
 
## Parameters ##
 None

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) 
| Feature | Active | This is a boolean array that states *true* if the **Feature** exists and states *false* if the **Feature** does not exist | This array must be present for this filter to run.  The values cannot be determined by this filter. | Minimum Size Filter (Processing), Minimum Number of Neighbors Filter (Processing), Crop Volume (Sampling) |


## Created Arrays ##
None



## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.





See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


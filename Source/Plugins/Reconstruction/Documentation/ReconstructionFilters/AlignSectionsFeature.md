Align Sections (Feature) {#alignsectionsfeature}
======

## Group (Subgroup) ##
Reconstruction Filters (Alignment)

## Description ##
This filter attempts to align consecutive serial sections by determining the position that results in the most overlap
of previously defined "regions".  The "regions" are defined by a binary/boolean array where the **Cells** have been flagged
by another filter.  Typically, during reading of the data, each **Cell** is subject to a "Quality Metric" (or threshold) that 
defines if the **Cell** is "good".  This threshold can be used to define areas of each slice that are bad, either due to actual 
features in the microstructure or external references inserted by the user/experimentalist.  If these "regions" of "bad" **Cells** 
are believed to be consistent through sections, then this filter will preserve that by aligning those "regions" on top of one another on 
consecutive sections.
The algorithm of this filter is as follows:

1. Compare the value of the binary/boolean array for each **Cell** in a section with the value of the array for the **Cell**  directly above it in the next section.  
2. Count the number of **Cell** pairs that do not have the same value and store that as the misalignment value for that position.
3. Repeat steps 1 and 2 for each position when shifting the second slice (relative to the first) from three (3) **Cells** to the left 
to three (3) **Cells** to the right, as well as from three (3) **Cells** up to three (3) **Cells** down.
*Note that this creates a 7x7 grid*
4. Determine the position in the 7x7 grid that has the lowest misalignment value. (It will be the position with the fewest different **Cell** pairs).
5. Repeat steps 1-4 with the center of each (new) 7x7 grid at the best position from the last 7x7 grid until the best position in the current/new 7x7 grid is the same as the last 7x7 grid. *Note that this is similar to a downhill simplex and can get caught in a local minima*

  


## Parameters ##
None

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GoodVoxels | Boolean values used to define "regions" to be aligned | Values are not required to be based on "good" or "bad" data, rather must only correspond to some identified "regions"  | Single Threshold (Cell Data) (Processing), Multi Threshold (Cell Data) (Processing) |

## Created Arrays ##
None


## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


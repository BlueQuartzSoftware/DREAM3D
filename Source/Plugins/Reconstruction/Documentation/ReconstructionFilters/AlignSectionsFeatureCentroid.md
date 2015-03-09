Align Sections (Feature Centroid) {#alignsectionsfeaturecentroid}
======

## Group (Subgroup) ##
Reconstruction Filters (Alignment)

## Description ##
This filter attempts to align consecutive serial sections by determining the position that closest aligns the centroid(s) of previously
defined "regions".  The "regions" are defined by a binary/boolean array where the **Cells** have been flagged by another filter.
Typically, during reading of the data, each **Cell** is subject to a "Quality Metric" (or threshold) that defines if the **Cell**
is "good".  This threshold can be used to define areas of each slice that are bad, either due to actual features in the microstructure or
external references inserted by the user/experimentalist.  If these "regions" of "bad" **Cells** are believed to be consistent through
sections, then this filter will preserve that by aligning those "regions" on top of one another on consecutive sections.
The algorithm of this filter is as follows:

1. Determine the centroid of all **Cells** that are flagged with a boolean value of one (or true) for each section 
2. Determine the full **Cell** shifts that place centroids of consecutive sections as close as possible.
*Note that this is algorithm cannot get caught in a local minima*


## Parameters ##

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


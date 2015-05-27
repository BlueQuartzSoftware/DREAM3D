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



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users



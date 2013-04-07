

Segment Fields (Scalar) {#scalarsegmentgrains}
======

## Group (Subgroup) ##
Reconstruction Filters (Segmentation)

## Description ##
This filter groups together **Cells** that differ in some user defined scalar value by less than a user defined tolerance.  For example, if the user selected array was an 8bit image array, then the array would consist of integer values between *0* and *255*.  If the user then set a tolerance vlaue of 10, then the filter would identify all sets of contiguous **Cells** that have *grayscale* values with *10* of each other.

## Parameters ##

| Name | Type |
|------|------|
| Input Cell Array Name | Unknown Type |
| Scalar Tolerance | Double |

## Required Arrays ##
None



## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | GrainIds |  |
| Field | Active |  |

## Authors ##

Copyright [INSERT YOUR NAME HERE]
Contact Info:[INSERT EMAIL ADDRESS HERE]
Version: 1.0.0
License: See the License.txt file that came with DREAM3D.



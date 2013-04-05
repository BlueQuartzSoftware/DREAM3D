Write Ph File (**Field** Ids) {#phwriter}
======

## Group (Subgroup) ##
I/O Filters


## Description ##
This Filter writes to a data file in a format used by Carnegie Mellon University.  See the PhReader filter description for example data format. 

## Parameters ##

| Name | Type |
|------|------|
| Output File | Output File |

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |

## Created Arrays ##
None

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.




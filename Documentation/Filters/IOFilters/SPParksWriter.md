Write SPParks File (Grain Ids) {#spparkswriter}
======

## Group ##
I/O Filters


## Description ##
This Filter writes to a data file in a format used by <a href="http://spparks.sandia.gov/">SPPARKS Kinetic Monte Carlo Simulator.</a> The
 "Values" section is a pair of numbers where the first number is the site ID and the second
 number is the actual value of the Grain Id at that site. LINE 4 evidently must be what is shown in the example file.
 LINES 5,6 and 7 are the Dimensions of the volume. LINE 3 is the total number of voxels. 
 More information can be found at the <a href="http://spparks.sandia.gov/doc/read_sites.html">SPParks web site</a>

__Start Example Partial File__

_[LINE 1]_<tt>-</tt>
_[LINE 2]_<tt>3 dimension</tt>
_[LINE 3]_<tt>8000000 sites</tt>
_[LINE 4]_<tt>26 max neighbors</tt>
_[LINE 5]_<tt>0 200 xlo xhi</tt>
_[LINE 6]_<tt>0 200 ylo yhi</tt>
_[LINE 7]_<tt>0 200 zlo zhi</tt>
_[LINE 8]_
_[LINE 9]_<tt>Values</tt>
_[LINE 10]_
 1 944
 2 944
 3 944
 4 944
 5 509
 6 509
 7 509
.....


## Parameters ## 

| Name | Type |
|------|------|
| Output File | Output File |

## Required Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which feature/grain each voxel/datapoint belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |

## Created Arrays ##
None

## Authors ##

**Copyright**2012 Michael A. Jackson (BlueQuartz Software)
**Contact Info** dream3d@bluequartz.net
**Version** 1.0.0
**License**  See the License.txt file that came with DREAM3D.


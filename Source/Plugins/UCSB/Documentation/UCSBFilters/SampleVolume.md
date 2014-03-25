SampleVolume {#samplevolume}
=====

## Group (Subgroup) ##
UCSB (UCSB)


## Description ##
This filter makes a copy of the dataset, picks a region of the specified dimensions at random, writes it to the specified location, and loops. Only the voxel data is copied and written.

## Parameters ##
| Name             | Type |
|------------------|------|
| Number of Volumes | Int |
| Sampling Size | 3x Int |
| Output File Prefix | String |
| Write Xdmf | Bool |
| Output Directory | Path |

## Required Arrays ##

None (a valid voxel data container is required)


## Created Arrays ##

None



## Authors: ##

**Copyright:** 2014 Michael A. Jackson (BlueQuartz Software), 2014 William Lenthe (UCSB)

**Contact Info:** willlenthe@gmail.com

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



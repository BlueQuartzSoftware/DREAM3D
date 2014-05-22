FindFieldAverage {#findfieldaverage}
=====

## Group (Subgroup) ##
UCSB (UCSB)


## Description ##
For each grain calculates the average value of the selected array. Multicomponenent arrays are averaged on a per component basis. Average values are cast back to the original array type which may create artifacts for integer type arrays (especially boolean).


## Parameters ##
| Name             | Type |
|------------------|------|
| Voxel Array Name| String |
  

## Required Arrays ##
| Name| Type |
|------|------|
| Grain IDs| Int |    
| Selected Array | Any |       


## Created Arrays ##
| Name| Type |
|------|------| 
| Selected Array | Any |   

## Authors ##

**Copyright** 2014 Will Lenthe (UCSB)

**Contact Info** willlenthe@gmail.com

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

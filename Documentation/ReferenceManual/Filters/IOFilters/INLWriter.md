Write INL File {#inlwriter}
======
## Group (Subgroup) ##
IO Filters

## Description ##

This filter writes out a file in column format that is space delimited. Header lines are denoted by the "#" character. The columns are the following:

- phi1
- Phi
- phi2
- x Position (Microns)
- y Position (Microns)
- z Position (Microns)
- Grain Id
- Phase Id
- Symmetry
 
Some information about the phase is included in the header section of the file in addtion
 to values for the Origin, Step Size, Dimensions and number of grains in the file.

## Parameters ##

| Name | Type |
|------|------|
| Output File | String |

## Required Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | GrainIds |  |
| Cell | Euler Angles | Values in Radians |
| Cell | Phases |  |
| Ensemble | Crystal Structures |  |
| Ensemble | Material Name |  |

## Created Arrays ##
None



## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.




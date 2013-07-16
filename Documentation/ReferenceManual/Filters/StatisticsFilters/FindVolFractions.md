Find Volume Fractions of Phases {#findvolfractions}
======


## Group (Subgroup) ##

Statistics Filters (Morphological)



## Description ##
This filter determines the volume fraction of each phase and stores it in an **Ensemble** array.


## Parameters ##
None 

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell**| | ReadH5Ebsd (IO) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Ensemble | VolFractions | Fraction (float) of volume that belongs to each phases. |  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




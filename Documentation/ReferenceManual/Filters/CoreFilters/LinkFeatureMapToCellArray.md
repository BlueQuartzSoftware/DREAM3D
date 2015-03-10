Link Feature Map To Cell Array {#linkfeaturemaptocellarray}
==============================

## Group (Subgroup) ##
Core Filters (Memory/Management)

## Description ##
A **Cell** array is used as the index into the **Feature** map for each **Cell**.  This filter allows the user to specify which **Cell** array is that index.  The filter clears the current **Feature** map, creates a new **Feature** map and resizes it the largest value found in the user selected **Cell** array.  Further the filter creates the *Active* array in the new **Feature** map, which identifies whether each **Feature** *exists* by checking if there is *at least* one **Cell** that belongs to each **Feature**. 

## Parameters ##

| Name | Type |
|------|------|
| Cell Array Name | String |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | *User Chosen* |  | Array will always be present, since the user may only chose from existing arrays | N/A |


## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | Active | | |





## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


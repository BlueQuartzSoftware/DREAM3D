Link Field Map To Cell Array {#linkfieldmaptocellarray}
==============================

## Group (Subgroup) ##
Generic Filters (Memory/Management)

## Description ##
A **Cell** array is used as the index into the **Field** map for each **Cell**.  This filter allows the user to specify which **Cell** array is that index.  The filter clears the current **Field** map, creates a new **Field** map and resizes it the largest value found in the user selected **Cell** array.  Further the filter creates the *Active* array in the new **Field** map, which identifies whether each **Field** *exists* by checking if there is *at least* one **Cell** that belongs to each **Field**. 

## Parameters ##
| Name | Type |
|------|------|
| Cell Array Name | String |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data
|------|--------------|-------------|---------|-----|
| Cell | *User Chosen* |  | Array will always be present, since the user may only chose from existing arrays | N/A |


## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | Active | | |





## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL), 2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



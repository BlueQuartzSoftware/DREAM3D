Link Feature Map To Cell Array {#linkfeaturemaptoelementarray}
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





## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users



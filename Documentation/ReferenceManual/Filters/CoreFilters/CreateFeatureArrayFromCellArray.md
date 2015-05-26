Create Feature Array From Cell Array {#createfeaturearrayfromcellarray}
==============================

## Group (Subgroup) ##

Core Filters (Memory/Management)

## Description ##

This filter copies all the associated **Cells** to the **Feature** that they belong to.

## Parameters ##

| Name | Type |
|------|------|
| Cell Array Name | String |

## Required DataContainers ##

Voxel DataContainer

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | *User Chosen* |  | Array will always be present, since the user may only chose from existing arrays | N/A |


## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | *User Chosen* | | Array will have same name as its **Cell** counterpart |

## Authors ##

**Copyright:** 2013 Joseph C. Tucker (UES)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users



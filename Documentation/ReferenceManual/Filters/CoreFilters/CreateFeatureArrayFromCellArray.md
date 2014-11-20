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



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


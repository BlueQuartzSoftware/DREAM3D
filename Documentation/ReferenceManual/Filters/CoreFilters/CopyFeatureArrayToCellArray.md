Copy Feature Array To Cell Array {#copyfeaturearraytocellarray}
==============================

## Group (Subgroup) ##

Generic Filters (Memory/Management)

## Description ##

This filter copies the values associated with a **Feature** to all the **Cells** that belong to that **Feature**.  Visualization files write only the **Cell** attributes, so if the user wants to display a spatial map of a **Feature** level attribute, this filter will transfer that information down to the **Cell** level. One issue to consider is the memory required when expanding from the number of **Features** to the number of **Cells**.

## Parameters ##

| Name | Type |
|------|------|
| Feature Array Name | String |

## Required DataContainers ##

Voxel DataContainer

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Feature | *User Chosen* |  | Array will always be present, since the user may only chose from existing arrays | N/A |


## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | *User Chosen* | | Array will have same name as its **Feature** counterpart |

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users



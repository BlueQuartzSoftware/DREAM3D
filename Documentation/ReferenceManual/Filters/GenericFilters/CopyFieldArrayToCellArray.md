Copy Field Array To Cell Array {#copyfieldarraytocellarray}
==============================

## Group (Subgroup) ##

Generic Filters (Memory/Management)

## Description ##

This filter copies the values associated with a **Field** to all the **Cells** that belong to that **Field**.  Visualization files write only the **Cell** attributes, so if the user wants to display a spatial map of a **Field** level attribute, this filter will transfer that information down to the **Cell** level. One issue to consider is the memory required when expanding from the number of **Fields** to the number of **Cells**.

## Parameters ##

| Name | Type |
|------|------|
| Field Array Name | String |

## Required DataContainers ##

Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Field | *User Chosen* |  | Array will always be present, since the user may only chose from existing arrays | N/A |


## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | *User Chosen* | | Array will have same name as its **Field** counterpart |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL), 2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


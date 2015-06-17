Copy Feature Array To Cell Array {#copyfeaturearraytoelementarray}
==============================

## Group (Subgroup) ##

Core Filters (Memory/Management)

## Description ##

This filter copies the values associated with a **Feature** to all the **Cells** that belong to that **Feature**.  Visualization files write only the **Cell** attributes, so if the user wants to display a spatial map of a **Feature** level attribute, this filter will transfer that information down to the **Cell** level. One issue to consider is the memory required when expanding from the number of **Features** to the number of **Cells**.

## Parameters ##

| Name | Type | Description |
|------|------| ----------- |
| New Element Array Name | String | |

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature  | Feature Array To Copy To Element Data | Int | (1) | Feature Array to Copy To Element Data |
| Cell  | Element Feature Ids  | Any | (1) | Element Feature Ids  |


## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell  | New Element Array Name | Any | (1) | Element Array  |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users



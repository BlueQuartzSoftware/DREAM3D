Link Feature Map To Element Array {#linkfeaturemaptoelementarray}
=============

## Group (Subgroup) ##
Core Filters (Memory/Management)

## Description ##
A **Cell** array is used as the index into the **Feature** map for each **Cell**.  This filter allows the user to specify which **Cell** array is that index.  The filter clears the current **Feature** map, creates a new **Feature** map and resizes it the largest value found in the user selected **Cell** array.  Further the filter creates the *Active* array in the new **Feature** map, which identifies whether each **Feature** *exists* by checking if there is *at least* one **Cell** that belongs to each **Feature**. 

## Parameters ##

| Name | Type | Description |
|------|------|--------------|
| Feature Attribute Matrix Name| String | Cell Feature Attribute Matrix Name |
| Active | String | Active Array |

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell  | None         | Any | (1) | Element Array To Link |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature  | Active | Any | (1) | Active Array |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users



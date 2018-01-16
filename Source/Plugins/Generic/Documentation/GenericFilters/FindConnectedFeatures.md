Find Connected Features {#findconnectedfeatures}
=============

## Group (Subgroup) ##
Generic (Generic)

## Description ##
This **Filter** uses a neighbor list to find all features that are connected to a given feature or phase.

Examples:
1.  Using the **Feature** algorithm and inputting a feature id of 5, this filter will find all features connected to feature 5.

2.  Using the **Phase** algorithm and inputting a phase id of 1, this filter will first find all features that are a part of phase 1, and then find all features connected to those features.

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Algorithm Type | int | Integer that stores what algorithm type the filter is using |
| Selected Feature Id | int | The feature id that will be used to find connected features (Feature algorithm only) |
| Selected Phase Id | int | The phase id that will be used to find connected features (Phase algorithm only) |

## Required Geometry ##
Not Applicable

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Feature Attribute Array** | NeighborList | NeighborList | (1) | The neighbor list used to find connected features |
| **Feature Attribute Array** | Phases | int32_t | (1) | The phases array used to find the features with a given phase (Phase algorithm only) |

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Feature Attribute Array** | ConnectedFeatures | bool | (1) | The connected features array that marks a feature id true if it is connected or false if it is not connected |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users
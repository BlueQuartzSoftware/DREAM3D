Flatten Image {#flattenimage}
=============

## Group (Subgroup) ##
Processing (Image)

## Description ##
This **Filter** allows the user to select a *flattening* method for turning an array of RGB or RGBa values into grayscale values.  The options available in this **Filter** are *Average* and *Luminosity*.  If *Average* is selected, then the R, G and B values are averaged to get a single grayscale value.  If *Luminosity* is selected, then the grayscale value equals (R*0.21)+(G*0.72)+(B*0.07).

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Flattening Method | Enumeration | Which method to use when flattening the RGB array |

## Required Geometry ##
Not Applicable

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Element Attribute Array** | ImageData | uint8_t | (3) or (4) | RGB or ARGB values |

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| **Element Attribute Array** | FlatImageData | uint8_t | (1) | Grayscale values |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users



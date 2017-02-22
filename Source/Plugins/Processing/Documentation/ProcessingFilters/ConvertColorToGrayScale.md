Convert Color Data to GrayScale Data {#convertcolortograyscale}
=============

## Group (Subgroup) ##

Processing (Image)

## Description ##

This **Filter** allows the user to select a *flattening* method for turning an array of RGB or RGBa values into grayscale values.

+ Luminosity: The **luminosity** method is a more sophisticated version of the average method. It also averages the values, but it forms a weighted average to account for human perception. We’re more sensitive to green than other colors, so green is weighted most heavily. The default formula for luminosity is BT709 Greyscale: Red: 0.2125 Green: 0.7154 Blue: 0.0721. The user can set the weightings to what ever values they would like.
+ Average  (R + G + B) / 3
+ Lightness (max(R, G, B) + min(R, G, B)) / 2
+ Color Channel: The user selects a specific R|G|B channel to use as the GrayScale values.

The user can select 1 or more image data arrays which are assumed to be multi-component arrays of unsigned 8 bit values. The user can create a new AttributeMatrix if they want to store all the newly created arrays in a separate AttributeMatrix.

### Additional GrayScale Conversions ###

The following are some additional accepted grayscale conversions

+ RMY Greyscale: Red: 0.5 Green: 0.419 Blue: 0.081
+ (YIQ/NTSC): Red: 0.299 Green: 0.587 Blue: 0.114


## Parameters ##

| Name | Type | Description |
|------|------|------|
| Flattening Method | Enumeration | Which method to use when flattening the RGB array |
| Luminosity Triplet | 3xFloats | Only used if Luminosity is selected as conversion method |
| Color Channel | 1 Int | Only needed if the Color Channel conversion method is selected |
| Output Array Prefix | String | This prefix will be added to each array name that is selected for conversion to form the new array name |

## Required Geometry ##

Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Element Attribute Array(s)** | ImageData | uint8_t | (3) or (4) | RGB or RGBA values |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| **Element Attribute Array** | GrayScale Data | uint8_t | (1) | Grayscale values |
| AttributeMatrix (Optional)  |                |         |     |                  |


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)



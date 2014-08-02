Auto Threshold Image {#autothreshold}
=====

## Group (Subgroup) ##
ImageProcessing (ImageProcessing)


## Description ##
Thresholds an 8 bit array to 0 and 255 using the selected method. Values below the selected value will be set
to 0 (black) and above will be set to 255 (white). Manual Parameter is threshold value for manual selection
and power for robust automatic selection.

## Parameters ##
| Name             | Type |
|------------------|------|
| Array to Process | String |
| Overwrite Array| Bool |
| Created Array Name | String |
| Threshold Method | String |
| Slice at a Time | Bool|
| Manual Parameter | Int |

## Required Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| uint8_t | ImageData | 8 bit image data       | |


## Created Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| uint8_t | ProcessedArray | 8 bit image data       | |



## Authors: ##

**Contact Info:** Will Lente willlenthe@gmail.com

**Version:** 1.0.0



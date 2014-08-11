Find Maxima {#findmaxima}
=====

## Group (Subgroup) ##
ImageProcessing (ImageProcessing)


## Description ##
Finds local maxima with intensity of at least the specified minimum value. If more than 1 pixel share the peak value, all will be considered a maxima. Note: minimum intensity value will be cast to the same type as the array being processed.

## Parameters ##
| Name             | Type |
|------------------|------|
| Array to Process | String |
| Created Array Name | String |
| Minimum Intensity| Double |

## Required Arrays ##

| Type | Default Array Name |
|------|--------------------|
| Any Scalar | ImageData |


## Created Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| bool | Maxima | local maxima       | |



## Authors: ##

**Contact Info:** Will Lente willlenthe@gmail.com

**Version:** 1.0.0



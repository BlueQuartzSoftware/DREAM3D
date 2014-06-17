RGB to Gray {#rgbtogray}
=====

## Group (Subgroup) ##
ImageProcessing (ImageProcessing)


## Description ##
converts a color array to grayscale with the specified weightings. Graylevel = red*red weight+green*green weight+blue*blue weight. Weights are normalized so they sum to 1.

## Parameters ##
| Name             | Type |
|------------------|------|
| Selected Array | String |
| Color Weights | 3*double |

## Required Arrays ##

| Type | Default Array Name | Description |
|------|--------------------|-------------|
| uint8 | Selected | 3 component rgb image data |


## Created Arrays ##

| Type | Default Array Name | Description |
|------|--------------------|-------------|
| uint8 | Selected | 1 component grayscale image data |



## Authors: ##

**Contact Info:** willlenthe@gmail.com

**Version:** 1.0.0



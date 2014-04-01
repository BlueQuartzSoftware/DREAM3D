Watershed {#watershed}
=====

## Group (Subgroup) ##
ImageProcessing (ImageProcessing)


## Description ##
This filter segments grayscale images into grains using ITK's watershed segmentation

## Parameters ##
| Name             | Type |
|------------------|------|
| Array to Process | String |
| Watershed threshold | float |
| Watershed level | float |

## Required Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| Int  | ImageData | 8 bit image data        | |


## Created Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| Int  | Grain ID | | |


## Authors: ##

**Contact Info:** Will Lente willlenthe@gmail.com

**Version:** 1.0.0



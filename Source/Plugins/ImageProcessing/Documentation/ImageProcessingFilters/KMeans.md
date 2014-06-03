K-Means Clustering {#kmeans}
=====

## Group (Subgroup) ##
ImageProcessing (ImageProcessing)


## Description ##
Splits an image into (Classes) classes using k-means clustering. Initial cluster means are evenly spaced between minimum and maximum image values.

## Parameters ##
| Name             | Type |
|------------------|------|
| Array to Process | String |
| Overwrite Array| Bool |
| Created Array Name | String |
| Slice at a Time | Bool|
| Number of Classes | Int |

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



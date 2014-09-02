Auto Threshold Image {#binarywatershed}
=====

## Group (Subgroup) ##
ImageProcessing (ImageProcessing)


## Description ##
Performs a binary watershed operation to split concave objects in a binary image. The watershed using a distance map instead of a grayscale gradient. Watershed regions are seeded using ultimate points to avoid over splitting the image. Ultimate points are found as maxima on the distance map using the algorithm of "Find Maxima". As a result a higher noise tolerance will reject more maxima on the distance map and therefore split concave objects more conservatively (while a lower value will split more agressively).

## Parameters ##
| Name             | Type |
|------------------|------|
| Array to Process | String |
| Peak Noise Tolerance | float |
| Created Array Name | String |


## Required Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| boolean | ThresholdArray | boolean array to be watershed       | |


## Created Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| boolean | WatershedArray | watershedded array | |



## Authors: ##

**Contact Info:** Will Lente willlenthe@gmail.com

**Version:** 1.0.0



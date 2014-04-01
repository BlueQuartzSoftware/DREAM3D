ImageRegistration {#imageregistration}
=====

## Group (Subgroup) ##
ImageProcessing (ImageProcessing)


## Description ##
Uses itk's rigid translation image registration to calculate slice alignments using the specified image comparison metric on the selected array. If apply shifts is selected subpixel shifts well be applied with linear interpolation.

## Parameters ##
| Name             | Type |
|------------------|------|
| Selected Array | String |
| Similarity Metric | String |
| Minimum Step | float |
| Maximum Step | float |
| Maximum Iterations | float |
| Apply Shifts | bool |
| Write Alignment Shift File | bool |
| Alignment File | string |

## Required Arrays ##

| Type | Default Array Name | 
|------|--------------------|
| UInt8  | ImageData     |


## Created Arrays ##

None



## Authors: ##

**Contact Info:** Will Lenthe willlenthe@gmail.com

**Version:** 1.1.0



Multi-EMMPM {#multiemmpmfilter}
=====

## Group (Subgroup) ##
Filter Group (Filter Subgroup)


## Description ##
This filter performs the EM/MPM algorithm on an entire AttributeMatrix worth of DataArrays. The user needs to select the AttributeMatrix that contains the image data (8 bit, unsigned char only) data and the filter will perform EM/MPM on each data array. The segmented images will be stored into a newly created AttributeMatrix where the name of each output array will be the a user defined **prefix** + the original name of the input array.

The filter author highly recommends reading the original paper on the subject to gain an understanding of the values for the input parameters.

There is also an option to use the last Mu (mean) and Sigma (variance) values calculated on the current array as the initialization values for the next data array to process. Using this can help the EM/MPM algorithm achieve subjectively "better" segmentations by starting the algorithm at values that should be close to the ending values. **NOTE** This option should _only_ be used if all of the images are similar to one another. Something like a montage/tiled data set or a 3D stack of images. If the input arrays are vastly different using this option can have negative effects on the accuracy of the final segmented images.

## Input Parameters ##

| Name | Type | Description |
|------|------| ----------- |
| Input AttributeArrays | Strings | List of AttributeArrays to segment |
| Num Classes | Integer | The number of Classes to segment the *image* into. | 
| Exchange Energy | float | The **Exchange Energy** input | 
| Histogram (EM) Loops | int | The number of Histogram loops to perform |
| Segmentation (EMPM) Loops | int | The number of Segmentation loops |
| Use Simulated Annealing | bool | Simulate an Annealing process |
| Use Gradient Penalty | bool | Use a penalty to gradients when segmenting |
| Gradient Penalty | float | The penalty to apply for gradients |
| Use Curvature Penalty | bool | Use a penalty for curvature |
| Curvature Penalty | float | The penalty to use for curvatures |
| R Max | float | The max radius for the curvature penalty |
| EM Loop Delay | int | The number of EM Loops to delay before applying the curvature penalty |


## Output Parameters ##

| Name | Type | Description |
|------|------| ----------- |
| Use Mu/Sigma as initializers | Bool |  |
| Output AttributeMatrix Name | String | The name of the new output AttributeMatrix to create |
| Output AttributeArray Name Prefix | String | A prefix to apply to each created output AttributeArray |



## Required Geometry ##

Image / Rectilinear Grid

## Required Arrays ##

| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| Unsigned 8 Bit Integers  | | uint 8 | (1)  | 8 Bit Grayscale images only |


## Created Arrays ##

| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| Unsigned 8 Bit Integers  |  |  uint 8   | (1) | The value is the class that the specific point was segmented into |


## Citations ##
H. C. Chuang, M. L. Comer, and J. Simmons, “Texture Segmentation in Microstructure Images of Advanced Materials,” In Proceedings of the _2008 IEEE Southwest Symposium on Image Analysis and Interpretation_, March 24-26, 2008, Santa Fe, NM

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


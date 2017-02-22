Multi-Array EM/MPM {#multiemmpmfilter}
=============

## Group (Subgroup) ##
Reconstruction (Segmentation)

## Description ##
This **Filter** performs the EM/MPM algorithm on a selected number of **Atribute Arrays**, representing grayscale images, that all belong to the same **Cell Attribute Matrix**. The user may select any number of **Attribute Arrays** to segment.  The _Select/Deselect All_  button can used to automatically select all **Attribute Arrays** in a given **Attribute Matrix**. The segmented images will be stored into a newly created **Cell Attribute Matrix** where the name of each output array will be the a user defined _prefix_ plus the original name of the input array. For information regarding the operation of the EM/MPM algorithm and the meaning of the parameters, please refer to the documentation for the [EM/MPM](EMMPMFilter.html "") **Filter**.

This **Filter** contains an additional option to use the last mu (mean) and sigma (variance) values calculated on the current array as the initialization values for the next **Attribute Array** to process. Using this can help the EM/MPM algorithm achieve subjectively "better" segmentations by starting the algorithm at values that should be close to the ending values. This option should _only_ be used if all of the images are "similar" to one another (e.g., a montage/tiled data set or a 3D stack of images). If the input **Attribute Arrays** are qualitatively different, using this option can have negative effects on the accuracy of the final segmented images.

## Input Parameters ##
| Name             | Type | Description |
|------------------|------|-------------|
| Number of Classes | int32_t | The number of classes in which to segment the image | 
| Exchange Energy | float | The value of the exchange energy | 
| Histogram Loops (EM) | int32_t | The number of histogram loops (EM) to perform |
| Segmentation Loops (MPM) | int32_t | The number of segmentation loops (MPM) to perform |
| Use Simulated Annealing | bool | Apply the simulated annealing process |
| Use Gradient Penalty | bool | Use a penalty to gradients when segmenting |
| Gradient Penalty (Beta E) | float | The penalty to apply for gradients. Only needed if _Use Gradient Penalty_ is checked |
| Use Curvature Penalty | bool | Use a penalty for curvature |
| Curvature Penalty | float | The penalty to use for curvatures. Only needed if _Use Curvature Penalty_ is checked |
| R Max | float | The max radius for the curvature penalty. Only needed if _Use Curvature Penalty_ is checked |
| EM Loop Delay | int32_t | The number of EM Loops to delay before applying the curvature penalty. Only needed if _Use Curvature Penalty_ is checked |
| Use 1-Based Values | bool | Use 1-based values instead of 0-based values |
| Use Mu/Sigma from Previous Image as Initialization for Current Image | bool | Whether to use the calculated mu/sigma from the previous segmented image as the starting point for the next image segmentation. May help reduce computation time |
| Output Array Name Prefix | String | Prefix to apply to the output segmented arrays |

## Required Geometry ##
Image

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Arrays** | None | uint8_t | (1)  | 8 bit grayscale images to segment. The user may select any number of arrays to segment |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Arrays** | None | uint8_t | (1) | Unsigned 8 bit array representing the value of the class that the **Cell** was segmented into. An output array is created for each segmented input array and placed in a new **Cell Attribute Matrix** that the user may name |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


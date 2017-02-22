EM/MPM {#emmpmfilter}
=============

## Group (Subgroup) ##
Reconstruction (Segmentation)

## Description ##
This **Filter** performs the EM/MPM segmentation algorithm on an **Attribute Array** representing a grayscale image. The EM/MPM algorithm employs an advanced expectation maximization routine over Gaussian mixtures to determine an image segmeneation into a defined number of classes. The segmented image will be stored into a new **Attribute Array** with a user definable name. Note that the created segmentation will have **Cell** labels defining the class membership.  Thus, the labels will be unsigned 8 bit integers, matching the incoming grayscale image.  These labels can be considered **Feature** Ids for the purposes of most DREAM.3D analysis routines.  However, DREAM.3D assumes that **Feature** Ids are signed 32 bit integers.  It may therefore be required to use the [Convert Attribute Data Type](ConvertData.html "") **Filter** to convert the segmented image labels from unsigned 8 bit integers to signed 32 bit integers for further analysis.  

**It is highly recommended that users consult references [1], [2], [3], and [4] for details on the impact of particular parameters on the EM/MPM algorithm.**

## Parameters ##
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

## Required Geometry ##
Image

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | None | uint8_t | (1)  | 8 bit grayscale image to segment |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | None | uint8_t | (1) | Unsigned 8 bit array representing the value of the class that the **Cell** was segmented into |

## References ##

[1] H.C. Chuang, M.L. Comer, J.P. Simmons, Texture Segmentation in Microstructure Images of Advanced Materials, In Proceedings of the 2008 IEEE Southwest Symposium on Image Analysis and Interpretation, March 24-26, 2008, Santa Fe, NM

[2] M.L. Comer, The EM/MPM Algorithm for Segmentation of Textured Images: Analysis and Further Experimental Results, IEEE Transactions on Image Processing, vol. 9, no. 10, October 2000. 

[3] J.P. Simmons, P. Chuang, M. Comer, J.E. Spowart, M.D. Uchic, M. De Graef, Application and further development of advanced image processing algorithms for automated analysis of serial section image data, Modelling Simul. Mater. Sci. Eng. 17 (2009). 

[4] D. Doria, Expectation Maximization of Gaussian Mixture Models in VTK - Release 0.00, September 21, 2010, Insight Journal [http://hdl.handle.net/10380/3218](http://hdl.handle.net/10380/3218)


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


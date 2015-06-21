EM/MPM {#emmpmfilter}
=============

## Group (Subgroup) ##
Reconstruction (Segmentation)

## Description ##
This Filter performs the EM/MPM segmentation algorithm on an **Attribute Array** representing a grayscale image. The EM/MPM algorithm employs an advanced expectation maximization routine over Gaussian mixtures to determine an image segmeneation into a defined number of classes. The segmented image will be stored into a new **Attribute Array** with a user definable name. Note that the created segmentation will have **Cell** labels defining the class membership.  Thus, the labels will be unsigned 8 bit integers, matching the incoming grayscale image.  These labels can be considered **Feature** Ids for the purposes of most DREAM.3D analysis routines.  However, DREAM.3D assumes that **Feature** Ids are signed 32 bit integers.  It may therefore be required to use the [Convert Attribute Data Type](ConvertData.html "") Filter to convert the segmented image labels from unsigned 8 bit integers to signed 32 bit integers for further analysis.  

**It is highly recommended that users consult references [1], [2], [3], and [4] for details on the impact of particular parameters on the EM/MPM algorithm.**

[1] H.C. Chuang, M.L. Comer, and J.P. Simmons, Texture Segmentation in Microstructure Images of Advanced Materials, In Proceedings of the 2008 IEEE Southwest Symposium on Image Analysis and Interpretation, March 24-26, 2008, Santa Fe, NM

[2] M.L. Comer, The EM/MPM Algorithm for Segmentation of Textured Images: Analysis and Further Experimental Results, IEEE Transactions on Image Processing, vol. 9, no. 10, October 2000. 

[3] J.P. Simmons, P. Chuang, M. Comer, J.E. Spowart, M.D. Uchic, and M. De Graef, Application and further development of advanced image processing algorithms for automated analysis of serial section image data, Modelling Simul. Mater. Sci. Eng. 17 (2009). 

[4] D. Doria, Expectation Maximization of Gaussian Mixture Models in VTK - Release 0.00, September 21, 2010, Insight Journal [http://hdl.handle.net/10380/3218](http://hdl.handle.net/10380/3218)

## Parameters ##
| Name             | Type | Description |
|------------------|------|-------------|
| Number of Classes | Int | The number of classes in which to segment the image | 
| Exchange Energy | Float | The value of the exchange energy | 
| Histogram Loops (EM) | Int | The number of histogram loops (EM) to perform |
| Segmentation Loops (MPM) | Int | The number of segmentation loops (MPM) to perform |
| Use Simulated Annealing | Bool | Apply the simulated annealing process |
| Use Gradient Penalty | Bool | Use a penalty to gradients when segmenting |
| Gradient Penalty (Beta E) | Float | The penalty to apply for gradients. Only needed if _Use Gradient Penalty_ is checked |
| Use Curvature Penalty | Bool | Use a penalty for curvature |
| Curvature Penalty | Float | The penalty to use for curvatures. Only needed if _Use Curvature Penalty_ is checked |
| R Max | Float | The max radius for the curvature penalty. Only needed if _Use Curvature Penalty_ is checked |
| EM Loop Delay | Int | The number of EM Loops to delay before applying the curvature penalty. Only needed if _Use Curvature Penalty_ is checked |

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| Cell  | None | UInt8 | (1)  | 8 bit grayscale image to segment |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| Cell | None | UInt8 | (1) | Unsigned 8 bit array representing the value of the class that the **Cell** was segmented into |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


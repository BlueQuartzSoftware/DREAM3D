Find Feature Face Curvature {#featurefacecurvaturefilter}
=============

## Group (Subgroup) ##
Surface Meshing (Curvature)

## Description ##
This **Filter** calculates the _mean_ and _Gaussian_ curvature for each **Triangle** in a **Triangle Geometry** using the technique in [1]. The groups of **Triangles** over which to compute the curvatures is determines by the **Features** they are associated, deonoted by their _Face Labels_. The curvature information will be stored in a new **Edge Attribute Matrix**.

Principal Curvatures 1 and 2 are the &kappa;<sub>1</sub> and &kappa;<sub>2</sub> from [1] and are the eigenvalues from the Wiengarten matrix. The Principal Directions 1 and 2 are the eigenvectors from the solution to the least squares fit algorithm. The Mean Curvature is (&kappa;<sub>1</sub> + &kappa;<sub>2</sub>)/2, while the Gaussian curvature is (&kappa;<sub>1</sub> * &kappa;<sub>2</sub>).

-----

![Curvature Coloring of a Feature](FeatureFaceCurvatureFilter.png)
@image latex FeatureFaceCurvatureFilter.png "Curvature Coloring of a Feature" width=6in

-----


## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Neighborhood Ring Count | int32_t | The size of the nieghborhood to use to calculate the curvature values |
| Compute Principal Direction Vectors | bool | Whether to compute the principal direction vectors |
| Compute Gaussian Curvature | bool | Whether to compute the Gaussian curvature values |
| Compute Mean Curvature | bool | Whether to compute the mean curvature values |
| Use Face Normals for Curve Fitting | bool | Whether to use the **Face** normals to improve the least squares fit |
| Edge Attribute Matrix Name | String | Created **Edge Attribute Matrix** name |

## Required Geometry ##
Triangle

## Required Arrays ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Face Attribute Array** | FaceLabels | int32_t | (2) | Specifies which **Features** are on either side of each **Face** |
| **Face Attribute Array** | FeatureFaceIds | int32_t | (1) | Specifies to which **Feature** boundary each **Face** belongs |
| **Face Attribute Array** | FaceNormals | double | (3) | Specifies the normal of each **Face** |
| **Face Attribute Array** | FaceCentroids | double | (3) | Specifies the centroid of each **Face** |

## Created Arrays ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Attribute Matrix** | EdgeData | Edge | N/A | Created **Edge Attribute Matrix** name |
| **Edge Attribute Array** | PrincipalCurvature1 | double | (2) | First set eigenvalues of the Wiengarten matrix (&kappa;<sub>1</sub>). Only created if _Compute Principal Direction Vectors_ is checked |
| **Edge Attribute Array** | PrincipalCurvature2 | double | (2) | Second set eigenvalues of the Wiengarten matrix (&kappa;<sub>2</sub>). Only created if _Compute Principal Direction Vectors_ is checked |
| **Edge Attribute Array** | PrincipalDirection1 | double | (3) | First set of eigenvectors. Only created if _Compute Principal Direction Vectors_ is checked |
| **Edge Attribute Array** | PrincipalDirection2 | double | (3) | Second set of eigenvectors. Only created if _Compute Principal Direction Vectors_ is checked |
| **Edge Attribute Array** | MeanCurvatures      | double | (2) | Mean curvature values. Only created if _Compute Mean Curvature_ is checked |
| **Edge Attribute Array** | GaussianCurvatures  | double | (2) | Gaussian curvature values. Only created if _Compute Gaussian Curvature_ is checked |

## References ##
[1] J. Goldfeather, V. Interrante, "A Novel Cubic-Order Algorithm for Approximating Principal Direction Vectors", ACM Transactions on Graphics 2004, 23(1), pp. 45-63.

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users



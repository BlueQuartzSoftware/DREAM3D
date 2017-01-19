Array Calculator {#arraycalculator}
=============

## Group (Subgroup) ##
LGPLToolbox (LGPLToolbox)

## Description ##
This **Filter** element wise evaluates mathematical expressions with existing arrays as variables, saving the resulting scalar in a new array. Expressions are entered as plain text. Selected scalar arrays are their own variable name while selected 1D vectors are 0 indexed with brackets:
| Expression | Result |
|------------|--------|
| (Scalar1 + Scalar2) / 2 | element wise average of the scalar arrays 'Scalar1' and 'Scalar2'|
| 2 * Dir[0] - Dir[1] + 3 * Dir[2] | element wise dot product of the 1D vector array 'Dir' (having a tuple size of at least 3) with {2, -1, 3}|

All values are cast to a double before evaluation. The following symbols are available, (and equivalent to c++ functions with the same name unless otherwise noted):

 * **binary operators:** + - * / ^ (std::pow) % (std::fmod)
 * **trigonometric:** sin, cos, tan, tan2, atin, acos, atan
 * **hyperbolic:** sinh, cosh, tanh
 * **exponential:** exp, log, log10, sqrt
 * **rounding:** ceil, floor
 * **other:** fabs, fmin, fmax

## Parameters ##
| Name | Type | Description |
|------|------|-------------|
| Expression | String | mathematical expression to apply |
| Input Arrays | Attribute Array Names | variables to include in the *Expression* |
| Created Array | String | name of creating array holding element wise result |

## Required Geometry ##
Any

## Required Objects ##
| Kind | Type | Component Dimensions | Description |
|------|------|----------------------|-------------|
| **Attribute Matrix** | Any | N/A | attribute matrix containing variable arrays (location of created array)
| **Selected Attribute Arrays** | Any (converted to double for evaluation) | scalar or 1D vector | variables for *Expression* |

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions |
|------|--------------|------|----------------------|
| Attribute Array | Scalar | double | 1 |

## License & Copyright ##
Copyright (c) 2015 The Regents of the University of California, Author: William Lenthe  

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users
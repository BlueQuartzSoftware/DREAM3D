
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//


#ifndef _VTKH5CONSTANTS_H_
#define _VTKH5CONSTANTS_H_


#define H5_VTK_OBJECT_INDEX_PATH "/VTK_OBJECT_INDEX"
#define H5_VTK_DATA_OBJECT  "VTK_DATA_OBJECT"
#define H5_VTK_POLYDATA     "VTK_POLYDATA"
#define H5_VTK_UNSTRUCTURED_GRID "VTK_UNSTRUCTURED_GRID"
#define H5_VTK_STRUCTURED_POINTS "VTK_STRUCTURED_POINTS"
#define H5_NAME             "NAME"
#define H5_VTK_DATASET_ATTRIBUTE "VTK_DATASET_ATTRIBUTE"

#define H5_ENSEMBLE_DATA_GROUP_NAME "ENSEMBLE_DATA"
#define H5_VERTEX_ENSEMBLE_DATA_GROUP_NAME "VERTEX_ENSEMBLE_DATA"
#define H5_EDGE_ENSEMBLE_DATA_GROUP_NAME "EDGE_ENSEMBLE_DATA"
#define H5_FACE_ENSEMBLE_DATA_GROUP_NAME "FACE_ENSEMBLE_DATA"
#define H5_CELL_ENSEMBLE_DATA_GROUP_NAME "CELL_ENSEMBLE_DATA"
#define H5_ENSEMBLE_DATA_DEFAULT "EnsembleData"
#define H5_VERTEX_ENSEMBLE_DATA_DEFAULT "VertexEnsembleData"
#define H5_EDGE_ENSEMBLE_DATA_DEFAULT "EdgeEnsembleData"
#define H5_FACE_ENSEMBLE_DATA_DEFAULT "FaceEnsembleData"
#define H5_CELL_ENSEMBLE_DATA_DEFAULT "CellEnsembleData"

#define H5_FIELD_DATA_GROUP_NAME "FEATURE_DATA"
#define H5_VERTEX_FIELD_DATA_GROUP_NAME "VERTEX_FEATURE_DATA"
#define H5_EDGE_FIELD_DATA_GROUP_NAME "EDGE_FEATURE_DATA"
#define H5_FACE_FIELD_DATA_GROUP_NAME "FACE_FEATURE_DATA"
#define H5_CELL_FIELD_DATA_GROUP_NAME "CELL_FEATURE_DATA"
#define H5_FIELD_DATA_DEFAULT    "FeatureData"
#define H5_VERTEX_FIELD_DATA_DEFAULT    "VertexFeatureData"
#define H5_EDGE_FIELD_DATA_DEFAULT    "EdgeFeatureData"
#define H5_FACE_FIELD_DATA_DEFAULT    "FaceFeatureData"
#define H5_CELL_FIELD_DATA_DEFAULT    "CellFeatureData"
#define H5_CELL_DATA_GROUP_NAME  "CELL_DATA"
#define H5_VERTEX_DATA_GROUP_NAME  "VERTEX_DATA"
#define H5_POINT_DATA_GROUP_NAME "POINT_DATA"
#define H5_SCALAR_DATA_GROUP_NAME "SCALARS"
#define H5_FACE_DATA_GROUP_NAME "FACE_DATA"
#define H5_EDGE_DATA_GROUP_NAME "EDGE_DATA"

#define H5_POINTS                "POINTS"
#define H5_VERTICES              "VERTICES"
#define H5_LINES                 "LINES"
#define H5_POLYGONS              "POLYGONS"
#define H5_TRIANGLE_STRIPS       "TRIANGLE_STRIPS"

#define H5_CELLS                  "CELLS"
#define H5_CELL_TYPES             "CELL_TYPES"

#define H5_NUMCOMPONENTS          "NumComponents"

#define H5_DEFAULT       "default"

#define H5_DIMENSIONS          "DIMENSIONS"
#define H5_ORIGIN              "ORIGIN"
#define H5_SPACING             "SPACING"
#define H5_POINT_DATA          "POINT_DATA"
#define H5_NUMBER_OF_POINTS    "NUM_POINTS"

#define H5_ACTIVE_SCALARS      "ActiveScalars"
#define H5_SCALARS             "SCALARS"
#define H5_SCALARS_DEFAULT     "scalars"

#define H5_COLOR_SCALARS    "COLOR_SCALARS"
#define H5_COlOR_SCALARS_DEFAULT "color_scalars"

#define H5_LOOKUP_TABLE    "LOOKUP_TABLE"
#define H5_LOOKUP_TABLE_DEFAULT "lookup_table"

#define H5_ACTIVE_VECTORS      "ActiveVectors"
#define H5_VECTORS         "VECTORS"
#define H5_VECTORS_DEFAULT "vectors"

#define H5_ACTIVE_NORMALS      "ActiveNormals"
#define H5_NORMALS    "NORMALS"
#define H5_NORMALS_NAME "Normals"
#define H5_NORMALS_DEFAULT "normals"

#define H5_ACTIVE_TEXTURE_COORDINATES      "ActiveTextureCoordinates"
#define H5_TEXTURE_COORDINATES  "TEXTURE_COORDINATES"
#define H5_TEXTURE_COORDINATES_DEFAULT "tcoords"

#define H5_ACTIVE_TENSORS      "ActiveTensors"
#define H5_TENSORS "TENSORS"
#define H5_TENSORS_DEFAULT "tensors"

#define H5_ACTIVE_GLOBAL_IDS      "ActiveGlobalIds"
#define H5_GLOBAL_IDS   "GLOBAL_IDS"
#define H5_GLOBAL_IDS_DEFAULT  "global_ids"

#define H5_ACTIVE_PEDIGREE_IDS      "ActivePedigreeIds"
#define H5_PEDIGREE_IDS "PEDIGREE_IDS"
#define H5_PEDIGREE_IDS_DEFAULT "pedigree_ids"

#define VTK_CELLTYPE_EMPTY_CELL        0
#define VTK_CELLTYPE_VERTEX            1
#define VTK_CELLTYPE_POLY_VERTEX       2
#define VTK_CELLTYPE_LINE              3
#define VTK_CELLTYPE_POLY_LINE         4
#define VTK_CELLTYPE_TRIANGLE          5
#define VTK_CELLTYPE_TRIANGLE_STRIP    6
#define VTK_CELLTYPE_POLYGON           7
#define VTK_CELLTYPE_PIXEL             8
#define VTK_CELLTYPE_QUAD              9
#define VTK_CELLTYPE_TETRA             10
#define VTK_CELLTYPE_VOXEL             11
#define VTK_CELLTYPE_HEXAHEDRON        12
#define VTK_CELLTYPE_WEDGE             13
#define VTK_CELLTYPE_PYRAMID           14
#define VTK_CELLTYPE_PENTAGONAL_PRISM  15
#define VTK_CELLTYPE_HEXAGONAL_PRISM   16

// Quadratic isoparametric cells
#define VTK_CELLTYPE_QUADRATIC_EDGE                    21
#define VTK_CELLTYPE_QUADRATIC_TRIANGLE                22
#define VTK_CELLTYPE_QUADRATIC_QUAD                    23
#define VTK_CELLTYPE_QUADRATIC_TETRA                   24
#define VTK_CELLTYPE_QUADRATIC_HEXAHEDRON              25
#define VTK_CELLTYPE_QUADRATIC_WEDGE                   26
#define VTK_CELLTYPE_QUADRATIC_PYRAMID                 27
#define VTK_CELLTYPE_BIQUADRATIC_QUAD                  28
#define VTK_CELLTYPE_TRIQUADRATIC_HEXAHEDRON           29
#define VTK_CELLTYPE_QUADRATIC_LINEAR_QUAD             30
#define VTK_CELLTYPE_QUADRATIC_LINEAR_WEDGE            31
#define VTK_CELLTYPE_BIQUADRATIC_QUADRATIC_WEDGE       32
#define VTK_CELLTYPE_BIQUADRATIC_QUADRATIC_HEXAHEDRON  33
#define VTK_CELLTYPE_BIQUADRATIC_TRIANGLE              34

// Cubic isoparametric cell
#define VTK_CELLTYPE_CUBIC_LINE                        35

// Special class of cells formed by convex group of points
#define VTK_CELLTYPE_CONVEX_POINT_SET  41

// Polyhedron cell (consisting of polygonal faces)
#define VTK_CELLTYPE_POLYHEDRON  42

// Higher order cells in parametric form
#define VTK_CELLTYPE_PARAMETRIC_CURVE         51
#define VTK_CELLTYPE_PARAMETRIC_SURFACE       52
#define VTK_CELLTYPE_PARAMETRIC_TRI_SURFACE   53
#define VTK_CELLTYPE_PARAMETRIC_QUAD_SURFACE  54
#define VTK_CELLTYPE_PARAMETRIC_TETRA_REGION  55
#define VTK_CELLTYPE_PARAMETRIC_HEX_REGION    56

// Higher order cells
#define VTK_CELLTYPE_HIGHER_ORDER_EDGE         60
#define VTK_CELLTYPE_HIGHER_ORDER_TRIANGLE     61
#define VTK_CELLTYPE_HIGHER_ORDER_QUAD         62
#define VTK_CELLTYPE_HIGHER_ORDER_POLYGON      63
#define VTK_CELLTYPE_HIGHER_ORDER_TETRAHEDRON  64
#define VTK_CELLTYPE_HIGHER_ORDER_WEDGE        65
#define VTK_CELLTYPE_HIGHER_ORDER_PYRAMID      66
#define VTK_CELLTYPE_HIGHER_ORDER_HEXAHEDRON   67

#endif /* VTKH5CONSTANTS_H_ */



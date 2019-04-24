import time

# Based on CreateLambertSphereSurface pipeline example
# Creates a LambertSphereSurface using Python/C++ bindings
#
# There are 2 options. Run using the pipeline and building the filters non-pythonic
# The other option is to "build" the pipeline effect through pythonic API and
# executing each filter upon creation. The choice can be selected by changing the value of runPipeline

# These are the simpl_py python modules

from dream3d import simplpy
from dream3d import simpl
from dream3d import simpl_helpers as sc
from dream3d import simpl_test_dirs as sd
from dream3d import orientationanalysispy as orientationanalysis


def create_lambert_sphere_test():
    run_pipeline = False
    # Create Data Container Array
    dca = simpl.DataContainerArray.New()
    if run_pipeline:
        run_pipeline_test(dca)
    else:
        run_pythonic_test(dca)


def run_pipeline_test(dca):
    # Create Data Container
    data_container = simpl.CreateDataContainer.New()
    data_container.setDataContainerArray(dca)
    data_container.DataContainerName = "ImageDataContainer"

    # Create Geometry (Image)
    image_geom = simpl.CreateImageGeometry.New()
    image_geom.setDataContainerArray(dca)
    image_geom.SelectedDataContainer = "ImageDataContainer"
    image_geom.Dimensions = simpl.IntVec3Type([101, 101, 1])
    image_geom.Origin = simpl.FloatVec3Type([0, 0, 0])
    image_geom.Resolution = simpl.FloatVec3Type([1, 1, 1])

    # Create Attribute Matrix
    attribute_matrix = simpl.CreateAttributeMatrix.New()
    attribute_matrix.setDataContainerArray(dca)
    attribute_matrix.AttributeMatrixType = 3
    new_row = simpl.VectorDouble()
    new_row.append(101)
    new_row.append(101)
    new_row.append(1)
    table_data = list()
    table_data.append(new_row)
    attribute_matrix.TupleDimensions = simpl.DynamicTableData(table_data, ["0", "1", "2"], ["0"])
    attribute_matrix.CreatedAttributeMatrix = simpl.DataArrayPath("ImageDataContainer", "CellAttributeMatrix", "")

    # Create Data Array
    data_array = simpl.CreateDataArray.New()
    data_array.setDataContainerArray(dca)
    data_array.ScalarType = simpl.ScalarTypes.UInt8
    data_array.NumberOfComponents = 1
    data_array.InitializationType = 0
    data_array.InitializationValue = "128"
    data_array.NewArray = simpl.DataArrayPath("ImageDataContainer", "CellAttributeMatrix", "ScalarValues")

    # Create Sphere (Lambert Projection)
    # Note: "Checkboxes" default to true
    create_lambert_sphere = orientationanalysis.CreateLambertSphere.New()
    create_lambert_sphere.setDataContainerArray(dca)
    create_lambert_sphere.Hemisphere = 0
    create_lambert_sphere.ImageDataArrayPath = simpl.DataArrayPath("ImageDataContainer", "CellAttributeMatrix",
                                                                 "ScalarValues")
    create_lambert_sphere.VertexDataContainerName = "VertexDataContainer"
    create_lambert_sphere.EdgeDataContainerName = "EdgeDataContainer"
    create_lambert_sphere.TriangleDataContainerName = "TriangleDataContainer"
    create_lambert_sphere.QuadDataContainerName = "QuadDataContainer"

    # Write DREAM.3D Data File
    data_container_writer = simpl.DataContainerWriter.New()
    data_container_writer.setDataContainerArray(dca)
    data_container_writer.OutputFile = sd.GetTestTempDirectory() + "/LambertSphere.dream3d"
    data_container_writer.WriteXdmfFile = True
    data_container_writer.WriteTimeSeries = False

    # Create  Pipeline
    pipeline = simpl.FilterPipeline.New()
    pipeline.Name = "New Pipeline"

    # Add filters (order is important)
    pipeline.pushBack(data_container)
    pipeline.pushBack(image_geom)
    pipeline.pushBack(attribute_matrix)
    pipeline.pushBack(data_array)
    pipeline.pushBack(create_lambert_sphere)
    pipeline.pushBack(data_container_writer)

    err = pipeline.preflightPipeline()
    if err < 0:
        print("Preflight ErrorCondition: %d" % err)

    pipeline.run()
    err = pipeline.ErrorCondition
    if err < 0:
        print("Execute ErrorCondition: %d" % err)

    time.sleep(2)


def run_pythonic_test(dca):
    err = simplpy.create_data_container(dca, "ImageDataContainer")
    if err < 0:
        print("DataContainer ErrorCondition: %d" % err)

    err = simplpy.create_image_geometry(dca, "ImageDataContainer", simpl.IntVec3Type([101, 101, 1]), simpl.FloatVec3Type([0, 0, 0]),
                                        simpl.FloatVec3Type([1, 1, 1]))
    if err < 0:
        print("ImageGeometry ErrorCondition: %d" % err)

    new_row = simpl.VectorDouble()
    new_row.append(101)
    new_row.append(101)
    new_row.append(1)
    table_data = list()
    table_data.append(new_row)
    err = simplpy.create_attribute_matrix(dca, simpl.DataArrayPath("ImageDataContainer", "CellAttributeMatrix", ""), 3,
                                          simpl.DynamicTableData(table_data, ["0", "1", "2"], ["0"]))
    if err < 0:
        print("AttributeMatrix ErrorCondition: %d" % err)

    err = simplpy.create_data_array(dca, simpl.ScalarTypes.UInt8, 1,
                                    simpl.DataArrayPath("ImageDataContainer", "CellAttributeMatrix", "ScalarValues"),
                                    simpl.InitializationType.Manual, "128", (0.0, 151.1))
    if err < 0:
        print("DataArray ErrorCondition: %d" % err)

    err = orientationanalysis.create_lambert_sphere(dca, sc.Hemisphere.Northern,
                                                    simpl.DataArrayPath("ImageDataContainer", "CellAttributeMatrix",
                                                                        "ScalarValues"),
                                                    "QuadDataContainer", "TriangleDataContainer", "EdgeDataContainer",
                                                    "VertexDataContainer", "VertexAttributeMatrix",
                                                    "EdgeAttributeMatrix", "FaceAttributeMatrix",
                                                    True, True, True, True)
    if err < 0:
        print("LambertSphere ErrorCondition: %d" % err)
    err = simplpy.data_container_writer(dca, sd.GetBuildDirectory() + "/Data/Output/LambertSphere.dream3d", True, False)
    if err < 0:
        print("DataContainerWriter ErrorCondition: %d" % err)


"""
Main entry point for python script
"""
if __name__ == "__main__":
    create_lambert_sphere_test()
    print("Test Complete")

# Pipeline : (03) SmallIN100 Alignment (from EBSD Reconstruction)
#
#

import simpl
import simplpy as d3d
import simpl_helpers as sc
import simpl_test_dirs as sd
import orientationanalysispy as orientation_analysis
import reconstructionpy as reconstruction
import processingpy as processing
import orientationanalysis

def small_in100_alignment():
    # Create Data Container Array
    dca = simpl.DataContainerArray()

    err = sc.ReadDREAM3DFile(dca, sd.GetBuildDirectory() + '/Data/Output/Reconstruction/SmallIN100.dream3d')
    assert err == 0, f'Read DataContainerArray Structure Failed {err}'

    # Threshold Objects
    # Create the selected thresholds / comparison inputs for MultiThresholdObjects filter
    selected_thresholds = simpl.ComparisonInputs()
    selected_thresholds.addInput('Small IN100', 'EBSD Scan Data', 'Confidence Index',
                                simpl.ComparisonOperators.GreaterThan, 0.1)
    selected_thresholds.addInput('Small IN100', 'EBSD Scan Data', 'Image Quality', simpl.ComparisonOperators.GreaterThan,
                                120)

    err = d3d.multi_threshold_objects(dca, 'Mask', selected_thresholds)
    assert err == 0, f'MultiThresholdObjects ErrorCondition: {err}'

    # Convert Orientation Representation
    err = orientation_analysis.convert_orientations(dca, orientationanalysis.OrientationType.Euler, orientationanalysis.OrientationType.Quaternion,
                                                    simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'EulerAngles'),
                                                    'Quats')
    assert err == 0, f'ConvertOrientation ErrorCondition: {err}'

    # Align Sections (Misorientation)
    err = reconstruction.align_sections_misorientation(dca, 5, True,
                                                       simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'Quats'),
                                                       simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'Phases'),
                                                       simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'Mask'),
                                                       simpl.DataArrayPath('Small IN100', 'Phase Data',
                                                                           'CrystalStructures'))
    assert err == 0, f'AlignSectionsMisorientation ErrorCondition: {err}'

    # Isolate Largest Feature (Identify Sample)
    err = processing.identify_sample(dca, False, simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'Mask'))
    assert err == 0, f'IsolateLargestFeature ErrorCondition: {err}'

    # Align Sections (Feature Centroid)
    err = reconstruction.align_sections_feature_centroid(dca,
                                                         0, True,
                                                         simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'Mask'))
    assert err == 0, f'AlignSectionsFeatureCentroid {err}'

    # Generate IPF Colors
    err = orientation_analysis.generate_ipf_colors(dca, simpl.FloatVec3([0, 0, 1]),
                                                   simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'Phases'),
                                                   simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'EulerAngles'),
                                                   simpl.DataArrayPath('Small IN100', 'Phase Data',
                                                                       'CrystalStructures'),
                                                   True,
                                                   simpl.DataArrayPath('Small IN100', 'EBSD Scan Data', 'Mask'),
                                                   'IPFColor')
    assert err == 0, f'GenerateIPFColors ErrorCondition: {err}'

    # Write to DREAM3D file
    err = sc.WriteDREAM3DFile(
        sd.GetBuildDirectory() + '/Data/Output/Reconstruction/03_SmallIN100_Aligned.dream3d',
        dca)
    assert err == 0, f'WriteDREAM3DFile ErrorCondition: {err}'

if __name__ == '__main__':
    small_in100_alignment()

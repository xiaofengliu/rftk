#include "CreateDepthFirstLearner.h"

DepthFirstTreeLearner<float, int> CreateDepthFirstLearner( BufferCollectionKey_t xs_key, 
                                                          BufferCollectionKey_t classes_key, 
                                                          int numberOfClasses, 
                                                          FeatureValueOrdering featureOrdering, 
                                                          double minNodeSize)
{
    // Don't try split if size is above a minimum
    MinNodeSizeCriteria trySplitCriteria(minNodeSize);

    // Tree steps
    std::vector<PipelineStepI*> treeSteps;
    AllSamplesStep<MatrixBufferTemplate<float>, float, int> allSamplesStep(xs_key);
    treeSteps.push_back(&allSamplesStep);
    VectorBufferTemplate<int> numberOfFeaturesBuffer = CreateVector1<int>(2);
    SetBufferStep< VectorBufferTemplate<int> > numberOfFeatures( numberOfFeaturesBuffer, WHEN_NEW );
    treeSteps.push_back(&numberOfFeatures);
    Pipeline treeStepsPipeline(treeSteps);

    // Node steps
    std::vector<PipelineStepI*> nodeSteps;
    AxisAlignedParamsStep<float, int> featureParams(numberOfFeatures.OutputBufferId, xs_key);
    nodeSteps.push_back(&featureParams);
    LinearMatrixFeature<MatrixBufferTemplate<float>, float, int> feature(featureParams.FloatParamsBufferId,
                                                                          featureParams.IntParamsBufferId,
                                                                          allSamplesStep.IndicesBufferId,
                                                                          xs_key); 
    FeatureExtractorStep< LinearMatrixFeature<MatrixBufferTemplate<float>, float, int> > featureExtractor(feature, 
                                                                                                            featureOrdering);
    nodeSteps.push_back(&featureExtractor);
    SliceBufferStep< VectorBufferTemplate<int>, int> sliceClasses(classes_key, allSamplesStep.IndicesBufferId);
    nodeSteps.push_back(&sliceClasses);
    SliceBufferStep< VectorBufferTemplate<float>, int> sliceWeights(allSamplesStep.WeightsBufferId, allSamplesStep.IndicesBufferId);  
    nodeSteps.push_back(&sliceWeights); 
    ClassInfoGainWalker<float, int> classInfoGainWalker(sliceWeights.SlicedBufferId, sliceClasses.SlicedBufferId, numberOfClasses);
    BestSplitpointsWalkingSortedStep< ClassInfoGainWalker<float, int> > bestSplitpointStep(classInfoGainWalker, 
                                                                                            featureExtractor.FeatureValuesBufferId,
                                                                                            featureOrdering);
    nodeSteps.push_back(&bestSplitpointStep); 
    Pipeline nodeStepsPipeline(nodeSteps);

    //Split Selector
    std::vector<SplitSelectorBuffers> splitBuffers;
    splitBuffers.push_back(SplitSelectorBuffers(bestSplitpointStep.ImpurityBufferId,
                                                bestSplitpointStep.SplitpointBufferId,
                                                bestSplitpointStep.SplitpointCountsBufferId,
                                                bestSplitpointStep.ChildCountsBufferId,
                                                bestSplitpointStep.LeftYsBufferId,
                                                bestSplitpointStep.RightYsBufferId,
                                                featureParams.FloatParamsBufferId,
                                                featureParams.IntParamsBufferId,
                                                featureExtractor.FeatureValuesBufferId,
                                                featureOrdering,
                                                allSamplesStep.IndicesBufferId));
    ShouldSplitNoCriteria noCriteria;
    ClassEstimatorFinalizer<float> classFinalizer;
    SplitSelector<float, int> splitSelector(splitBuffers, &noCriteria, &classFinalizer);
    
    return DepthFirstTreeLearner<float, int>(&trySplitCriteria, &treeStepsPipeline, &nodeStepsPipeline, &splitSelector);
}
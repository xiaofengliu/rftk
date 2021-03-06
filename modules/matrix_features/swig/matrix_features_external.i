%module matrix_features
%{
    #define SWIG_FILE_WITH_INIT
    #include "AxisAlignedParamsStep.h"
    #include "LinearMatrixFeature.h"
%}

%include <exception.i>
%import(module="rftk.asserts") "asserts.i"
%import(module="rftk.buffers") "buffers.i"
%import(module="rftk.pipeline") "pipeline_external.i"

%include "AxisAlignedParamsStep.h"
%include "LinearMatrixFeature.h"


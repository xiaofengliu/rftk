#pragma once

#include <boost/random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>

#include <BufferCollection.h>
#include <BufferCollectionStack.h>

// ----------------------------------------------------------------------------
//
// PipelineStepI reads inputs from readCollection does some processing and
// writes the results to writeCollection
//
// ----------------------------------------------------------------------------

class PipelineStepI
{
public:
    virtual ~PipelineStepI() {}

    virtual PipelineStepI* Clone() const = 0;

    virtual void ProcessStep(   const BufferCollectionStack& readCollection,
                                BufferCollection& writeCollection,
                                boost::mt19937& gen) const = 0;
};

enum SetRule
{
    WHEN_NEW,
    EVERY_PROCESS
};

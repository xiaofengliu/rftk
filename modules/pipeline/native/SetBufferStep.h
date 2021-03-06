#pragma once

#include "VectorBuffer.h"
#include "MatrixBuffer.h"
#include "BufferCollection.h"
#include "PipelineStepI.h"
#include "UniqueBufferId.h"

// ----------------------------------------------------------------------------
//
// SetBufferStep writes a buffer to OutputBufferId so it can be used by later
// steps.  It can either written everytime the pipeline step is run or
// only when OutputBufferId has not already been set.
//
// ----------------------------------------------------------------------------
template <class BufType>
class SetBufferStep: public PipelineStepI
{
public:
    SetBufferStep(const BufType& buffer, SetRule setRule );
    virtual ~SetBufferStep();

    virtual PipelineStepI* Clone() const;

    virtual void ProcessStep(   const BufferCollectionStack& readCollection,
                                BufferCollection& writeCollection,
                                boost::mt19937& gen) const;

    // Read only output buffer
    const BufferId OutputBufferId;
private:
    BufType mBuffer;
    SetRule mSetRule;
};


template <class BufType>
SetBufferStep<BufType>::SetBufferStep(const BufType& buffer, SetRule setRule)
: OutputBufferId(GetBufferId("SetBufferStep"))
, mBuffer(buffer)
, mSetRule(setRule)
{}

template <class BufType>
SetBufferStep<BufType>::~SetBufferStep()
{}

template <class BufType>
PipelineStepI* SetBufferStep<BufType>::Clone() const
{
    SetBufferStep* clone = new SetBufferStep<BufType>(*this);
    return clone;
}

template <class BufType>
void SetBufferStep<BufType>::ProcessStep(const BufferCollectionStack& readCollection,
                                        BufferCollection& writeCollection,
                                        boost::mt19937& gen) const
{
    UNUSED_PARAM(readCollection);
    UNUSED_PARAM(gen);

    if(!writeCollection.HasBuffer<BufType>(OutputBufferId) || mSetRule == EVERY_PROCESS)
    {
        writeCollection.AddBuffer(OutputBufferId, mBuffer);
    }
}

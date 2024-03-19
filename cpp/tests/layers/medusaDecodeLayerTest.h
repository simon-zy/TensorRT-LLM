/*
 * Copyright (c) 2022-2024, NVIDIA CORPORATION.  All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <gtest/gtest.h>

#include <memory>

#include "tensorrt_llm/layers/medusaDecodingLayer.h"
#include "tensorrt_llm/runtime/bufferManager.h"
#include "tensorrt_llm/runtime/cudaStream.h"

#include "tensorrt_llm/runtime/bufferManager.h"
#include "tensorrt_llm/runtime/cudaStream.h"
#include "tensorrt_llm/runtime/runtimeKernels.h"
#include "tensorrt_llm/runtime/tllmLogger.h"

#include "tensorrt_llm/common/cudaAllocator.h"
#include "tensorrt_llm/common/tensorConversion.h"
#include "tensorrt_llm/common/tllmException.h"

namespace tensorrt_llm::tests::layers
{

struct SamplingParams
{
    tensorrt_llm::runtime::SizeType batchSize;
    std::vector<tensorrt_llm::runtime::SizeType> runtimeTopK;
    std::vector<std::vector<tensorrt_llm::runtime::SizeType>> runtimeHeadsTopK;
    std::vector<std::vector<tensorrt_llm::runtime::TokenIdType>> draftIds;
    std::vector<std::vector<tensorrt_llm::runtime::SizeType>> paths;
    std::vector<tensorrt_llm::runtime::SizeType> tokensPerStep;
    std::optional<tensorrt_llm::runtime::TokenIdType> endId;
};

template <typename T>
class MedusaDecodingLayerTest : public testing::Test
{
private:
    void SetUp() override;

    using TensorPtr = tensorrt_llm::runtime::ITensor::SharedPtr;
    using BufferPtr = tensorrt_llm::runtime::IBuffer::SharedPtr;
    using SizeType = tensorrt_llm::runtime::SizeType;
    using TokenIdType = tensorrt_llm::runtime::TokenIdType;

    SizeType mBatchSize{6};
    SizeType mMaxBatchSize{2 * mBatchSize};
    SizeType const mVocabSize{9};
    SizeType const mVocabSizePadded{mVocabSize};
    SizeType const mMaxTokensPerStep{12};
    SizeType const mMaxNumHeads{4};

    SizeType const mMaxSeqLen{mMaxTokensPerStep};
    TokenIdType mEndId{mVocabSize};

    bool mUseLogitsVec{false};

    TensorPtr mTargetLogitsDevice;
    TensorPtr mMedusaLogitsDevice;

    TensorPtr mFinishedDevice;
    TensorPtr mSeqLengthsDevice;
    TensorPtr mAcceptedLengths;
    TensorPtr mOutputIdsDevice;
    TensorPtr mNextDraftTokensDevice;

    TensorPtr mPathsDevice;
    TensorPtr mEndIdsDevice;
    TensorPtr mBatchSlots;

    std::vector<tensorrt_llm::common::Tensor> mLogitsVec;

    std::shared_ptr<tensorrt_llm::runtime::CudaStream> mStream;
    std::shared_ptr<tensorrt_llm::runtime::BufferManager> mBufferManager;
    std::shared_ptr<tensorrt_llm::common::CudaAllocator> mAllocator;
    std::shared_ptr<tensorrt_llm::layers::MedusaDecodingLayer<T>> mMedusaDecodingLayer;

private:
    void allocateBuffers();

    void setup(SamplingParams& params);

    typename tensorrt_llm::layers::MedusaDecodingLayer<T>::MedusaForwardParams createInputTensors();

    tensorrt_llm::layers::DecodingOutputParams createOutputTensors();

    void checkResult(std::vector<std::vector<std::set<TokenIdType>>> const& expectedOutTokens,
        std::vector<std::vector<TokenIdType>> const& expectedDraftTokens, std::vector<bool> const& finished);

public:
    void runTest(std::vector<std::vector<std::set<TokenIdType>>> const& expectedOutTokens,
        std::vector<std::vector<TokenIdType>> const& expectedDraftTokens, std::vector<bool> const& finished,
        SamplingParams& params);
};

typedef testing::Types<float, half> FloatAndHalfTypes;

} // namespace tensorrt_llm::tests::layers
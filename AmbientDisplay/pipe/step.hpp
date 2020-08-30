//
//  step.hpp
//  AmbientDisplay
//
//  Created by Simon Schneider on 29.08.20.
//  Copyright © 2020 Simon Schneider. All rights reserved.
//

#pragma once
#include <memory>

class PipelineStep {
public:
    virtual ~PipelineStep() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
};

using PipelineStepPtr = std::shared_ptr<PipelineStep>;

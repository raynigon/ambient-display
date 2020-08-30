//
//  pipeline.hpp
//  AmbientDisplay
//
//  Created by Simon Schneider on 29.08.20.
//  Copyright © 2020 Simon Schneider. All rights reserved.
//

#pragma once
#include "step.hpp"

class Pipeline{
public:
    Pipeline(PipelineStepPtr input, PipelineStepPtr output);
    void start();
    void stop();
private:
    PipelineStepPtr input;
    PipelineStepPtr output;
};

using PipelinePtr = std::shared_ptr<Pipeline>;

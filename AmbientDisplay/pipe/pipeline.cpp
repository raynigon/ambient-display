//
//  pipeline.cpp
//  AmbientDisplay
//
//  Created by Simon Schneider on 29.08.20.
//  Copyright © 2020 Simon Schneider. All rights reserved.
//

#include "pipeline.hpp"

Pipeline::Pipeline(PipelineStepPtr input, PipelineStepPtr output) :input(input), output(output) {
    
}

void Pipeline::start() {
    input->start();
    output->start();
}

void Pipeline::stop() {
    input->stop();
    output->stop();
}

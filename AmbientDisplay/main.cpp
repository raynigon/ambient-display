//
//  main.cpp
//  AmbientDisplay
//
//  Created by Simon Schneider on 29.08.20.
//  Copyright © 2020 Simon Schneider. All rights reserved.
//

#include <cstdio>
#include <iostream>
#include "pipe/pipeline.hpp"
#include "pipe/step.hpp"
#include "entity/queue.hpp"
#include "record/recorder.hpp"
#include "transmit/transmitter.hpp"

PipelinePtr createPipeline(){
    RecordQueuePtr queue = std::make_shared<RecordQueue>();
    auto recorder = std::dynamic_pointer_cast<PipelineStep>(std::make_shared<Recorder>(queue));
    auto transmitter = std::dynamic_pointer_cast<PipelineStep>(std::make_shared<Transmitter>(queue));
    return std::make_shared<Pipeline>(recorder, transmitter);
}

int main(int argc, const char * argv[]) {
    auto pipeline = createPipeline();
    pipeline->start();
    std::cout << "Press ENTER twice to exit" << std::endl;
    std::getchar();
    std::getchar();
    pipeline->stop();
    std::cout << "Goodbye!" << std::endl;
    return 0;
}

//
//  recorder.hpp
//  AmbientDisplay
//
//  Created by Simon Schneider on 29.08.20.
//  Copyright © 2020 Simon Schneider. All rights reserved.
//

#pragma once
#include <chrono>
#include <QuartzCore/QuartzCore.h>
#include "../pipe/step.hpp"
#include "../entity/queue.hpp"

class Recorder : public PipelineStep{
    
public:
    Recorder(RecordQueuePtr queue);
    virtual void start() override;
    virtual void stop() override;
protected:
    void copy(RecordPtr record, unsigned int* buffer, size_t size, int screen_width, int screen_height);
private:
    RecordQueuePtr                          queue;
    dispatch_queue_t                        dispatch_queue;
    int                                     screen_width;
    int                                     screen_height;
    CGDisplayStreamRef                      stream_ref;
    std::chrono::steady_clock::time_point   last_update;
};

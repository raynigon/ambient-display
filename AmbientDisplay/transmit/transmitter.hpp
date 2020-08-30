//
//  transmitter.hpp
//  AmbientDisplay
//
//  Created by Simon Schneider on 29.08.20.
//  Copyright © 2020 Simon Schneider. All rights reserved.
//

#pragma once
#include <thread>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>

#include "../pipe/step.hpp"
#include "../entity/queue.hpp"

using uchar = unsigned char;
struct Pixel{
    uchar red = 0;
    uchar green = 0;
    uchar blue = 0;
    uchar white = 0; // TODO https://stackoverflow.com/questions/40312216/converting-rgb-to-rgbw
};

class Transmitter : public PipelineStep{
    
public:
    Transmitter(RecordQueuePtr queue);
    virtual void start() override;
    virtual void stop() override;
protected:
    void run();
    std::vector<Pixel> handle_record(RecordPtr record);
private:
    RecordQueuePtr  queue;
    bool            active = false;
    std::thread     thread;
    int             socketfd;
};

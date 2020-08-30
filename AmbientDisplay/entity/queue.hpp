//
//  queue.hpp
//  AmbientDisplay
//
//  Created by Simon Schneider on 29.08.20.
//  Copyright © 2020 Simon Schneider. All rights reserved.
//

#pragma once
#include <queue>
#include "record.hpp"

class RecordQueue{
    
public:
    bool empty();
    void push(RecordPtr record);
    RecordPtr pop();
private:
    std::queue<RecordPtr> std_queue;
};

using RecordQueuePtr = std::shared_ptr<RecordQueue>;

//
//  queue.cpp
//  AmbientDisplay
//
//  Created by Simon Schneider on 29.08.20.
//  Copyright © 2020 Simon Schneider. All rights reserved.
//

#include "queue.hpp"


void RecordQueue::push(RecordPtr record) { 
    std_queue.push(record);
}

RecordPtr RecordQueue::pop() {
    if (std_queue.empty()) return nullptr;
    auto value = std_queue.front();
    std_queue.pop();
    return value;
}

bool RecordQueue::empty() { 
    return std_queue.empty();
}




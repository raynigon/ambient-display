//
//  record.hpp
//  AmbientDisplay
//
//  Created by Simon Schneider on 29.08.20.
//  Copyright © 2020 Simon Schneider. All rights reserved.
//

#pragma once
#include <memory>

class Record{
public:
    void store_pixel(int x, int y, unsigned int value);
    unsigned int pixel(int x, int y);
    int width(){return 100;};
    int height(){return 100;};
private:
    unsigned int content[100][100];
};

using RecordPtr = std::shared_ptr<Record>;

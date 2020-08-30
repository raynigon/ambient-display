//
//  record.cpp
//  AmbientDisplay
//
//  Created by Simon Schneider on 29.08.20.
//  Copyright © 2020 Simon Schneider. All rights reserved.
//

#include "record.hpp"


void Record::store_pixel(int x, int y, unsigned int value) {
    if (x<0 || x>=width() || y<0 || y>=height()) return;
    content[x][y] = value;
}

unsigned int Record::pixel(int x, int y) {
    if (x<0 || x>=width() || y<0 || y>=height()) return -1;
    return content[x][y];
}


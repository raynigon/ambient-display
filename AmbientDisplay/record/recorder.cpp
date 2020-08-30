//
//  recorder.cpp
//  AmbientDisplay
//
//  Created by Simon Schneider on 29.08.20.
//  Copyright © 2020 Simon Schneider. All rights reserved.
//

#include "recorder.hpp"

Recorder::Recorder(RecordQueuePtr queue) :
queue(queue),
dispatch_queue(dispatch_queue_create("com.raynigon.conference-web-cam.screencapture", DISPATCH_QUEUE_SERIAL)),
screen_width(0),
screen_height(0),
stream_ref(nullptr)
{}

void Recorder::start() { 
    CGDirectDisplayID mainDisplayId = CGMainDisplayID();
    auto bounds = CGDisplayBounds(mainDisplayId);
    screen_width = bounds.size.width;
    screen_height = bounds.size.height;
    last_update = std::chrono::steady_clock::now();
    
    CGDisplayStreamFrameAvailableHandler handler = ^void(CGDisplayStreamFrameStatus status,
                                                         uint64_t timestamp,
                                                         IOSurfaceRef frameSurface,
                                                         CGDisplayStreamUpdateRef updateRef) {
        /*
         IOPhysicalAddress           baseAddress;
         UInt32                      activeWidth;
         UInt32                      activeHeight;
         IOByteCount                 bytesPerRow;
         IOByteCount                 bytesPerPlane;
         UInt32                      bitsPerPixel;
         UInt32                      pixelType;
         UInt32                      flags;
         UInt32                      reserved[ 4 ];
         */
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update).count();
        if (duration < 10) return;
        IOSurfaceLock(frameSurface, kIOSurfaceLockReadOnly, nullptr);
        int width = int(IOSurfaceGetWidth(frameSurface));
        int height = int(IOSurfaceGetHeight(frameSurface));
        auto baseAddress = reinterpret_cast<unsigned int*>(IOSurfaceGetBaseAddress(frameSurface));
        auto bufferSize = IOSurfaceGetAllocSize(frameSurface);
        auto record = std::make_shared<Record>();
        copy(record, baseAddress, bufferSize, width, height);
        queue->push(record);
        last_update = std::chrono::steady_clock::now();
        IOSurfaceUnlock(frameSurface, kIOSurfaceLockReadOnly, nullptr);
    };
    
    int32_t pixel_format = reinterpret_cast<int32_t>('BGRA');
    stream_ref = CGDisplayStreamCreateWithDispatchQueue(mainDisplayId, screen_width, screen_height, pixel_format, nullptr, dispatch_queue, handler);
    CGError error = CGDisplayStreamStart(stream_ref);
    if (error == kCGErrorSuccess) return;
    // TODO handle error
}

void Recorder::stop() {
    
}

void Recorder::copy(RecordPtr record, unsigned int *buffer, size_t size, int screen_width, int screen_height) {
    int record_width = record->width();
    int record_height = record->height();
    for (int x=0;x<record_width;x++){
        for (int y=0;y<record_height;y++){
            double rel_x = double(x)/double(record_width);
            double rel_y = double(y)/double(record_height);
            int offset = int(screen_width*rel_x)+int(screen_height*rel_y)*screen_width;
            unsigned int value = buffer[offset];
            record->store_pixel(x,y,value);
        }
    }
}




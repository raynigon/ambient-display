//
//  transmitter.cpp
//  AmbientDisplay
//
//  Created by Simon Schneider on 29.08.20.
//  Copyright © 2020 Simon Schneider. All rights reserved.
//

#include "transmitter.hpp"
#include <array>
#include <iostream>
#include <chrono>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>


struct ArtNetDmxPacket{
    unsigned char header[8] = "Art-Net";
    unsigned char opcode[2] = {0x00, 0x50};
    unsigned char version[2] = {0x00, 0x0E};
    unsigned char sequence = 0;
    unsigned char physical = 0;
    unsigned short universe = 0;
    unsigned char length[2] = {0x02, 0x00};
    unsigned char data[512];
};

Pixel toPixel(unsigned int value){
    auto pixel = Pixel();
    pixel.red = uchar((value >> 16) & 0xFF);
    pixel.green = uchar((value >> 8) & 0xFF);
    pixel.blue = uchar(value & 0xFF);
    return pixel;
}

std::array<int, 24> pixel_to_dmx = {
    98,     104,    110,    116,    122,    128,    134,    140,    146,    152,    158,    164, // Bar 2
    24,     30,     36,     42,     48,     54,     60,     66,     72,     78,     84,     90 // Bar 1
};

Transmitter::Transmitter(RecordQueuePtr queue) : queue(queue) {}

void Transmitter::start() {
    active = true;
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketfd < 0){
        throw new std::exception();
    }
    auto server = gethostbyname("192.168.178.31");
    sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(6454);
    auto result = connect(socketfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (result < 0){
        throw new std::exception();
    }
    thread = std::thread([=](){run();});
}

void Transmitter::stop() {
    active = false;
    thread.join();
}

void Transmitter::run() {
    using namespace std::chrono_literals;
    ArtNetDmxPacket packet;
    bzero(&(packet.data), 512);
    // set Intensity to full
    packet.data[22] = 255;
    packet.data[96] = 255;
    
    while(active){
        if (queue->empty()){
            std::this_thread::sleep_for(10ms);
        }else{
            auto record = queue->pop();
            auto pixels = handle_record(record);
            for (size_t i=0;i<pixels.size();i++){
                int offset = pixel_to_dmx[i];
                Pixel pixel = pixels[i];
                packet.data[offset] = pixel.red;
                packet.data[offset+1] = pixel.green;
                packet.data[offset+2] = pixel.blue;
                packet.data[offset+3] = pixel.white;
            }
        }
        packet.sequence++;
        auto result = send(socketfd, &packet, sizeof(ArtNetDmxPacket), 0);
        if (result<0){
            std::cout << "Error: " << errno << std::endl;
        } else {
            std::cout << "Success" << std::endl;
        }
    }
}

std::vector<Pixel> Transmitter::handle_record(RecordPtr record) {
    int pixel_count = 24;
    std::vector<Pixel> pixels;
    for (int i = 0;i<pixel_count;i++){
        int offset_x = double(record->width())*double(i)/double(pixel_count);
        auto pixel = toPixel(record->pixel(offset_x,30));
        pixels.push_back(pixel);
    }
    return pixels;
}





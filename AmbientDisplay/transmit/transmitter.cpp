//
//  transmitter.cpp
//  AmbientDisplay
//
//  Created by Simon Schneider on 29.08.20.
//  Copyright © 2020 Simon Schneider. All rights reserved.
//

#include "transmitter.hpp"
#include <array>
#include <cmath>
#include <iostream>
#include <algorithm>
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

Pixel enhanceWhite(Pixel input){
    auto Ri = input.red;
    auto Gi = input.green;
    auto Bi = input.blue;
    //Get the maximum between R, G, and B
    float tM = std::max(Ri, std::max(Gi, Bi));

    //If the maximum value is 0, immediately return pure black.
    if(tM == 0) return Pixel{};

    //This section serves to figure out what the color with 100% hue is
    float multiplier = 255.0f / tM;
    float hR = Ri * multiplier;
    float hG = Gi * multiplier;
    float hB = Bi * multiplier;

    //This calculates the Whiteness (not strictly speaking Luminance) of the color
    float M = std::max(hR, std::max(hG, hB));
    float m = std::min(hR, std::min(hG, hB));
    float Luminance = ((M + m) / 2.0f - 127.5f) * (255.0f/127.5f) / multiplier;

    //Calculate the output values
    uchar Wo = uchar(lround(Luminance));
    uchar Bo = uchar(lround(Bi - Luminance));
    uchar Ro = uchar(lround(Ri - Luminance));
    uchar Go = uchar(lround(Gi - Luminance));

    //Trim them so that they are all between 0 and 255
    if (Wo < 0) Wo = 0;
    if (Bo < 0) Bo = 0;
    if (Ro < 0) Ro = 0;
    if (Go < 0) Go = 0;
    if (Wo > 255) Wo = 255;
    if (Bo > 255) Bo = 255;
    if (Ro > 255) Ro = 255;
    if (Go > 255) Go = 255;
    return Pixel{Ro, Go, Bo, Wo};
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
        }
    }
}

std::vector<Pixel> Transmitter::handle_record(RecordPtr record) {
    int pixel_count = 24;
    std::vector<Pixel> pixels;
    for (int i = 0;i<pixel_count;i++){
        int offset_x = int(lround(double(record->width())*double(i)/double(pixel_count)));
        auto pixel = toPixel(record->pixel(offset_x,29));
        pixels.push_back(enhanceWhite(pixel));
    }
    return pixels;
}





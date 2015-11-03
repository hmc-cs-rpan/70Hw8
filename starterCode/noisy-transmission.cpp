/**
 * \file noisytransmit.cpp
 * \author Fill me in!
 *
 * \brief Defines the noisyTransmit() function for stepping through a 
 * chunkystring and randomly doubling, erasing, or leaving each character
 * unchanged.
 *
 * \details
 *
 * \remarks
 *
 */

#include <iostream>
#include <fstream>
#include <random>
#include "chunkystring.hpp"
#include "noisy-transmission.hpp"

NoisyTransmission::NoisyTransmission(float errorRate) : errorRate_(errorRate), dis_(0,1) {
    seed();
}

void NoisyTransmission::seed() {
    std::random_device rd;
    gen_.seed(rd());
}

float NoisyTransmission::getRandomFloat() {
    return dis_(gen_);
}

void NoisyTransmission::transmit(ChunkyString::ChunkyString& message) 
{
  //std::cerr << "Not implemented yet" << std::endl;
}

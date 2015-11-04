/**
 * \file noisytransmit.hpp
 * \author CS70 Provided Code
 *
 * \brief Defines the noisyTransmit() function for stepping through a 
 * chunkystring and randomly doubling, erasing, or leaving each character
 * unchanged.
 *
 */

#ifndef NOISYTRANSMISSION_HPP_INCLUDED
#define NOISYTRANSMISSION_HPP_INCLUDED 1


#include "chunkystring.hpp"
#include <random>

class NoisyTransmission {
public:
    NoisyTransmission(float errorRate);
    void transmit(ChunkyString& message);
    float getRandomFloat();
  
private:
    float errorRate_;
    std::uniform_real_distribution<> dis_;
    std::mt19937 gen_;

    void seed();
};

#endif

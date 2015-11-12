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

void NoisyTransmission::transmit(ChunkyString& message) 
{
	float prob = 0;
	for(ChunkyString::iterator i = message.begin(); i != message.end(); ++i)
	{
		prob = getRandomFloat();
		if(prob < errorRate_)
		{
			message.erase(i);
		}
		else if(prob > 1-errorRate_)
		{
			char toInsert = *i;
			message.insert(i, toInsert);
		}
	}
}

What are the data members of the NoisyTransmission class? Explain what their 
types are, and how they get their values.

data members:

1) errorRate_: errorRate_ is a float, which is a rational number that describes
the frequency of errors in transmitting a signal. THe user provides this value
in the constructor.

2) dis_: A member that can produce random floating-point values using the
uniform distribution. This takes in a value for seed and generates a random
number from that seed. dis_ is initialized with boundaries (0,1).

Cite: 
<http://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution>

3) gen_: gen_ is a Mersenne Twister random 32-bit number generator. gen_ gets
its value from the seed of a random_device.

Cite:
<http://www.cplusplus.com/reference/random/mt19937/>

The processOptions function in message-passer.cpp handles parsing the options 
passed on the command line to your program. How does it work? If we run 
messagepasser -n 0.2 -f myfile.txt, describe what will happen when 
processOptions is called.

processOptions allows us to input our specifications. -n is a flag for the 
noiseLevel, or errorRate and -f is a flag for the filename. When we run 
messagepasser -n 0.2 -f myfile.txt, we are running messagepasser with an 
errorRate of 0.2 on a file called myfile.txt. Thus, on whatever text we have in
myfile.txt, for each char, there will be a 0.2 chance that we erase, a 0.2 
chance that we double (insert) and a 0.6 chance nothing happens.

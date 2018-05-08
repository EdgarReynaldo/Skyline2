



#ifndef RNG_HPP
#define RNG_HPP





void Seed(unsigned int s = 0);

/// These should return all possible values (2^32 - 1), signed or unsigned
unsigned int URand();
int IRand();

/// I think these two range from -1.0 to 1.0
float FRand();
double DRand();

unsigned int Rand0toNM1(unsigned int n);
unsigned int Rand1toN(unsigned int n);

float Percent();
double DPercent();






#endif // RNG_HPP

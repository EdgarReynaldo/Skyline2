



#include "RNG.hpp"

#include "Eagle/Random.hpp"

MTRNG rng;


void Seed(unsigned int s) {
   rng.Seed(s);
}



/// These should return all possible values (2^32 - 1), signed or unsigned

unsigned int URand() {
   return rng.URand();
}



int IRand() {
   return rng.IRand();
}



/// I think these two range from -1.0 to 1.0

float FRand() {
   return rng.FRand();
}



double DRand() {
   return rng.DRand();
}



/// 0 to N minus 1 and 1 to N

unsigned int Rand0toNM1(unsigned int n) {
   return rng.Rand0toNM1(n);
}



unsigned int Rand1toN(unsigned int n) {
   return rng.Rand1toN(n);
}



float FRNGPercent() {
   return rng.DPercent();
}



double DRNGPercent() {
   return rng.DPercent();
}

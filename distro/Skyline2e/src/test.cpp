

#include "GL/gl.h"



#include "Eagle.hpp"

#include "Missiles.hpp"
#include "Config.hpp"
#include "Globals.hpp"

Config cg;
Rectangle re;

AI::AI(Rectangle launch_zone , int num_missiles , Config config) : mb(0) , zone(launch_zone) , nmissiles(num_missiles) , c(config) {}




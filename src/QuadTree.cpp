


/**
#include "QuadTree.hpp"


bool QTNode::Hit(double xpos , double ypos) {
   bool hit = !empty && x >= xpos && (x + w) < xpos && y >= ypos && (y + h) < ypos;
   if (hit) {
      if (quads) {
         int index = 0;
         bool subhit = false;
         if (xpos >= (x + w/2.0)) {
            index += 1;
         }
         if (ypos >= (y + h/2.0)) {
            index += 2;
         }
         if (!quads[index].Hit(xpos,ypos)) {
            hit = false;
         }
      }
   }
   return hit;
}

//*/



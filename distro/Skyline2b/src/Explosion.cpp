



#include "Explosion.hpp"


#include "GL/gl.h"

#include <cmath>


void DrawExplosion(float cx , float cy , float crad , EagleColor ic , EagleColor oc) {
   
      double deg = 0.0;
      glBegin(GL_TRIANGLE_FAN);
      glColor4d(ic.fr , ic.fg , ic.fb , ic.fa);
      glVertex2d(cx , cy);
      glColor4d(oc.fr , oc.fg , oc.fb , oc.fa);
      glVertex2d(cx + crad*cos(deg*M_PI/180.0f) , cy + crad*sin(deg*M_PI/180.0f));
      for (deg = 0.0 ; deg < 360.0 ; deg += 1.0) {
         glVertex2d(cx + crad*cos((deg+1)*M_PI/180.0f) , cy + crad*sin((deg+1)*M_PI/180.0f));
      }
      glEnd();
}


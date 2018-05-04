



#include "OpenGLDrawing.hpp"



void DrawJet(Pos2D p , double theta , EagleColor icol , EagleColor ocol) {
   const double JET_LENGTH = 25.0;
   
   theta -= M_PI/2.0;
   glBegin(GL_TRIANGLE_FAN);
   eglColor(icol);
   glVertex2d(p.X(),p.Y());
   eglColor(ocol);
   for (double d = 0.0 ; d < 180.0 ; ++d) {
      double t = (d+1)*M_PI/180.0;
      double s = sin(t);
      double rad = JET_LENGTH*(s*s*s*s*s);
      Pos2D p2 = Vector(p , rad , theta + t);
      glVertex2d(p2.X() , p2.Y());
   }
   glEnd();
}





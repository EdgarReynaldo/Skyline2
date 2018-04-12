



#include "Shields.hpp"


#include "allegro5/allegro.h"
#include "allegro5/allegro_color.h"


#include "GL/gl.h"

#include "Eagle/Logging.hpp"
#include "Eagle/Exception.hpp"



void Shield::ResetShieldRadius() {
   double pct = hp/maxhp;
   orad = irad + pct*(maxrad - irad);
   
   double hue = pct*360.0;
   float r,g,b;
   al_color_hsl_to_rgb(hue , 1.0 , 0.5 + 0.5*pct , &r , &g , &b);
   icol.SetFloatColor(r,g,b,1.0f);
///   ocol.SetFloatColor(r,g,b,0.5f , false);/// OpenGL uses PM alpha
   ocol.SetFloatColor(r,g,b,0.5f);
   EAGLE_DEBUG(
      EagleLog() << "Shield radius reset : ICOL = " << icol << " OCOL = " << ocol << std::endl;
   );
}



Shield::Shield() :
      cx(0.0),
      cy(0.0),
      irad(0.0),
      orad(0.0),
      maxrad(0.0),
      hp(0.0),
      maxhp(0.0),
      icol(),
      ocol()
{}



void Shield::Setup(Pos2D left , Pos2D right , double height , double thickness) {
   Pos2D c((left.X() + right.X()) / 2.0 , (left.Y() + right.Y())/2.0 + height);
   double inner_radius = (left - c).Length();
   double outer_radius = inner_radius + thickness;

   /// Coordinates are upside down, postive theta goes clockwise, so stop should be on the right
   thetastart = c.AngleToPoint(left);
   thetastop = c.AngleToPoint(right);
   if (thetastart < 0.0) {thetastart += 2.0*M_PI;}
   if (thetastop < 0.0) {thetastop += 2.0*M_PI;}
   double thetadiff = thetastop - thetastart;
   if (thetadiff < -M_PI) {thetadiff += 2.0*M_PI;}
   if (thetadiff >= M_PI) {thetadiff -= 2.0*M_PI;}

   cx = c.X();
   cy = c.Y();
   irad = inner_radius;
   orad = maxrad = outer_radius;
   double ao = M_PI*orad*orad;
   double ai = M_PI*irad*irad;
   double pct = fabs(thetadiff)/(2.0*M_PI);
   hp = maxhp = pct*(ao - ai);
   ResetShieldRadius();
   
   
}



bool Shield::Hit(double xpos , double ypos) {
   if (hp < 0.0) {return false;}
   Pos2D p1(cx,cy);
   Pos2D p2(xpos,ypos);
   double theta = p1.AngleToPoint(p2);
   if (theta < 0.0) {theta += 2.0*M_PI;}
   if (theta < thetastart || theta > thetastop) {
      return false;
   }
   double d = (p2-p1).Length();
   return (d >= irad && d <= orad);
}



void Shield::Damage(double area) {
   hp -= area;
   ResetShieldRadius();
}



void Shield::Reset() {
   hp = maxhp;
   ResetShieldRadius();
}



void Shield::Draw() {
   
   if (hp <= 0.0) {
      return;
   }
   const int NSEGMENTS = 60;
   
   double thetadiff = thetastop - thetastart;
   if (thetadiff < 0.0) {thetadiff += 2.0*M_PI;}
   
   double dtheta = thetadiff/NSEGMENTS;
   
   glEnable(GL_COLOR);
   glEnable(GL_BLEND);
   glBegin(GL_TRIANGLE_STRIP);
   glColor4d(1.0 , 1.0 , 1.0 , 1.0);
   bool even = true;
   for (int i = 0 ; i < NSEGMENTS + 1 ; ++i) {
      double theta = dtheta*i;
      Pos2D outer = Vector(Pos2D(cx,cy) , orad , thetastart + theta);
      Pos2D inner = Vector(Pos2D(cx,cy) , irad , thetastart + theta);
      glColor4f(ocol.fR() , ocol.fG() , ocol.fB() , ocol.fA());
      glVertex2d(outer.X() , outer.Y());
      glColor4f(icol.fR() , icol.fG() , icol.fB() , icol.fA());
      glVertex2d(inner.X() , inner.Y());
/**
      if (even) {
///         glColor4f(icol.fR() , icol.fG() , icol.fB() , icol.fA());
         glVertex2d(inner.X() , inner.Y());
///         glColor4f(ocol.fR() , ocol.fG() , ocol.fB() , ocol.fA());
         glVertex2d(outer.X() , outer.Y());
      }
      else {
///         glColor4f(ocol.fR() , ocol.fG() , ocol.fB() , ocol.fA());
         glVertex2d(outer.X() , outer.Y());
///         glColor4f(icol.fR() , icol.fG() , icol.fB() , icol.fA());
         glVertex2d(inner.X() , inner.Y());
      }
      even = !even;
*/
   }
   glEnd();
}






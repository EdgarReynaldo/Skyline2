



#include "Lasers.hpp"
#include "Globals.hpp"

#include "Eagle/InputHandler.hpp"
#include "Eagle/StringWork.hpp"

#include <string>

#include "GL/gl.h"



double LASER_BEAM_WIDTH = 25.0;
double LASER_BEAM_DURATION = 0.5;



Laser::Laser(float srcx , float srcy , float destx , float desty , float beam_width , float time_to_live) :
      s(srcx , srcy),
      d(destx , desty),
      ttlmax(time_to_live),
      ttl(time_to_live),
      maxw(beam_width),
      w(0.0)
{}



void Laser::DrawLaser(EagleColor ic , EagleColor oc) {
   
   double theta = s.AngleToPoint(d.X() , d.Y());
   double rtheta = theta + M_PI/2.0f;
   double ltheta = theta - M_PI/2.0f;
   
   Pos2F vdl = VectorF(d , w/2.0f , ltheta);
   Pos2F vdr = VectorF(d , w/2.0f , rtheta);
   Pos2F vsl = VectorF(s , w/2.0f , ltheta);
   Pos2F vsr = VectorF(s , w/2.0f , rtheta);
   
   /// 6 points make up a gradient line
   Pos2F p[6] = {
      s , vsl , vdl , d , vdr , vsr
   };
   
   float a = 5.0/FPS;
   
   glBegin(GL_TRIANGLE_FAN);
///   glColor4f(ic.fR() , ic.fG() , ic.fB() , 3.0/FPS);///ic.fA());
   
   glColor4f(a*ic.fR() , a*ic.fG() , a*ic.fB() , a);///ic.fA());
   glVertex2f(p[0].X() , p[0].Y());

   glColor4f(oc.fR() , oc.fG() , oc.fB() , oc.fA());///oc.fA());
   glVertex2f(p[1].X() , p[1].Y());
   glVertex2f(p[2].X() , p[2].Y());

   glColor4f(a*ic.fR() , a*ic.fG() , a*ic.fB() , a);///ic.fA());
   glVertex2f(p[3].X() , p[3].Y());

   glColor4f(oc.fR() , oc.fG() , oc.fB() , oc.fA());///oc.fA());
   glVertex2f(p[4].X() , p[4].Y());
   glVertex2f(p[5].X() , p[5].Y());

   glEnd();
}



LASER_STATE Laser::Update(double dt) {
   ttl -= (float)dt;
   if (ttl < 0.0f) {return LASER_TOAST;}
   double pct = 2.0*(ttlmax - ttl)/ttlmax;
   if (pct > 1.0) {
      pct = 2.0 - pct;
   }
   w = maxw*pct;
   return LASER_FIRING;
}



void Laser::Draw() {
   DrawLaser(innercolor , outercolor);
}



void Laser::SetColors(EagleColor inner , EagleColor outer) {
   innercolor = inner;
   outercolor = outer;
}



bool Laser::Hit(int x , int y) {
   
   GLE2D l1(Pos2D(s.X() , s.Y()) , Pos2D(d.X() , d.Y()));
   return (DistanceFromLine(Pos2D(x + 0.5 , y + 0.5) , l1) < w/2.0);/// TODO : DEBUG : This doesn't work

/*      
   Pos2F p = Pos2F(x + 0.5f - s.X() , y + 0.5f - s.Y());
   if (p.Length() < (d-s).Length()) {
      /// Closer than radius of line segment
      /// Check perpendicular distance to line
      GLE2D l1(Pos2D(s.X() , s.Y()) , Pos2D(d.X() , d.Y()));
      return (DistanceFromLine(Pos2D(x + 0.5 , y + 0.5) , l1) < w/2.0);
   }
   return false;
*/
}



/// -------------------------     Laser Launcher    --------------------------------




LaserLauncher::LaserLauncher(Pos2F position , double range) :
      pos(position),
      aim(position),
      aim_theta(0.0),
      aim_length(range),
      lc1(),
      lc2()
{}



LaserLauncher::~LaserLauncher() {
   (void)0;
}



void LaserLauncher::SetColors(EagleColor laser_color , EagleColor fade_color) {
   lc1 = laser_color;
   lc2 = fade_color;
}



void LaserLauncher::AimAt(int aimx , int aimy) {
   aim_theta = atan2(aimy - pos.Y() , aimx - pos.X());
   aim.SetPos(pos.X() + aim_length*cos(aim_theta) , pos.Y() + aim_length*sin(aim_theta));
}



void LaserLauncher::DrawLaserSight() {
   glBegin(GL_LINES);
   glColor4f(lc1.fR() , lc1.fG() , lc1.fB() , 0.5f);
   glVertex2f(pos.X() , pos.Y());
   glColor4f(lc1.fR() , lc1.fG() , lc1.fB() , 0.0f);
   glVertex2f(aim.X() , aim.Y());
   glEnd();
}



Laser* LaserLauncher::Fire() {
   Laser* l = new Laser(pos.X() , pos.Y() , aim.X() , aim.Y() , LASER_BEAM_WIDTH , LASER_BEAM_DURATION);
   l->SetColors(lc1 , lc2);
   return l;
}






/// --------------------      Laser Blast      -------------------------



void LaserBlast::Free() {
   if (beams[0]) {delete beams[0];}
   if (beams[1]) {delete beams[1];}
   if (beams[2]) {delete beams[2];}
}



void LaserBlast::Draw() {
   beams[0]->Draw();
   beams[1]->Draw();
   beams[2]->Draw();
}



LASER_STATE LaserBlast::Update(double dt) {
   
   if (lstate == LASER_TOAST) {return lstate;}
   
   LASER_STATE b0 = beams[0]->Update(dt);
   LASER_STATE b1 = beams[1]->Update(dt);
   LASER_STATE b2 = beams[2]->Update(dt);
   
   if (b0 == LASER_TOAST &&
       b1 == LASER_TOAST &&
       b2 == LASER_TOAST)
   {
      lstate = LASER_TOAST;
   }
   return lstate;
}



bool LaserBlast::Hit(int x , int y) {
   bool h1 = beams[0]->Hit(x,y);
   bool h2 = beams[1]->Hit(x,y);
   bool h3 = beams[2]->Hit(x,y);
   bool h = false;
   if (lcstr.compare("RGB") == 0) {
      h = h1 && h2 && h3;
   }
   else if (lcstr.compare("CMY") == 0) {
      h = (h1 && h2) || (h2 && h3) || (h1 && h3);
   }
   else if (lcstr.compare("WWW") == 0) {
      h = h1 || h2 || h3;
   }
   return h;
}



/// --------------------      Laser Battery      -------------------------




LaserBattery::LaserBattery() :
      aim(0.0f,0.0f),
      lasers(),
      lblasts(),
      lbstr(""),
      lcstr("")
{
   
}



LaserBattery::~LaserBattery() {
   Free();
}


void LaserBattery::Free() {
   Reset();
   for (int i = 0 ; i < 3 ; ++i) {
      if (lasers[i]) {delete lasers[i];lasers[i] = 0;}
   }
}



void LaserBattery::Setup(const Config& c) {
   Free();
   double range = sqrt(sw*sw + sh*sh);
   lasers[0] = new LaserLauncher(Pos2F(0 , sh) , range);
   lasers[1] = new LaserLauncher(Pos2F(sw/2 , sh) , range);
   lasers[2] = new LaserLauncher(Pos2F(sw , sh) , range);
   
   EagleColor red(1.0f , 0.0f , 0.0f , 1.0f);
   EagleColor green(0.0f , 1.0f , 0.0f , 1.0f);
   EagleColor blue(0.0f , 0.0f , 1.0f , 1.0f);
   EagleColor cyan(0.0f , 1.0f , 1.0f , 1.0f);
   EagleColor magenta(1.0f , 0.0f , 1.0f , 1.0f);
   EagleColor yellow(1.0f , 1.0f , 0.0f , 1.0f);
   EagleColor white(1.0f , 1.0f , 1.0f , 1.0f);
   EagleColor clear(1.0f , 1.0f , 1.0f , 0.0f);

   bool blend = false;

   lbstr = c.laser_blend_mode;
   if (lbstr.compare("SOLID") == 0) {
      blend = false;
   }
   else if (lbstr.compare("BLEND") == 0) {
      blend = true;
   }
   else {
      throw EagleException(StringPrintF("Laser blend mode not supported. Invalid value is '%s'." , lbstr.c_str()));
   }
   
   lcstr = c.laser_color;
   if (lcstr.compare("RGB") == 0) {
      lasers[0]->SetColors(red   , blend?clear:red);
      lasers[1]->SetColors(green , blend?clear:green);
      lasers[2]->SetColors(blue  , blend?clear:blue);
   }
   else if (lcstr.compare("CMY") == 0) {
      lasers[0]->SetColors(cyan    , blend?clear:cyan);
      lasers[1]->SetColors(magenta , blend?clear:magenta);
      lasers[2]->SetColors(yellow  , blend?clear:yellow);
   }
   else if (lcstr.compare("WWW") == 0) {
      lasers[0]->SetColors(white , blend?clear:white);
      lasers[1]->SetColors(white , blend?clear:white);
      lasers[2]->SetColors(white , blend?clear:white);
   }
   else {
      throw EagleException(StringPrintF("Failed to read valid value from laser color string. str is '%s'" , lcstr.c_str()));
   }
   
   LASER_BEAM_WIDTH = c.laser_width;
   LASER_BEAM_DURATION = c.laser_duration;
   
}



void LaserBattery::Reset() {
   for (unsigned int i = 0 ; i < lblasts.size() ; ++i) {
      delete lblasts[i];
   }
   lblasts.clear();
}



void LaserBattery::AimAt(int aimx , int aimy) {
   aim = Pos2D(aimx + 0.5 , aimy + 0.5);
   
   lasers[0]->AimAt(aim.X() , aim.Y());
   lasers[1]->AimAt(aim.X() , aim.Y());
   lasers[2]->AimAt(aim.X() , aim.Y());
   
}



void LaserBattery::Draw() {
   std::vector<LaserBlast*>::iterator lit = lblasts.begin();
   while (lit != lblasts.end()) {
      (*lit)->Draw();
      ++lit;
   }
   lasers[0]->DrawLaserSight();
   lasers[1]->DrawLaserSight();
   lasers[2]->DrawLaserSight();
}



void LaserBattery::Fire() {
   if (Ready()) {
      LaserBlast* lb = new LaserBlast(lcstr);
      lb->beams[0] = lasers[0]->Fire();
      lb->beams[1] = lasers[1]->Fire();
      lb->beams[2] = lasers[2]->Fire();
      lblasts.push_back(lb);
   }
}



void LaserBattery::Update(double dt) {
   std::vector<LaserBlast*>::iterator it = lblasts.begin();
   while (it != lblasts.end()) {
      LASER_STATE s = (*it)->Update(dt);
      if (s == LASER_TOAST) {
         it = lblasts.erase(it);
      }
      else {
         ++it;
      }
   }
}



bool LaserBattery::Ready() {return true;}




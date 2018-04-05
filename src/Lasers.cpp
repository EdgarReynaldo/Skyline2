



#include "Lasers.hpp"
#include "Globals.hpp"

#include "Eagle/InputHandler.hpp"

#include "GL/gl.h"



double LASER_BEAM_WIDTH = 100.0;
double LASER_BEAM_DURATION = 2.0;



Laser::Laser(float srcx , float srcy , float destx , float desty , float time_to_live) :
      s(srcx , srcy),
      d(destx , desty),
      ttlmax(time_to_live),
      ttl(time_to_live),
      maxw(LASER_BEAM_WIDTH),
      w(LASER_BEAM_WIDTH)
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
   
   glBegin(GL_TRIANGLE_FAN);
   glColor4f(ic.fR() , ic.fG() , ic.fB() , 255.0/FPS);///ic.fA());
   glVertex2f(p[0].X() , p[0].Y());
   glColor4f(oc.fR() , oc.fG() , oc.fB() , oc.fA());
   glVertex2f(p[1].X() , p[1].Y());
   glVertex2f(p[2].X() , p[2].Y());
   glColor4f(ic.fR() , ic.fG() , ic.fB() , 255.0/FPS);///ic.fA());
   glVertex2f(p[3].X() , p[3].Y());
   glColor4f(oc.fR() , oc.fG() , oc.fB() , oc.fA());
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



bool Laser::Hit(int x , int y) {
   
   GLE2D l1(Pos2D(s.X() , s.Y()) , Pos2D(d.X() , d.Y()));
   return (DistanceFromLine(Pos2D(x + 0.5 , y + 0.5) , l1) < w/2.0);

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
      active_beams(),
      lc1(),
      lc2()
{}



LaserLauncher::~LaserLauncher() {
   Free();
}



void LaserLauncher::Free() {
   std::vector<Laser*>::iterator it = active_beams.begin();
   while (it != active_beams.end()) {
      delete *it;
      ++it;
   }
   active_beams.clear();
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
   glVertex2f(aim.X() , aim.Y());
   glEnd();
}



void LaserLauncher::DrawLasers() {
   std::vector<Laser*>::iterator it = active_beams.begin();
   while (it != active_beams.end()) {
      (*it)->DrawLaser(lc1 , lc2);
      ++it;
   }
}



void LaserLauncher::Draw() {
   DrawLasers();
   DrawLaserSight();
}



void LaserLauncher::Fire() {
   active_beams.push_back(new Laser(pos.X() , pos.Y() , aim.X() , aim.Y() , LASER_BEAM_DURATION));
}



void LaserLauncher::Update(double dt) {
   std::vector<Laser*>::iterator it = active_beams.begin();
   while (it != active_beams.end()) {
      LASER_STATE s = (*it)->Update(dt);
      if (s == LASER_TOAST) {
         it = active_beams.erase(it);
      }
      else {
         ++it;
      }
   }
}



/// --------------------      Laser Battery      -------------------------




LaserBattery::LaserBattery() :
      aim(0.0f,0.0f),
      lasers()
{
   Setup();
}



LaserBattery::~LaserBattery() {
   Free();
}


void LaserBattery::Free() {
   std::vector<LaserLauncher*>::iterator it = lasers.begin();
   while (it != lasers.end()) {
      delete *it;
      ++it;
   }
   lasers.clear();
}



void LaserBattery::Setup() {
   Free();
   double range = sqrt(ww*ww + wh*wh);
   lasers.push_back(new LaserLauncher(Pos2F(0 , wh) , range));
   lasers.push_back(new LaserLauncher(Pos2F(ww/2 , wh) , range));
   lasers.push_back(new LaserLauncher(Pos2F(ww , wh) , range));
   lasers[0]->SetColors(EagleColor(255,0,0,255) , EagleColor(255,255,255,0));
   lasers[1]->SetColors(EagleColor(0,255,0,255) , EagleColor(255,255,255,0));
   lasers[2]->SetColors(EagleColor(0,0,255,255) , EagleColor(255,255,255,0));
}



void LaserBattery::AimAt(int aimx , int aimy) {
   aim = Pos2F(aimx + 0.5f , aimy + 0.5f);
   std::vector<LaserLauncher*>::iterator it = lasers.begin();
   while (it != lasers.end()) {
      (*it)->AimAt(aimx , aimy);
      ++it;
   }
}



void LaserBattery::Draw() {
   std::vector<LaserLauncher*>::iterator it = lasers.begin();
   while (it != lasers.end()) {
      (*it)->Draw();
      ++it;
   }
}



void LaserBattery::Fire() {
   std::vector<LaserLauncher*>::iterator it = lasers.begin();
   while (it != lasers.end()) {
      (*it)->Fire();
      ++it;
   }
}



void LaserBattery::Update(double dt) {
   std::vector<LaserLauncher*>::iterator it = lasers.begin();
   while (it != lasers.end()) {
      (*it)->Update(dt);
      ++it;
   }
}



void LaserBattery::CheckInputs() {
   if (input_mouse_held(RMB)) {
      Fire();
   }
}



std::vector<Laser*> LaserBattery::GetActiveLaserBeams() {
   std::vector<Laser*> beams;
   for (unsigned int i = 0 ; i < lasers.size() ; ++i) {
      std::vector<Laser*> bi = lasers[i]->Beams();
      beams.insert(beams.end() , bi.begin() , bi.end());
   }
   return beams;
}





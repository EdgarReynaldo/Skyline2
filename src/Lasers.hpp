



#ifndef Lasers_HPP
#define Lasers_HPP

#include "Eagle/Color.hpp"
#include "Eagle/Position.hpp"
#include "Eagle/LineEquations.hpp"


#include <list>
#include <vector>



#include "NewConfig.hpp"


enum LASER_STATE {
   LASER_FIRING = 0,
   LASER_TOAST = 1
};

extern double LASER_BEAM_WIDTH;
extern double LASER_BEAM_DURATION;

class Laser {
public :
   Pos2F s,d;/// source and destination xy of laser beam
   float ttlmax;/// time to live
   float ttl;/// time to live
   float maxw;
   float w;/// width

   Laser(float srcx , float srcy , float destx , float desty , float beam_width , float time_to_live);

   
   void DrawLaser(EagleColor ic , EagleColor oc);
   
   LASER_STATE Update(double dt);
   
   bool Hit(int x , int y);
};


class LaserLauncher {
   
protected :
   Pos2F pos;
   Pos2F aim;
   double aim_theta;
   double aim_length;
   
   std::vector<Laser*> active_beams;
   
   EagleColor lc1;
   EagleColor lc2;
   
public :
   LaserLauncher(Pos2F position , double range);
   
   ~LaserLauncher();
   void Free();
   
   void SetColors(EagleColor laser_color , EagleColor fade_color);
   
   void AimAt(int aimx , int aimy);

   void DrawLaserSight();
   void DrawLasers();
   void Draw();
   
   void Fire();
   
   void Update(double dt);
   
   std::vector<Laser*> Beams() {return active_beams;}

};

class LaserBattery {

protected :
   
   Pos2D aim;
   
   std::vector<LaserLauncher*> lasers;
   
public :
   
   LaserBattery();
   ~LaserBattery();
   
   void Free();
   void Setup(const Config& c);
   
   void Reset();
   
   void AimAt(int aimx , int aimy);

   void Draw();
   
   void Fire();
   
   void Update(double dt);
   
   void CheckInputs();
   
   std::vector<Laser*> GetActiveLaserBeams();
   
};



#endif // Lasers_HPP






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
   void DrawLaser(EagleColor ic , EagleColor oc);

public :
   Pos2F s,d;/// source and destination xy of laser beam
   float ttlmax;/// time to live
   float ttl;/// time to live
   float maxw;
   float w;/// width

   EagleColor innercolor;
   EagleColor outercolor;
   
   Laser(float srcx , float srcy , float destx , float desty , float beam_width , float time_to_live);

   void Draw();
   
   void SetColors(EagleColor inner , EagleColor outer);
   
   LASER_STATE Update(double dt);
   
   bool Hit(int x , int y);
};


class LaserLauncher {
   
protected :
   Pos2F pos;
   Pos2F aim;
   double aim_theta;
   double aim_length;
   
   EagleColor lc1;
   EagleColor lc2;
   
public :
   LaserLauncher(Pos2F position , double range);
   
   ~LaserLauncher();
   void Free();
   
   void SetColors(EagleColor laser_color , EagleColor fade_color);
   
   void AimAt(int aimx , int aimy);

   void DrawLaserSight();
   
   Laser* Fire();
   
};


class LaserBlast {

protected :
   
   friend class LaserBattery;
   
   Laser* beams[3];
   std::string lcstr;
   
   LASER_STATE lstate;
   
public :
   
   LaserBlast(std::string colors) : beams() , lcstr(colors) , lstate(LASER_FIRING) {}
   
   ~LaserBlast() {Free();}
   
   void Free();
   
   void Draw();
   
   LASER_STATE Update(double dt);
   
   bool Hit(int x , int y);
};



class LaserBattery {

protected :
   
   Pos2D aim;
   
   LaserLauncher* lasers[3];
   
   std::vector<LaserBlast*> lblasts;
   
   std::string lbstr;
   std::string lcstr;
   
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
   
   std::vector<LaserBlast*> LaserBlasts() {return lblasts;}
   
   bool Ready();
   
};



#endif // Lasers_HPP


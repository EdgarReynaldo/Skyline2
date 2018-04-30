

#ifndef Missiles_H
#define Missiles_H

#include "Eagle.hpp"
#include "Eagle/backends/Allegro5Backend.hpp"
#include "NewConfig.hpp"



#include <cmath>
#include <cstdlib>

#include <list>
using std::list;

#include <string>
using std::string;


#include "Eagle/Position.hpp"



enum MISSILE_STATE {
   NORMAL = 0,
   EXPLODING = 1,
   EXPLODED = 2,
   IMPLODING = 3,
   TOAST = 4
};


struct MISSILEDATA {
public :
   double mspeed;
   double etime;
   double eradius;
   
   MISSILEDATA() : mspeed(25.0) , etime(3.0) , eradius(10.0) {}
   MISSILEDATA(double speed , double exptime , double expradius) : mspeed(speed) , etime(exptime) , eradius(expradius) {}
};


class Missile {
public :
   Pos2D start;
   Pos2D dest;

   Pos2D cpos;
   Pos2D cvel;

   double theta;        // angle of travel
   double speed;
   double distleft;

   double crad;         // current radius;
   double radspeed;     // speed radius expands
   double explodetime;  // time it takes to explode

   int rad;            // total detonation radius

   MISSILE_STATE state;
   

   Missile(Pos2D startpos , Pos2D destpos , MISSILEDATA data);

   void SetDestination(Pos2D destpos);

   virtual ~Missile() {}
   
   virtual void Update(double dt);
   virtual void Display();

   void Explode();
   void Destroy();

   int X() {return (int)cpos.X();}
   int Y() {return (int)cpos.Y();}
   MISSILE_STATE State() {return state;}
   bool Exploding() {return (state != NORMAL) && (state != TOAST);}
   int CRad() {return (int)crad;}
   int Rad() {return rad;}
   
   virtual EagleColor MissileColor()    {return EagleColor(255,   0,  0, 255);}
   virtual EagleColor IExplosionColor() {return EagleColor(255,   0,  0, 127);}
   virtual EagleColor OExplosionColor() {return EagleColor(255, 127,  0, 255);}
};



class AAMissile : public Missile {
public :
   AAMissile(Pos2D startpos , Pos2D destpos , MISSILEDATA data);
   virtual EagleColor MissileColor()    {return EagleColor(255,255,255,255);}
   virtual EagleColor IExplosionColor() {return EagleColor(0.0f,0.25f,1.0f,0.5f);}
   virtual EagleColor OExplosionColor() {return EagleColor(1.0f,1.0f,1.0f,1.0f);}
};


typedef Missile* (*MISSILECREATOR)(Pos2D startpos , Pos2D destpos , MISSILEDATA data);

Missile* PlainMissileCreator(Pos2D startpos , Pos2D destpos , MISSILEDATA data);
Missile* AAMissileCreator(Pos2D startpos , Pos2D destpos , MISSILEDATA data);



class MissileLauncher {

protected :
   std::vector<Missile*> missiles;
   double ttnl;/// time to next launch
   double tbl;/// time between launches

   Pos2D lpos;/// launcher position
   
   MISSILECREATOR mcreator;
   
public :

   MissileLauncher(MISSILECREATOR creator , Pos2D pos , double time_between_launches) :
         missiles(),
         ttnl(0.0f),
         tbl(time_between_launches),
         lpos(pos),
         mcreator(creator)
   {}
   virtual ~MissileLauncher();
   
   void FreeMissiles();
   
   virtual void Launch(Pos2D destpos , MISSILEDATA data);

   void DelayLaunch(double delay) {ttnl += delay;}
   
   void Update(double dt);
   void Display();
   
   bool Ready() {return ttnl <= 0.0f;}

   virtual int NMissilesPerLaunch() {return 1;}
   int NMissilesActive();

   std::vector<Missile*> GetMissiles() {return missiles;}
};


class SpreadMissileLauncher : public MissileLauncher {
protected :
   double nmsl;
   double arcrad;
   
public :
   SpreadMissileLauncher(MISSILECREATOR creator , Pos2D pos , double time_between_launches);

   virtual void Launch(Pos2D destpos , MISSILEDATA data);

   void SetSpread(int nmissiles , double arc);

   void LaunchSpread(int nmissiles , double arc , Pos2D destpos , double mspeed , double etime , int mradius);

   int NMissilesPerLaunch() {return nmsl;}
};



typedef MissileLauncher* (*LAUNCHERCREATOR) (MISSILECREATOR creator , Pos2D pos , double time_between_launches);

MissileLauncher* SingleMissileLauncher    (MISSILECREATOR creator , Pos2D pos , double time_between_launches);
MissileLauncher* TripleMissileLauncher    (MISSILECREATOR creator , Pos2D pos , double time_between_launches);
MissileLauncher* PentupleMissileLauncher  (MISSILECREATOR creator , Pos2D pos , double time_between_launches);
MissileLauncher* SeptupleMissileLauncher  (MISSILECREATOR creator , Pos2D pos , double time_between_launches);



class MissileBattery {
protected :
   std::vector<MissileLauncher*> launchers;

   int nmissilestotal;
   int nmissilesleft;
   int clauncher;

   MISSILECREATOR mcfunc;
   MISSILEDATA mdata;
   
   
   
public :
   
   void Launch(int destx , int desty);
   
   MissileBattery(int nmissiles , MISSILECREATOR mcreator , MISSILEDATA data);
   ~MissileBattery();

   void Free();

     
   void AddLauncher(MissileLauncher* ml);
   
   void Update(double dt);
   void Display();
   
   bool Ready();
   int NMissilesLeft();
   int NMissilesActive();
   
   std::vector<Missile*> GetMissiles();
};



#endif // Missiles_H




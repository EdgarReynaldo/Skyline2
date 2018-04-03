

#ifndef Missiles_H
#define Missiles_H

#include "Eagle.hpp"
#include "Eagle/backends/Allegro5Backend.hpp"
#include "Config.hpp"



#include <cmath>
#include <cstdlib>

#include <list>
using std::list;

#include <string>
using std::string;


extern Rectangle r;

enum MISSILE_STATE {
   NORMAL = 0,
   EXPLODING = 1,
   EXPLODED = 2,
   IMPLODING = 3,
   TOAST = 4
};



class Missile {
public :
   float sx,sy;        // start position
   float xp,yp;        // position
   float xv,yv;        // velocity
   float theta;        // angle of travel
   float crad;         // current radius;
   float radspeed;     // speed radius expands
   float explodetime;  // time it takes to explode
   int rad;            // total detonation radius
   MISSILE_STATE state;
   
   Missile(float startx , float starty , float speed , float angle , int radius , float time);
   virtual ~Missile() {}
   
   virtual void Update(double dt);
   virtual void Display();

   void Explode();
   void Destroy();

   int X() {return (int)xp;}
   int Y() {return (int)yp;}
   MISSILE_STATE State() {return state;}
   bool Exploding() {return (state != NORMAL) && (state != TOAST);}
   int CRad() {return (int)crad;}
};



class AAMissile : public Missile {
public :
   float dx,dy;// destination x and y
   float mspeed;
   float distance_left;
   
   AAMissile(float startx , float starty , float speed , float angle , int radius , float time , int destx , int desty);

   virtual void Update(double dt);
   virtual void Display();

};



class AI;
class Game;
class EnemyAI;
class PlayerAI;


class MissileBattery {

friend class AI;
friend class Game;
friend class EnemyAI;
friend class PlayerAI;

private :
   list<Missile*> missiles;
   int nmissiles;
   int nmissilesleft;
   float ttnl;// time to next launch
   float tbl;// time between launches
   
   AI* ai;
   
public :
   
   MissileBattery();
   ~MissileBattery();
   
   void FreeAI();
   void FreeMissiles();
   
   void SetAI(AI* new_ai);
   
   void Update(double dt);
   void CheckInputs();
   void Display();
   
   bool Ready() {return ttnl <= 0.0f;}

};


class AI {
protected :
   MissileBattery* mb;
   Rectangle zone;
   int nmissiles;
   Config c;
   
public :
   
   AI(Rectangle launch_zone , int num_missiles , Config config);

   virtual ~AI() {}
   
   virtual void ControlMissileBattery(MissileBattery* battery)=0;
   void DelayLaunchBy(double dt);
   
   virtual void Update(double dt)=0;
   virtual void CheckInputs()=0;
};



class EnemyAI : public AI {
private :

   void Launch();
   
public :
   EnemyAI(Rectangle launch_zone , int num_missiles , Config config);
   
   virtual void ControlMissileBattery(MissileBattery* battery);
   
   virtual void Update(double dt);
   virtual void CheckInputs();

};



class PlayerAI : public AI {
private :
   
public :
   PlayerAI(Rectangle launch_zone , int num_missiles , Config config);
   
   virtual void ControlMissileBattery(MissileBattery* battery);
   
   virtual void Update(double dt);
   virtual void CheckInputs();

   void Launch(int dx , int dy);
   
};


#endif // Missiles_H




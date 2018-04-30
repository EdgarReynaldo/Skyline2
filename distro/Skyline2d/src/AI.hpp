



#ifndef AI_HPP
#define AI_HPP

#include "NewConfig.hpp"
#include "Eagle/Position.hpp"
#include "Eagle/Events.hpp"
#include "Eagle/InputHandler.hpp"


class MissileBattery;
class LaserBattery;
class Missile;

class AI {
protected :
   MissileBattery* mb;
public :
   AI();
   virtual ~AI() {}
   
   virtual void Setup(const Config& c)=0;

   virtual void Update(double dt)=0;
   virtual void HandleEvent(EagleEvent ee)=0;
   virtual void CheckInputs()=0;
   
   virtual void Display()=0;

   std::vector<Missile*> GetMissiles();
   int NMissilesLeft();
};



class EnemyAI : public AI {
protected :

public :
   
   EnemyAI();
   ~EnemyAI();
   
   void Free();

   virtual void Setup(const Config& c);

   virtual void Update(double dt);
   virtual void HandleEvent(EagleEvent ee);
   virtual void CheckInputs();

   virtual void Display();
};


#include "Missiles.hpp"

class PlayerAI : public AI {
private :
   LaserBattery* lb;

   Pos2I aim;
   
   Input mtrigger;
   Input mtriggerheld;
   Input ltrigger;
   Input ltriggerheld;
   
   LAUNCHERCREATOR lcreator;
   
public :

   PlayerAI();
   ~PlayerAI();
   void Free();

   virtual void Setup(const Config& c);

   virtual void Update(double dt);
   virtual void HandleEvent(EagleEvent ee);
   virtual void CheckInputs();

   virtual void Display();
   
   void DrawLasers();
   
   bool MissilesReady();
};



#endif // AI_HPP


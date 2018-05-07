



#ifndef Shields_HPP
#define Shields_HPP



#include "Eagle/Position.hpp"
#include "Eagle/Color.hpp"



class Shield {
   double cx,cy,irad,orad,maxrad;
   double thetastart,thetastop;
   double hp,maxhp;
   
   EagleColor icol;
   EagleColor ocol;
   
protected :
   
   void ResetShieldRadius();
   
public :
   Shield();
   
   void Setup(Pos2D top , double radius , double thickness);
   
   bool Hit(double xpos , double ypos);

   void Damage(double area);

   void Reset();
   
   void Draw();
   
   bool Up() {return hp > 0.0;}
   
   double Percent() {return hp/maxhp;}
};

#endif // Shields_HPP


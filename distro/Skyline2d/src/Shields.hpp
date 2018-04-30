



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
   
   void Setup(Pos2D left , Pos2D right , double height , double thickness);

   bool Hit(double xpos , double ypos);

   void Damage(double area);

   void Reset();
   
   void Draw();
};

#endif // Shields_HPP


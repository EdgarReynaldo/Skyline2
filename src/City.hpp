

#ifndef City_H
#define City_H


#include "Eagle/backends/Allegro5Backend.hpp"

#include <string>
using std::string;


#include "Shields.hpp"
#include "Collision.hpp"

int CountPixels(Allegro5Image* image , ALLEGRO_COLOR c);
int CountPixels(Allegro5Image* image , int red , int green , int blue , int alpha);



class City {
   friend class Game;
private :
   Allegro5Image original;
   Allegro5Image workingcopy;
   
   string city;
   
   int x,y;
   
   int maxpixels;
   int pixelsleft;
   
   Shield shield;
   
   Hitmask hitmask;
   
public :
   
   City(string name , string path , int screenw , int screenh);
   
   void Display();
   
   void Reset();
   
   void Destroy(EagleGraphicsContext* win , int cx , int cy , int radius);
   
   void DamageShield(double damage);
   
   void Recount();
   
   double PercentLeft();

   string Name() {return city;}
   
   bool Hit(int tx , int ty);
   bool HitShield(int tx , int ty);
   
   int X() {return x;}
   int Y() {return y;} 
   
   bool ShieldDown() {return !shield.Up();}
   
   const Hitmask& GetHitmask() {return hitmask;}
};


#endif // City_H


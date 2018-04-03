

#ifndef City_H
#define City_H


#include "Eagle/backends/Allegro5Backend.hpp"



#include <string>
using std::string;


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
   
public :
   
   City(string name , string path , int screenw , int screenh);
   
   void Display(EagleGraphicsContext* win);
   
   void Reset();
   
   void Destroy(EagleGraphicsContext* win , int cx , int cy , int radius);
   
   void Recount();
   
   float PercentLeft();

   string Name() {return city;}
   
   bool Hit(int tx , int ty);
};


#endif // City_H


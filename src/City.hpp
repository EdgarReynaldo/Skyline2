

#ifndef City_H
#define City_H


#include "Eagle.hpp"

#include <string>
using std::string;


int CountPixels(BITMAP* bmp , int color);
int CountPixels(BITMAP* bmp , int red , int green , int blue);



class City {
private :
   BitmapHandler original;
   BitmapHandler workingcopy;
   
   string city;
   
   int x,y;
   
   int maxpixels;
   int pixelsleft;
   
public :
   
   City(string name , string path , int screenw , int screenh);
   
   void Display(BITMAP* bmp);
   
   void Reset();
   
   void Destroy(int cx , int cy , int radius);
   
   void Recount();
   
   float PercentLeft();

   string Name() {return city;}
   
   bool Hit(int tx , int ty);
};


#endif // City_H


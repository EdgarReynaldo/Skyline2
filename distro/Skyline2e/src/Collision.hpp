



#ifndef Collision_HPP
#define Collision_HPP


#include <vector>
#include <map>



struct Line {
   int y;
   int xmin;
   int xmax;
   Line(int ypos , int lx , int rx) : y(ypos) , xmin(lx) , xmax(rx) {}
};

typedef std::vector<Line> LineList;

LineList CircleScanLines(int centerx , int centery , int radius);


class Allegro5Image;

class Hitmask {
   std::vector<bool> hitmask;
   int width;
   int height;
   
   std::map<int , LineList> cmap;
   
   int solidmax;
   int solidcount;

public :
   void Resize(int w , int h);
   void ReadFromImage(Allegro5Image* a5img);

   void EraseCircle(int ecx , int ecy , int radius);
   
   bool Hit(int x , int y) const;

   int SolidCount() {return solidcount;}
   
   double PercentLeft() {return (double)solidcount/solidmax;}
   
   void DrawMask(int xpos , int ypos) const ;
};


class Missile;
class City;
class LaserBlast;

void MissilesVsCity(std::vector<Missile*> missiles , City* city);

void MissilesVsShield(std::vector<Missile*> missiles , City* city);

void ExplosionsVsMissiles(std::vector<Missile*> missiles);

void LasersVsMissiles(std::vector<LaserBlast*> lblasts , std::vector<Missile*> missiles);


#endif // Collision_HPP


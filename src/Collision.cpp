



#include "Collision.hpp"

#include "City.hpp"
#include "Missiles.hpp"
#include "Lasers.hpp"


LineList CircleScanLines(int centerx , int centery , int radius) {
   LineList ll;
   int r2 = radius*radius;
   
   for (int y = centery - radius ; y <= centery + radius ; ++y) {
      double y2 = y*y;
      for (int x = centerx - radius ; x <= centerx + radius ; ++x) {
         if (x*x + y2 > r2) {
            continue;
         }
         else {
            ll.push_back(Line(y , x , centerx + (centerx - x)));
            break;
         }
      }
   }
   return ll;
}



void Hitmask::Resize(int w , int h) {
   width = w;
   height = h;
   hitmask = std::vector<bool>(w*h , false);
}



void Hitmask::ReadFromImage(Allegro5Image* a5img) {
   hitmask.clear();
   solidmax = 0;
   solidcount = 0;
   if (!a5img) {return;}
   ALLEGRO_LOCKED_REGION* lock = al_lock_bitmap(a5img->AllegroBitmap() , al_get_bitmap_format(a5img->AllegroBitmap()) , ALLEGRO_LOCK_READONLY);
   (void)lock;
   unsigned int w = a5img->W();
   unsigned int h = a5img->H();
   Resize(w,h);
   int count = 0;
   for (unsigned int y = 0 ; y < h ; ++y) {
      for (unsigned int x = 0 ; x < w ; ++x) {
         unsigned char r,g,b,a;
         al_unmap_rgba(al_get_pixel(a5img->AllegroBitmap() , x , y) , &r , &g , &b , &a);
         int index = y*w + x;
         bool hit = (a > 0)?true:false;
         hitmask[index] = hit;
         if (hit) {
            ++solidmax;
         }
      }
   }
   solidcount = solidmax;
   al_unlock_bitmap(a5img->AllegroBitmap());
}



void Hitmask::EraseCircle(int ecx , int ecy , int radius) {
   if (radius < 1) {return;}
   std::map<int , LineList>::iterator it = cmap.find(radius);
   LineList ll;
   if (it != cmap.end()) {
      ll = it->second;
   }
   else {
      cmap[radius] = ll = CircleScanLines(0 , 0 , radius);
   }
   for (unsigned int i = 0 ; i < ll.size() ; ++i) {
      Line l = ll[i];
      int y = l.y + ecy;
      if (y < 0) {continue;}
      if (y >= height) {continue;}
      int lx = l.xmin + ecx;
      int rx = l.xmax + ecx;
      if (rx < 0) {continue;}
      if (rx >= width) {rx = width - 1;}
      if (lx >= width) {continue;}
      if (lx < 0) {lx = 0;}
      for (int x = lx ; x <= rx ; ++x) {
         int index = y*width + x;
         if (hitmask[index]) {
            --solidcount;
         }
         hitmask[index] = false;
      }
   }
}



bool Hitmask::Hit(int x , int y) const {
   if (x < 0 || x >= width) {return false;}
   if (y < 0 || y >= height) {return false;}
   return hitmask[y*width + x];
}




/// ----------------------     Global collision functions      ------------------------



void MissilesVsCity(std::vector<Missile*> missiles , City* city) {
   const Hitmask& citymask = city->GetHitmask();
   const int x = city->X();
   const int y = city->Y();
   for (unsigned int i = 0 ; i < missiles.size() ; ++i) {
      Missile* m = missiles[i];
      if (!m->Exploding()) {
         if (citymask.Hit(m->X() - x , m->Y() - y)) {
            m->Explode();
         }
      }
   }
}



void MissilesVsShield(std::vector<Missile*> missiles , City* city) {
   if (city->ShieldDown()) {return;}
   for (unsigned int i = 0 ; i < missiles.size() ; ++i) {
      Missile* m = missiles[i];
      if (m->Exploding()) {continue;}
      if (city->HitShield(m->X() , m->Y())) {
         double d = m->Rad()*m->Rad()*M_PI;
         m->Explode();
         city->DamageShield(d);
      }
   }
}



void ExplosionsVsMissiles(std::vector<Missile*> missiles) {
   for (unsigned int i = 0 ; i < missiles.size() ; ++i) {
      Missile* m1 = missiles[i];
      bool e1 = m1->Exploding();
      for (unsigned int j = i + 1 ; j < missiles.size() ; ++j) {
         Missile* m2 = missiles[j];
         bool e2 = m2->Exploding();
         if (e1 || e2) {
            int x1 = m1->X();
            int x2 = m2->X();
            int dx = x2 - x1;
            int y1 = m1->Y();
            int y2 = m2->Y();
            int dy = y2 - y1;
            int r1 = m1->CRad();
            int r2 = m2->CRad();
            int r = r1 + r2;
            if (dx*dx + dy*dy <= r*r) {
               if (!e1) {m1->Explode();}
               if (!e2) {m2->Explode();}
            }
         }
      }
   }
}



void LasersVsMissiles(std::vector<LaserBlast*> lblasts , std::vector<Missile*> missiles) {
   /// This is gonna suck - O(n^2), but we can skip missiles that are exploding, and stop checking if we find a hit
   for (unsigned int i = 0 ; i < missiles.size() ; ++i) {
      Missile* m = missiles[i];
      if (m->Exploding()) {continue;}
      for (unsigned int j = 0 ; j < lblasts.size() ; ++j) {
         LaserBlast* lb = lblasts[j];
         if (lb->Hit(m->X() , m->Y())) {
            m->Explode();
            break;
         }
      }
   }
}












#include "City.hpp"
#include "Globals.hpp"

#include "Utility.hpp"



City::City(string name , string path , int screenw , int screenh) :
//   BitmapHandler(const char* filepath , RGB* pal , BMP_MEM_TYPE bmp_mem_type , BMP_DRAW_TYPE bmp_draw_type ,
//                  bool load_now , ResourceRegistry* reg = &resource_registry);
   original(win , string("") + "City::original - " + path),
   workingcopy(win , string("") + "City::workingcopy - " + path),
   city(name),
   x(0),
   y(0),
   maxpixels(-1),
   pixelsleft(0),
   shield(),
   wc_lock(0)
{
   if (!original.Load(path)) {
      throw EagleException(StringPrintF("Failed to load city (%s).\n" , path.c_str()));
   }
   ConvertMaskColorToAlphaZero(&original , EagleColor(255,0,255,255));

   /// Scale up the city to fit the background
   Rectangle scaled = Rectangle(0 , 0 , original.W() , original.H());
   scaled.Scale((double)sw/original.W());
   if (!workingcopy.Allocate(scaled.W() , scaled.H())) {
      throw EagleException("Failed to allocate working copy.\n");
   }
///   scaled.SetPos(0 , sh - scaled.H());
   win->SetDrawingTarget(&workingcopy);
   win->Clear(EagleColor(0,0,0,0));
   win->DrawStretchedRegion(&original , Rectangle(0,0,original.W() , original.H()) , scaled);

   original.Allocate(scaled.W() , scaled.H());
   /// Draw the scaled city on the original
   win->SetDrawingTarget(&original);
   win->Clear(EagleColor(0,0,0,0));
   win->SetCopyBlender();
   win->Draw(&workingcopy , 0 , 0);
   win->RestoreLastBlendingState();
   x = (screenw - original.W())/2;
   y = screenh - original.H();
   
   shield.Setup(Pos2D(0.5 , y + 0.5) , Pos2D(screenw - 0.5 , y + 0.5) , screenh , 100);
   
   maxpixels = original.W()*original.H() - CountPixels(&original , al_map_rgba(0,0,0,0));
//   maxpixels = original.W()*original.H() - CountPixels(original , makecol(255,0,255));
///   maxpixels = original.W()*original.H() - CountPixels(original , 255 , 0 , 255);
   EagleLog() << StringPrintF("%s pixel count = %i\n" , path.c_str() , maxpixels);
   EagleLog() << StringPrintF("Total size = %i\n" , original.W()*original.H());
   pixelsleft = maxpixels;
//   int index = path.find_first_of('.');
//   string newpath = path.substr(0 , index);
//   newpath += ".bmp";
//   original.SaveImage(newpath.c_str() , 0);
}



void City::Display() {
   win->Draw(&workingcopy , x , y);
   shield.Draw();
}



void City::Reset() {
   win->SetDrawingTarget(&workingcopy);
   win->SetCopyBlender();
   win->Draw(&original , 0 , 0);
   win->RestoreLastBlendingState();
   pixelsleft = maxpixels;
   shield.Reset();
}



void City::Destroy(EagleGraphicsContext* win , int cx , int cy , int radius) {
   float xpos = cx - x + 0.5f;
   float ypos = cy - y + 0.5f;
   win->DrawFilledCircle(xpos , ypos , (float)radius , EagleColor(0,0,0,0));
   
}



void City::DamageShield(double damage) {
   shield.Damage(damage);
}



void City::Recount() {
   pixelsleft = original.W()*original.H() - CountPixels(&workingcopy , al_map_rgba(0,0,0,0));
//   pixelsleft = original.W()*original.H() - CountPixels(workingcopy , makecol(255,0,255));
///   pixelsleft = original.W()*original.H() - CountPixels(workingcopy , 255 , 0 , 255);
}



float City::PercentLeft() {
   return float(pixelsleft)/float(maxpixels);
}



void City::LockCityBuffer() {
   if (!wc_lock) {
      wc_lock = al_lock_bitmap(workingcopy.AllegroBitmap() , al_get_bitmap_format(workingcopy.AllegroBitmap()) , ALLEGRO_LOCK_READONLY);
   }
}



void City::UnLockCityBuffer() {
   if (wc_lock) {
      al_unlock_bitmap(workingcopy.AllegroBitmap());
      wc_lock = 0;
   }
}



bool City::Hit(int tx , int ty) {
   if (shield.Hit(tx + 0.5 , ty + 0.5)) {
      return true;
   }
   int xpos = tx - x;
   int ypos = ty - y;
   if ((xpos < 0) || (xpos >= workingcopy.W())) {return false;}
   if ((ypos < 0) || (ypos >= workingcopy.H())) {return false;}
//   if (getpixel(workingcopy , xpos , ypos) == makecol(255,0,255)) {return false;}
   ALLEGRO_COLOR c = al_get_pixel(workingcopy.AllegroBitmap() , xpos , ypos);
   unsigned char r,g,b,a;
   al_unmap_rgba(c , &r , &g , &b , &a);
   if (a != 0) {
      return true;
   }
   return false;
}



bool City::HitShield(int tx , int ty) {
   return shield.Hit(tx + 0.5 , ty + 0.5);
}



int CountPixels(Allegro5Image* image , ALLEGRO_COLOR c) {
   unsigned char r,g,b,a;
   
   al_unmap_rgba(c , &r , &g , &b , &a);
   
   return CountPixels(image,r,g,b,a);
}



int CountPixels(Allegro5Image* image , int red , int green , int blue , int alpha) {
   if (!image) {return 0;}
   
   ALLEGRO_BITMAP* bmp = image->AllegroBitmap();
   
   if (!bmp) {return 0;}
   
   ALLEGRO_LOCKED_REGION* lock = al_lock_bitmap(bmp , ALLEGRO_PIXEL_FORMAT_ANY , ALLEGRO_LOCK_READONLY);
   (void)lock;
   int count = 0;
   for (int y = 0 ; y < image->H() ; ++y) {
      for (int x = 0 ; x < image->W() ; ++x) {
         unsigned char r,g,b,a;
         al_unmap_rgba(al_get_pixel(bmp,x,y) , &r , &g , &b , &a);
         if ((r == red) && (g == green) && (b == blue) && (a == alpha)) {++count;}
      }
   }
   
   al_unlock_bitmap(bmp);
   
   return count;
}






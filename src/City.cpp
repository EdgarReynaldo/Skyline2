

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
   shield()
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
   
}



void City::DrawDamage() {
   ALLEGRO_BITMAP* bmp = original.AllegroBitmap();
   al_draw_tinted_bitmap(bmp , al_map_rgb(255,0,0) , x , y , 0);
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
   hitmask.ReadFromImage(&workingcopy);
   shield.Reset();

   EagleLog() << StringPrintF("City::Reset - %s pixel count = %i\n" , city.c_str() , hitmask.SolidCount());
   EagleLog() << StringPrintF("Total size = %i\n" , workingcopy.Area());
}



void City::SetupShield(Pos2D top , double radius , double thickness) {
   shield.Setup(top , radius , thickness);
}



void City::Destroy(EagleGraphicsContext* win , int cx , int cy , int radius) {
   int xpos = cx - x + 0.5f;
   int ypos = cy - y + 0.5f;
   win->DrawFilledCircle(xpos + 0.5f , ypos + 0.5f , (float)radius , EagleColor(0,0,0,0));
   hitmask.EraseCircle(xpos , ypos , radius);
}



void City::DamageShield(double damage) {
   shield.Damage(damage);
}



double City::PercentLeft() {
   return hitmask.PercentLeft();
}



bool City::Hit(int tx , int ty) {
   int rx = tx - x;
   int ry = ty - y;
   bool hit = hitmask.Hit(rx,ry);
   if (hit) {
      EagleLog() << StringPrintF("City hit at %d,%d\n" , rx , ry) << std::endl;
   }
   return hit;
}



bool City::HitShield(int tx , int ty) {
   return shield.Hit(tx + 0.5 , ty + 0.5);
}





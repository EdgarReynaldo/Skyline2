

#include "City.hpp"

City::City(string name , string path , int screenw , int screenh) :
//   BitmapHandler(const char* filepath , RGB* pal , BMP_MEM_TYPE bmp_mem_type , BMP_DRAW_TYPE bmp_draw_type ,
//                  bool load_now , ResourceRegistry* reg = &resource_registry);
   original(path.c_str() , 0 , MEMORY , TRANSPARENT , true),
   workingcopy(path.c_str() , 0 , MEMORY , TRANSPARENT , true),
   city(name),
   x(0),
   y(0),
   maxpixels(0),
   pixelsleft(0)
{
   if (!original) {
      throw EagleError(StringPrintF("Failed to load city (%s).\n" , path.c_str()));
   }
   x = (screenw - original.W())/2;
   y = screenh - original.H();
   maxpixels = original.W()*original.H() - CountPixels(original , bitmap_mask_color(original));
//   maxpixels = original.W()*original.H() - CountPixels(original , makecol(255,0,255));
///   maxpixels = original.W()*original.H() - CountPixels(original , 255 , 0 , 255);
   OutputLog() << StringPrintF("%s pixel count = %i\n" , path.c_str() , maxpixels);
   OutputLog() << StringPrintF("Total size = %i\n" , original.W()*original.H());
   pixelsleft = maxpixels;
//   int index = path.find_first_of('.');
//   string newpath = path.substr(0 , index);
//   newpath += ".bmp";
//   original.SaveImage(newpath.c_str() , 0);
}



void City::Display(BITMAP* bmp) {
   workingcopy.DrawTo(bmp , x , y);
}



void City::Reset() {
   blit(original , workingcopy , 0 , 0 , 0 , 0 , original.W() , original.H());
   pixelsleft = maxpixels;
}



void City::Destroy(int cx , int cy , int radius) {
   int xpos = cx - x;
   int ypos = cy - y;
//   circlefill(workingcopy , xpos , ypos , radius , makecol(255,0,255));
   circlefill(workingcopy , xpos , ypos , radius , bitmap_mask_color(workingcopy));
}



void City::Recount() {
   pixelsleft = original.W()*original.H() - CountPixels(workingcopy , bitmap_mask_color(workingcopy));
//   pixelsleft = original.W()*original.H() - CountPixels(workingcopy , makecol(255,0,255));
///   pixelsleft = original.W()*original.H() - CountPixels(workingcopy , 255 , 0 , 255);
}



float City::PercentLeft() {
   return float(pixelsleft)/float(maxpixels);
}



bool City::Hit(int tx , int ty) {
   int xpos = tx - x;
   int ypos = ty - y;
   if ((xpos < 0) || (xpos >= workingcopy.W())) {return false;}
   if ((ypos < 0) || (ypos >= workingcopy.H())) {return false;}
//   if (getpixel(workingcopy , xpos , ypos) == makecol(255,0,255)) {return false;}
   if (getpixel(workingcopy , xpos , ypos) == bitmap_mask_color(workingcopy)) {return false;}
/*
   int color = getpixel(workingcopy , xpos , ypos);
   int r = getr(color);
   int g = getg(color);
   int b = getb(color);
   if ((r == 255) && (g == 0) && (b == 255)) {return false;}
//*/
   return true;
}



int CountPixels(BITMAP* bmp , int color) {
   if (!bmp) {return 0;}
   int count = 0;
   for (int y = 0 ; y < bmp->h ; ++y) {
      for (int x = 0 ; x < bmp->w ; ++x) {
         if (getpixel(bmp,x,y) == color) {++count;}
      }
   }
   return count;
}



int CountPixels(BITMAP* bmp , int red , int green , int blue) {
   if (!bmp) {return 0;}
   int count = 0;
   for (int y = 0 ; y < bmp->h ; ++y) {
      for (int x = 0 ; x < bmp->w ; ++x) {
         int color = getpixel(bmp,x,y);
         int r = getr(color);
         int g = getg(color);
         int b = getb(color);
         if ((r == red) && (g == green) && (b == blue)) {++count;}
      }
   }
   return count;
}


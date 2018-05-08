



#include "Eagle/backends/Allegro5Backend.hpp"

#include "Game.hpp"
#include "Globals.hpp"



int main2(int argc , char** argv) {
   
   (void)argc;
   (void)argv;
   
   GLE2D xaxis(Pos2D(-100.0 , 0.0) , 0.0);
   GLE2D yaxis(Pos2D(0.0 , -100.0) , M_PI/2.0);
   GLE2D xyaxis(Pos2D(0.0 , 0.0) , M_PI/4.0);
   Pos2D intersection;
   
   Pos2D f(4.9 , 4.9);
   GLE2D extra(f , 3.0*M_PI/4.0);

   bool i = IntersectionPoint(xyaxis , extra , &intersection);
   
   double d1 = DistanceFromLine(f , xaxis);
   double d2 = DistanceFromLine(f , yaxis);
   double d3 = DistanceFromLine(f , xyaxis);
   
   printf("Distance from lines = %lf , %lf , %lf\n" , d1 , d2 , d3);
   printf("%s Intersection point = %lf , %lf\n" , (!i)?"No":"" , intersection.X() , intersection.Y());
   return 0;
}

int main(int argc , char** argv) {
   
   (void)argc;
   (void)argv;
   
   EnableLog();
   
   if (!SendOutputToFile("SkylineLog.txt" , "Skyline log:\n" , false)) {
      std::cout << "Failed to open SkylineLog.txt!" << std::endl;
   }
   
   sys = GetAllegro5System();
   
   if (sys->Initialize(EAGLE_FULL_SETUP) != EAGLE_FULL_SETUP) {
      /// Usually touch fails to install, so ignore
      EagleLog() << "Failed to initialize some subsystem. Continuing anyway." << std::endl;
   }
   
   bool fullscreen = false;
   int screenw = 1024;
   int screenh = 768;

   ConfigFile cf;
   if (cf.LoadFromFile("Data/Config.txt")) {
      fullscreen = (cf["Graphics"]["fullscreen"].compare("true") == 0);
      try {
         int w = 0;
         int h = 0;
         w = stoi(cf["Graphics"]["screenw"]);
         h = stoi(cf["Graphics"]["screenh"]);
         screenw = w;
         screenh = h;
      }
      catch (...) {}
   }
   
   int winflags = EAGLE_OPENGL | (fullscreen?EAGLE_FULLSCREEN_WINDOW:EAGLE_WINDOWED);
   sw = screenw;
   sh = screenh;
   
   win = sys->CreateGraphicsContext("WIN" , sw , sh , winflags);
///   win = sys->CreateGraphicsContext("WIN" , sw , sh , EAGLE_OPENGL | EAGLE_FULLSCREEN_WINDOW);
   
   sw = win->Width();
   sh = win->Height();
   
   EAGLE_ASSERT(win);
   EAGLE_ASSERT(win->Valid());
   
   win->Clear();
   win->FlipDisplay();
   
   q = sys->GetSystemQueue();
   
   t = sys->GetSystemTimer();
   
   f = win->LoadFont("Verdana.ttf" , -36);
   
///   int w = f->Width("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
///   (void)w;
   
   if (!f) {
      EagleWarn() << "Failed to load Verdana.ttf!" << std::endl;
      f = win->DefaultFont();
   }
   
   try {
      Game game("Data/Cities.txt");
      
      EagleLog() << "Game successfully created." << std::endl;
      
      t->Start();
      
      game.Run();
   }
   catch (...) {
      EagleError() << "Game encountered an exception and needs to close." << std::endl;
   }
   
   EagleLog() << "Game closed." << std::endl;
   
   return 0;
}


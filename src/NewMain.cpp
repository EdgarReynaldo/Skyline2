



#include "Eagle/backends/Allegro5Backend.hpp"

#include "Game.hpp"
#include "Globals.hpp"



int main(int argc , char** argv) {
   
   (void)argc;
   (void)argv;
   
   SendOutputToFile("SkylineLog.txt" , StringPrintF("Skyline log:\n") , false);
   
   sys = GetAllegro5System();
   
   if (sys->Initialize(EAGLE_FULL_SETUP) != EAGLE_FULL_SETUP) {
      /// Usually touch fails to install, so ignore
      EagleLog() << "Failed to initialize some subsystem. Continuing anyway." << std::endl;
   }
   
   win = sys->CreateGraphicsContext("WIN" , sw , sh , EAGLE_OPENGL | EAGLE_WINDOWED);
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
   
   Game game("Data/Cities.txt");
   
   t->Start();
   
   game.Run();
   
   return 0;
}


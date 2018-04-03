



#include "Eagle/backends/Allegro5Backend.hpp"

#include "Game.hpp"
#include "Globals.hpp"



int main(int argc , char** argv) {
   
   (void)argc;
   (void)argv;
   
   SendOutputToFile("SkylineLog.txt" , StringPrintF("Skyline log:\n") , false);
   
   sys = GetAllegro5System();///dynamic_cast<Allegro5System*>(Eagle::EagleLibrary::System("Allegro5"));
///   sys = dynamic_cast<Allegro5System*>(Eagle::EagleLibrary::System("Allegro5"));
   
   if (sys->Initialize(EAGLE_FULL_SETUP) != EAGLE_FULL_SETUP) {
      /// Usually touch fails to install, so ignore
      EagleLog() << "Failed to initialize some subsystem. Continuing anyway." << std::endl;
   }
   
   win = sys->CreateGraphicsContext("WIN" , ww , wh , EAGLE_OPENGL | EAGLE_WINDOWED);
   
   EAGLE_ASSERT(win);
   EAGLE_ASSERT(win->Valid());
   
   win->Clear();
   win->FlipDisplay();
   
   q = sys->GetSystemQueue();
   
   t = sys->GetSystemTimer();
   
   
   Game game("Data/Cities.txt");
   
   t->Start();
   
   game.Run();
   
   return 0;
}


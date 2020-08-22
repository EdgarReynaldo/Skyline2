

#include "physfs.h"

#include "Eagle.hpp"


#include "Eagle/backends/Allegro5Backend.hpp"

#include "Game.hpp"
#include "Globals.hpp"
#include "SoundMan.hpp"



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

   al_reserve_samples(2);

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
      
      entrysid = soundman.LoadSound(cf["SFX"]["Entry"]);
      explodesid = soundman.LoadSound(cf["SFX"]["Explode"]);
      launchsid = soundman.LoadSound(cf["SFX"]["Launch"]);
      
   }
   
   if (entrysid == BADSOUNDID || explodesid == BADSOUNDID || launchsid == BADSOUNDID) {
      EagleWarn() << "Failed to load needed sound." << std::endl;
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
   
   int fsz = 32;
   try {
      fsz = stoi(cf["Menu"]["Fontsize"]);
   }
   catch (...) {}

   menu_font = win->LoadFont(cf["Menu"]["Font"] , fsz);

///   int w = f->Width("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
///   (void)w;
   
   if (!f) {
      EagleWarn() << "Failed to load Verdana.ttf!" << std::endl;
      f = win->DefaultFont();
   }
   
   try {
      Game game(&cf);
      
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




int dmain(int argc , char** argv) {
   
   std::string dir = "";
   if (argc > 1) {
      dir = argv[1];
   }
   
   Allegro5System* sys = GetAllegro5System();
   
   EAGLE_ASSERT(sys);
   
   if (sys->Initialize(EAGLE_FULL_SETUP) != EAGLE_FULL_SETUP) {
      EagleLog() << "Failed to initialize eagle.";
   }
   
   std::string cwd = GetCWD();
   
   FileSystem* fs = sys->GetFileSystem();
   
   PHYSFS_init(argv[0]);

   std::shared_ptr<Folder> rootfolder = fs->ReadFolder(cwd , true);
   
   rootfolder->PrintContentsAbsolute();
   
   return 0;
}



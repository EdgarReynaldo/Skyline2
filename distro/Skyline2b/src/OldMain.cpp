

#include "Eagle.hpp"
#include <jpgalleg.h>
#include <loadpng.h>

#include "Game.hpp"
#include "Config.hpp"

#include <cstdlib>
#include <ctime>
#include <iostream>
using std::endl;

void CallOut() {pausegame = true;}
void CallIn() {pausegame = false;}

void FixCWD();


int main(int argc , char** argv) {

   if (!SendOutputToFile("Skyline_log.txt" , "Skyline log file\n\n" , false)) {
      return 1;
   }

   if (!GetAllegroReady(true,true,true)) {
      OutputLog() << "Failed to initialize allegro properly." << endl;
      return 1;
   }

   FixCWD();

//   jpgalleg_init();
//   register_png_file_type();

   gfx_mode_handler.Setup(SW_BACKGROUND , CallIn , CallOut);

   int dcd = DesktopInfoKnown()?DCD():32;
   GfxModeData gmode(GFX_AUTODETECT_WINDOWED , 800 , 600 , dcd);

   if (FULL_SUCCESS != gfx_mode_handler.ChangeToMode(gmode , CRM_DIRECT)) {
      OutputLog() << "Failed to set graphics mode (" << gmode << ")" << endl;
      return 1;
   }

   set_color_conversion(COLORCONV_TOTAL | COLORCONV_KEEP_TRANS);

   if (!exists("Data/Config.txt")) {
      CreateConfigFile("Data/Config.txt");
   }

   set_config_file("Data/Config.txt");
   LoadConfig();

   
   int ret = 0;
   try {
      Game game("Data/Cities.txt" , gmode.W() , gmode.H() , GetConfigSettings().gui_padding);
      ret = game.Run();
   }
   catch (...) {
      OutputLog() << "Unrecoverable error detected. Shutting down." << endl;
      ret = 1;
   }

   resource_registry.FreeBitmapHandlers();

   allegro_exit();

   return ret;
}
END_OF_MAIN()



void FixCWD() {
  char dir[1024];
  get_executable_name(dir , 1023);
  chdir(StripFileName(dir).c_str());
}


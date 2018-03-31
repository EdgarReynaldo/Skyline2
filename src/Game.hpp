

#ifndef Game_H
#define Game_H

#include "Eagle.hpp"

#include <vector>
#include <map>
using std::map;
using std::vector;


#include "City.hpp"
#include "Missiles.hpp"
#include "Config.hpp"
#include "SettingsWidget.hpp"


extern volatile bool pausegame;

enum STATE {
   INTRO = 0,
   MENUE = 1,// MENU (E) to prevent clashes with allegro's MENU struct
   GAME = 2,
   WIN = 3,
   LOSE = 4,
   QUIT = 5
};


class Game {
private :
   map<string , City*> cities;
   string citystr;
   City* city;
   BitmapHandler background;
   BitmapHandler buffer;
   BitmapHandler nopointer;
   BitmapHandler okpointer;
   BitmapHandler* pointer;
   STATE state;
   DIFFICULTY difficulty;
   int status;
   bool redraw;
   int mousex;
   int mousey;

   vector<MissileBattery*> enemy_mbs;

   MissileBattery player_mb;
   PlayerAI* player_ai;

   int padding;

   SettingWidget enemy_nmsl_setting;
   SettingWidget player_nmsl_setting;
   SettingWidget enemy_tbl_setting;
   SettingWidget player_tbl_setting;
   SettingWidget city_left_setting;
   SettingWidget enemy_mspd_setting;
   SettingWidget player_mspd_setting;
   SettingWidget enemy_mrad_setting;
   SettingWidget player_mrad_setting;
   SettingWidget enemy_explode_time_setting;
   SettingWidget player_explode_time_setting;

   GenericRadioButton nl_radio;
   TextWidget nl_text;
   
   GenericRadioButton alldiff_radio;
   TextWidget alldiff_text;

   Button play_btn;
   Button quit_btn;
   Button save_changes_btn;
   Button revert_changes_btn;
   
   DropDownList city_ddl;
   TextWidget city_text;
   
   WidgetHandler menu_gui;
   
   ConfigSettings last_config_settings;
   ConfigSettings current_config_settings;
   Config current_config;
   bool config_changed;

   void SetAlldiffRadio();
   void SetupRadioButtons();
   void SetGuiValues(ConfigSettings cset);
   void ResetGuiPositions();
   void SetupGui();

   Difficulty ReadGuiDifficulty();
   Config ReadGuiCustomConfig();

   void SetupMissileBatteries(Config c);

   void DrawGame(BITMAP* bmp);

   void CheckCollisions();
   STATE CheckGameState();

   int NumMissilesLeft();

public :
   static volatile int ticks;

   Game(string cityfile , int screenw , int screenh , int pad);
   ~Game();

   void Free();
   void FreeMissileBatteries();
   void FreeCities();


   void SetupCities(string file , int screenw , int screenh);

   int Run();

   STATE Update(double dt);
   STATE CheckInputs(int msx , int msy);
   void Display(BITMAP* bmp);

};

#endif // Game_H


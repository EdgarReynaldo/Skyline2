

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
///#include "SettingsWidget.hpp"


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

   Allegro5Image nopointer;
   Allegro5Image okpointer;
   Allegro5Image* pointer;

   STATE state;
   DIFFICULTY difficulty;
   int status;
   
   vector<MissileBattery*> enemy_mbs;

   MissileBattery player_mb;
   PlayerAI* player_ai;

   ConfigSettings last_config_settings;
   ConfigSettings current_config_settings;
   Config current_config;
   bool config_changed;



   void SetupMissileBatteries(Config c);

   void DrawGame();

   void CheckCollisions();
   STATE CheckGameState();

   int NumMissilesLeft();

public :
///   static volatile int ticks;

   Game(string cityfile);
   ~Game();

   void Free();
   void FreeMissileBatteries();
   void FreeCities();


   void SetupCities(string file , int screenw , int screenh);

   int Run();
   
   void Display();
   
   STATE HandleEvent(EagleEvent ee);

   STATE Update(double dt);

   STATE CheckInputs();

};

#endif // Game_H


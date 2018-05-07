

#ifndef Game_H
#define Game_H

#include "Eagle.hpp"

#include <vector>
#include <map>
using std::map;
using std::vector;


#include "City.hpp"
#include "Missiles.hpp"
#include "Lasers.hpp"
#include "NewConfig.hpp"
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

class EnemyAI;
class PlayerAI;

class Game {
private :
   map<string , City*> cities;
   string citystr;
   City* city;

   Allegro5Image cbuffer;
   Allegro5Image bg;
   Allegro5Image nopointer;
   Allegro5Image okpointer;
   Allegro5Image* pointer;

   STATE state;
   DIFFICULTY difficulty[NUM_GAME_SETTINGS];

   int status;
   
   EnemyAI* enemy;
   PlayerAI* player;

   GameConfig gameconfig;
   
   Config config;
   
   bool config_changed;

   bool show_hud;
   bool show_fps;
   bool show_mask;
   int frame_skip;
   
   double lps;
   double fps;
   double ups;
   
   
   void SetupAI();
   
   void DrawGame();

   void CheckCollisions();

   STATE CheckGameState();

   int NumMissilesLeft();

   double CityPercentLeft();
   
public :

   Game(string cityfile);
   ~Game();

   void Free();
   void FreeCities();


   void SetupCities(string file , int screenw , int screenh);

   int Run();
   
   void Display();
   
   STATE HandleEvent(EagleEvent ee);

   STATE Update(double dt);

   STATE CheckInputs();

};

#endif // Game_H


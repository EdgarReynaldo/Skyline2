

#include <fstream>
using std::ifstream;

#include <iostream>
using std::iostream;
using std::endl;

#include <algorithm>

#include "Game.hpp"
#include "NewConfig.hpp"
#include "Globals.hpp"
#include "Blend.hpp"
#include "AI.hpp"
#include "RNG.hpp"


#include "Eagle/backends/Allegro5Backend.hpp"
#include "Eagle/InputHandler.hpp"



const float SPT = 1.0f/float(FPS);



/// ----------------------------------     Game class     -------------------------------------------

string okpointer_file("Data/Images/OkPointer.bmp");
string nopointer_file("Data/Images/NoPointer.bmp");



/**
void Game::SetupMissileBatteries(Config c) {
   FreeMissileBatteries();
   const char* cstr = c.nl_str.c_str();
   int count = 0;
   int nlaunchers = 0;
   int ret = 0;
   ret = sscanf(cstr , "%i%n" , &nlaunchers , &count);
///   OutputLog() << StringPrintF("Num launchers scan returned %i items.\n" , ret);
   if (1 != ret) {
      throw EagleException(StringPrintF("Failed to read number of enemy launchers from '%s'\n" , cstr));
   }
   cstr += count;
   float x = -1.0f;
   EnemyAI* enemyai = new EnemyAI(r , c.enemy_nmsl/nlaunchers , c);
   for (int i = 0 ; i < nlaunchers ; ++i) {
      ret = sscanf(cstr , "%f%n" , &x , &count);
      if (1 != ret) {
         throw EagleException(StringPrintF("Failed to read launcher #%i's zone from '%s'.\n" , i + 1 , cstr));
      }
      cstr += count;
      MissileBattery* mbtry = new MissileBattery();
      Rectangle r((float)sw*x - 25,-20,50,50);
      mbtry->SetAI(enemyai);
      if (nlaunchers > 1) {
         enemyai->DelayLaunchBy(c.enemy_tbl*((float)i/(float)(nlaunchers - 1)));
      }
      enemy_mbs.push_back(mbtry);
   }
   player_ai = new PlayerAI(Rectangle(sw/2,sh - 10,1,1) , c.player_nmsl , c);
   player_mb.SetAI(player_ai);
}
//*/



void Game::SetupAI() {
   EAGLE_ASSERT(enemy);
   EAGLE_ASSERT(player);
   enemy->Setup(config);
   player->Setup(config);
}



int SortMissileByY(Missile* m1 , Missile* m2) {return m1->Y() < m2->Y();}


void Game::DrawGame() {
   /// Draw collision buffer
   win->SetDrawingTarget(&cbuffer);
   
   win->Clear(EagleColor(0,0,0,255));

   SetAdditiveBlender();

   player->DrawLasers();

   
   /// Draw cbuffer to display
   win->DrawToBackBuffer();
   
   /// Copy blender
   al_set_blender(ALLEGRO_ADD , ALLEGRO_ONE , ALLEGRO_ZERO);
   
   /// Copy collision buffer to display
   win->Draw(&cbuffer , 0 , 0);

   /// Multiply blender
   al_set_blender(ALLEGRO_ADD , ALLEGRO_DEST_COLOR , ALLEGRO_ZERO);

   win->DrawStretchedRegion(&bg , 0 , 0 , bg.W() , bg.H() , 0 , 0 , sw , sh);
         
   /// Pre multiplied alpha blender
   al_set_blender(ALLEGRO_ADD , ALLEGRO_ONE , ALLEGRO_INVERSE_ALPHA);
   
   city->Display();

   std::vector<Missile*> missiles = enemy->GetMissiles();
   std::vector<Missile*> missiles2 = player->GetMissiles();
   missiles.insert(missiles.end() , missiles2.begin() , missiles2.end());
   missiles2.clear();
   std::sort(missiles.begin() , missiles.end() , SortMissileByY);
   
   for (unsigned int i = 0 ; i < missiles.size() ; ++i) {
      Missile* m = missiles[i];
      m->Display();
   }
   
   /// HUD display
   if (show_fps) {
      win->DrawTextString(f , StringPrintF("FPS : %4.2lf" , fps) , 10 , 10 , EagleColor(255,127,0,255) , HALIGN_LEFT , VALIGN_TOP);
      win->DrawTextString(f , StringPrintF("LPS : %4.2lf" , lps) , 10 , 30 , EagleColor(255,127,0,255) , HALIGN_LEFT , VALIGN_TOP);
   }
   
   /// Win status
   
   if (state == WIN) {
      win->DrawTextString(f , "WIN! WIN! WIN! WIN! WIN!" , sw/2 , sh/2 , EagleColor(0,255,0)  ,HALIGN_CENTER , VALIGN_CENTER);
   }
   else if (state == LOSE) {
      win->DrawTextString(f , "YOU LOSE! LOSE! LOSE! LOSE! LOSE!" , sw/2 , sh/2 , EagleColor(255,0,0)  ,HALIGN_CENTER , VALIGN_CENTER);
   }

   win->Draw(pointer , mouse_x - pointer->W()/2 , mouse_y - pointer->H()/2);
}



void Game::CheckCollisions() {

   vector<Missile*> pmissiles = player->GetMissiles();
   vector<Missile*> emissiles = enemy->GetMissiles();
   vector<Missile*> missiles(pmissiles);
   missiles.insert(missiles.begin() , emissiles.begin() , emissiles.end());

   /// O(log(n))?
   /// Check for missiles blowing up other missiles
   win->PushDrawingTarget(&city->workingcopy);
   win->SetCopyBlender();
   for (unsigned int i = 0 ; i < missiles.size() ; ++i) {
      Missile* m1 = missiles[i];
      int x1 = m1->X();
      int y1 = m1->Y();
      int r1 = m1->CRad();
//      MISSILE_STATE s1 = m1->State();
      bool e1 = m1->Exploding();
      if (e1) {
         city->Destroy(win,x1,y1,r1);
      }
      for (unsigned int j = i + 1 ; j < missiles.size() ; ++j) {
         Missile* m2 = missiles[j];
         int x2 = m2->X();
         int y2 = m2->Y();
         int r2 = m2->CRad();
//         MISSILE_STATE s2 = m2->State();
         bool e2 = m2->Exploding();
         int dx = x2 - x1;
         int dy = y2 - y1;
         int dsq = dx*dx + dy*dy;
         int rsq = (r1 + r2)*(r1 + r2);
         if (dsq <= rsq) {
            /// If one is exploding, the other should explode as well if not already
            /// If either is exploding, cause damage to the city
            if (e1 || e2) {
               if (e1) {
                  if (!e2) {
                     m2->Explode();
                  }
               }
               if (e2) {
                  if (!e1) {
                     m1->Explode();
                  }
               }
            }
         }
      }
   }
   win->RestoreLastBlendingState();
   win->PopDrawingTarget();
   /// Check for enemy missiles hitting the city
   for (unsigned int i = 0 ; i < emissiles.size() ; ++i) {
      Missile* m = emissiles[i];
      if (city->Hit(m->X() , m->Y())) {
         m->Explode();
         if (city->HitShield(m->X() , m->Y())) {
            city->DamageShield(M_PI*m->Rad()*m->Rad());
         }
      }
   }
   /// Check for player lasers hitting missiles
///   vector<Laser*> lasers = player_lasers.GetActiveLaserBeams();/// Line distance hit detection doesn't work yet
   ALLEGRO_BITMAP* bmp = cbuffer.AllegroBitmap();
   ALLEGRO_LOCKED_REGION* lock = al_lock_bitmap(bmp , ALLEGRO_PIXEL_FORMAT_ANY_32_WITH_ALPHA , ALLEGRO_LOCK_READONLY);
   (void)lock;/// This lock is used by al_get_pixel below
   for (unsigned int i = 0 ; i < missiles.size() ; ++i) {
      Missile* m = missiles[i];
      ALLEGRO_COLOR c;
      c = al_get_pixel(bmp , m->X() , m->Y());
      unsigned char r,g,b;
      al_unmap_rgb(c , &r , &g , &b);
      if (r == 255 && g == 255 && b == 255) {
         if (!m->Exploding()) {m->Explode();}
      }
   }
   al_unlock_bitmap(bmp);
      
}



STATE Game::CheckGameState() {
   city->Recount();
   float percent_allowed = config.city_left;

   // 1.0 - percents[difficulty] = percent allowed
   // city->PercentLeft - percents[difficulty] = percent remaining
   // percent remaining / percent allowed = percent of allowable damage taken


   float percent_left = (city->PercentLeft() - percent_allowed)/(1.0f - percent_allowed);
   if (percent_left <= 0.0f) {
      return LOSE;
   }
   if (NumMissilesLeft() == 0) {
      return WIN;
   }
   return state;
}



int Game::NumMissilesLeft() {
   return enemy->NMissilesLeft();
}

/**
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
   
   bool config_changed;

//*/
Game::Game(string cityfile) :
   cities(),
   citystr(""),
   city(0),
   cbuffer(sw,sh),
   bg("Data/Images/CloudySky.png"),
   nopointer(nopointer_file),
   okpointer(okpointer_file),
   pointer(&nopointer),
   state(INTRO),
   difficulty(),
   status(0),
   enemy(0),
   player(0),
   gameconfig(),
   config(),
   config_changed(false),
   show_hud(false),
   show_fps(false),
   frame_skip(0),
   lps(0.0),
   fps(0.0)
{
   
   bool loaded_config = gameconfig.LoadConfig();
   
   if (!loaded_config) {
      throw EagleException("Failed to load config!\n");
   }
   
   SetupCities(cityfile , sw , sh);
   if (!cbuffer.Valid()) {
      throw EagleException("Failed to allocate cbuffer!\n");
   }
   if (!bg.Valid()) {
      throw EagleException("Failed to load background.\n");
   }
   if (!okpointer.Valid()) {
      throw EagleException(StringPrintF("Failed to load okay pointer (%s)!\n" , okpointer_file.c_str()));
   }
   if (!nopointer.Valid()) {
      throw EagleException(StringPrintF("Failed to load no pointer (%s)!\n" , nopointer_file.c_str()));
   }
   
   
   
   enemy = new EnemyAI;
   player = new PlayerAI;
   
   ConvertMaskColorToAlphaZero(&okpointer , EagleColor(255,0,255,255));
   ConvertMaskColorToAlphaZero(&nopointer , EagleColor(255,0,255,255));
   
   Seed(gameconfig.GetSeed());
}



Game::~Game() {
   Free();
}



void Game::Free() {
   if (enemy) {
      delete enemy;
      enemy = 0;
   }
   if (player) {
      delete player;
      player = 0;
   }
   FreeCities();
}



void Game::FreeCities() {
   for (map<string , City*>::iterator it = cities.begin() ; it != cities.end() ; ++it) {
      City* c = it->second;
      delete c;
   }
   cities.clear();
}



void Game::SetupCities(string file , int screenw , int screenh) {
   FreeCities();
   ifstream strm(file.c_str() , std::ios::binary);
   if (!strm) {
      throw EagleException(StringPrintF("Could not open game setup file (%s)!\n" , file.c_str()));
   }
   int count = 0;
   while (!strm.eof()) {
      string line;
      GetLinePN(strm , line);
      EagleLog() << StringPrintF("line read for city = '%s'" , line.c_str()) << std::endl;
      if (line.size() == 0) {continue;}
      ++count;
      unsigned int index = line.find_first_of(',');
      if (index == string::npos) {
         throw EagleException("No comma detected. Bad city file format.\nShould be 'cityname,path/to/city.bmp'\n");
      }
      string cityname = line.substr(0 , index);
      string path = line.substr(index + 1 , string::npos);
      cities[cityname] = new City(cityname , path , screenw , screenh);
   }
   if (!count) {
      throw EagleException("No cities detected!\n");
   }
}



int Game::Run() {

   al_show_mouse_cursor(dynamic_cast<Allegro5GraphicsContext*>(win)->AllegroDisplay());

   bool paused = false;
   bool timing = false;

   int display_count = 0;
   double display_time = 0.0;

   int input_count = 0;
   double input_time = 0.0;
   
   while (state != QUIT) {
      if (timing) {
         ProgramTime pt1 = ProgramTime::Now();
         Display();
         ProgramTime pt2 = ProgramTime::Now();
         ++display_count;
         display_time += pt2 - pt1;
         if (display_count == 60) {
            fps = (double)display_count/display_time;
            display_count = 0;
            display_time = 0.0;
         }
      }
      else {
         Display();
      }
      int nevents = 0;
      do {
         EagleEvent ee = sys->WaitForSystemEventAndUpdateState();
         if (ee.type == EAGLE_EVENT_DISPLAY_CLOSE) {
            state = QUIT;
         }
         if (ee.type == EAGLE_EVENT_DISPLAY_SWITCH_OUT) {
            paused = true;
         }
         if (ee.type == EAGLE_EVENT_DISPLAY_SWITCH_IN) {
            paused = false;
         }
         if (ee.type == EAGLE_EVENT_KEY_DOWN) {
            if (ee.keyboard.keycode == EAGLE_KEY_T) {
               timing = !timing;
               show_fps = timing;
            }
         }
         if (!paused) {
            if (ee.type == EAGLE_EVENT_TIMER) {
               ++nevents;
               if (nevents == 1) {/// Slow down gracefully if the gpu or cpu can't handle it
                  Update(ee.timer.eagle_timer_source->SPT());
                  frame_skip = 0;
               }
               else {
                  frame_skip++;
               }
            }
            if (timing) {
               ProgramTime pt1 = ProgramTime::Now();
               CheckInputs();
               ProgramTime pt2 = ProgramTime::Now();
               double dt = pt2 - pt1;
               input_count++;
               input_time += dt;
               if (input_count == 60) {
                  lps = (double)input_count/input_time;
                  input_count = 0;
                  input_time = 0.0;
               }
            }
            else {
               CheckInputs();
            }
            state = HandleEvent(ee);
         }
      } while (sys->GetSystemQueue()->HasEvent(0));
   }
   return 0;
}




void Game::Display() {
   win->DrawToBackBuffer();
   switch (state) {
      case INTRO :
         win->Clear(EagleColor(0,0,0));
         break;
      case MENUE :
         {
            win->Clear(EagleColor(0,0,0));
            const char* text[4] = {
               "ESC : Quit",
               "1   : Chicago  - Easy",
               "2   : New York - Medium",
               "3   : Houston  - Hard"
            };
            EagleColor colors[4] = {
               EagleColor(255,0,0),
               EagleColor(0,255,0),
               EagleColor(0,255,255),
               EagleColor(255,255,0)
            };
            for (int i = 0 ; i < 4 ; ++i) {
               win->DrawTextString(win->DefaultFont() , text[i] , sw/2.0 , sh/2 - 40 + i*20 , colors[i] , HALIGN_CENTER , VALIGN_CENTER);
            }
         }
         break;
      case GAME :
      case WIN :
      case LOSE :
         DrawGame();
         break;
      case QUIT :
         break;
   }
   win->FlipDisplay();
}



STATE Game::Update(double dt) {
   switch (state) {
      case INTRO :
         state = MENUE;
         break;
      case MENUE :
///         menu_gui.Update(dt);
         break;
      case GAME :
         enemy->Update(dt);
         player->Update(dt);
         CheckCollisions();
         state = CheckGameState();
         break;
      case WIN :
      case LOSE :
      case QUIT :
         break;
   }
   return state;
}

STATE Game::HandleEvent(EagleEvent ee) {
   switch (state) {
   case INTRO :
      break;
   case MENUE :
      if (ee.type == EAGLE_EVENT_KEY_DOWN) {
         if (ee.keyboard.keycode >= EAGLE_KEY_1 && ee.keyboard.keycode <= EAGLE_KEY_3) {
            const char* citys[3] = {"Chicago" , "New York" , "Houston"};
            city = cities[citys[ee.keyboard.keycode - EAGLE_KEY_1]];
            EAGLE_ASSERT(city);
            city->Reset();
            
            DIFFICULTY d[3] = {
               EASY , MEDIUM , HARD
            };
            gameconfig.SetOverallDifficulty(d[ee.keyboard.keycode - EAGLE_KEY_1]);

            config = gameconfig.GetSelectedConfig();

            Seed(gameconfig.GetSeed());

            SetupAI();

            state = GAME;
         }
         if (ee.keyboard.keycode == EAGLE_KEY_ESCAPE) {
            state = QUIT;
         }
      }
      break;
   case GAME :
      {
         if (ee.type == EAGLE_EVENT_KEY_DOWN) {
            if (ee.keyboard.keycode == EAGLE_KEY_ESCAPE) {
               state = MENUE;
            }
            if (ee.keyboard.keycode == EAGLE_KEY_W) {state = WIN;}
            if (ee.keyboard.keycode == EAGLE_KEY_L) {state = LOSE;}
         }
         enemy->HandleEvent(ee);
         player->HandleEvent(ee);
      }
      break;
   case WIN:
   case LOSE:
      if (AnyKeyPressed(0)) {state = MENUE;}
      break;
   case QUIT :
      break;
   default :
      break;
   }
   return state;
}



STATE Game::CheckInputs() {
   switch (state) {
      case INTRO :
         state=MENUE;
         break;
      case MENUE :
         break;
         /**
         if (input_key_press(KEY_ESC)) {state = QUIT;}
         
         menu_gui.CheckInputs(msx,msy);
         while (menu_gui.HasMessages()) {
            WidgetMsg wmsg = menu_gui.TakeNextMessage();
            OutputLog() << "menu_gui received WidgetMsg [" << wmsg << "]" << endl;
            if (wmsg.Topic() == TOPIC_SETTING_WIDGET) {
               config_changed = true;
               SetAlldiffRadio();
            }
            if (wmsg == WidgetMsg(&nl_radio , TOPIC_RADIO_BUTTON , RADIO_BTN_CLICKED)) {
               config_changed = true;
            }
            if (wmsg == WidgetMsg(&alldiff_radio , TOPIC_RADIO_BUTTON , RADIO_BTN_CLICKED)) {
               // set all difficulties
               DIFFICULTY d = (DIFFICULTY)alldiff_radio.ActiveItemIndex();
               if (d == NUM_DIFFICULTIES) {
                  SetAlldiffRadio();
               }
               else {
                  enemy_nmsl_setting.SetDifficulty(d);
                  player_nmsl_setting.SetDifficulty(d);
                  enemy_tbl_setting.SetDifficulty(d);
                  player_tbl_setting.SetDifficulty(d);
                  city_left_setting.SetDifficulty(d);
                  enemy_mspd_setting.SetDifficulty(d);
                  player_mspd_setting.SetDifficulty(d);
                  enemy_mrad_setting.SetDifficulty(d);
                  player_mrad_setting.SetDifficulty(d);
                  enemy_explode_time_setting.SetDifficulty(d);
                  player_explode_time_setting.SetDifficulty(d);
               }
            }
            if (wmsg == WidgetMsg(&play_btn , TOPIC_BUTTON_WIDGET , BUTTON_CLICKED)) {
               current_config_settings.configs[CUSTOM] = ReadGuiCustomConfig();
               current_config_settings.diffs = ReadGuiDifficulty();
               current_config = GetConfig(current_config_settings);
               city = cities[city_ddl.GetSelection()];
               ASSERT(city);
               city->Reset();
               SetupMissileBatteries(current_config);
               state = GAME;
            }
            if (wmsg == WidgetMsg(&quit_btn , TOPIC_BUTTON_WIDGET , BUTTON_CLICKED)) {
               state = QUIT;
            }
            if (wmsg == WidgetMsg(&save_changes_btn , TOPIC_BUTTON_WIDGET , BUTTON_CLICKED)) {
               current_config_settings.configs[CUSTOM] = ReadGuiCustomConfig();
               current_config_settings.diffs = ReadGuiDifficulty();
               current_config = GetConfig(current_config_settings);
               config_changed = false;
               last_config_settings = current_config_settings;
               SetConfigSettingsAndWrite(current_config_settings);
               
            }
            if (wmsg == WidgetMsg(&revert_changes_btn , TOPIC_BUTTON_WIDGET , BUTTON_CLICKED)) {
               current_config_settings = last_config_settings;
               config_changed = false;
               SetGuiValues(current_config_settings);
            }
         }
         */
         
         break;
      case GAME :
         enemy->CheckInputs();
         player->CheckInputs();
         if (player->MissilesReady()) {
            pointer = &okpointer;
         }
         else {
            pointer = &nopointer;
         }
         break;
      case WIN :
      case LOSE :
         if (AnyKeyPressed(0)) {state = MENUE;}
         break;
      case QUIT :
         break;
   }
   return state;
}


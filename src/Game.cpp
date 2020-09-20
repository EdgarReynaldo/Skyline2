

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

#include "allegro5/allegro_color.h"

const float SPT = 1.0f/float(FPS);

   static DIFFICULTY seldiff = EASY;
   static int selcity = 0;
   static int index = 0;
   static const int diffkeys[4] = {EAGLE_KEY_A , EAGLE_KEY_B , EAGLE_KEY_C , EAGLE_KEY_D};


/// ----------------------------------     Game class     -------------------------------------------

string okpointer_file("Data/Images/OkPointer.bmp");
string nopointer_file("Data/Images/NoPointer.bmp");



int SortMissileByY(Missile* m1 , Missile* m2) {return m1->Y() < m2->Y();}



void Game::SetupAI() {
   EAGLE_ASSERT(enemy);
   EAGLE_ASSERT(player);
   enemy->Setup(config);
   player->Setup(config);
}



void Game::DrawGame() {
   /// Draw collision buffer
   win->SetDrawingTarget(cbuffer);
   
   win->Clear(EagleColor(0,0,0,255));

   SetAdditiveBlender();

   player->DrawLasers();

   
   /// Draw cbuffer to display
   win->DrawToBackBuffer();
   
   /// Copy blender
   al_set_blender(ALLEGRO_ADD , ALLEGRO_ONE , ALLEGRO_ZERO);
   
   /// Copy collision buffer to display
   win->Draw(cbuffer , 0 , 0);

   /// Multiply blender
   al_set_blender(ALLEGRO_ADD , ALLEGRO_DEST_COLOR , ALLEGRO_ZERO);

   win->Draw(bg , 0 , 0);
         
   /// Pre multiplied alpha blender
   al_set_blender(ALLEGRO_ADD , ALLEGRO_ONE , ALLEGRO_INVERSE_ALPHA);
   
   if (show_mask) {
      city->GetHitmask().DrawMask(city->X() , city->Y());
   }
   else {
      city->DrawDamage();
         
      city->Display();
   }
   
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
   if (show_hud) {
      int pml = player->NMissilesLeft();
      int eml = enemy->NMissilesLeft();
      win->DrawTextString(f , StringPrintF("%d" , pml) , 10 , sh - 10 , EagleColor(255,255,255,255) , HALIGN_LEFT , VALIGN_BOTTOM);
      win->DrawTextString(f , StringPrintF("%d" , eml) , sw - 10 , 10 , EagleColor(0,127,255,255) , HALIGN_RIGHT , VALIGN_TOP);
      double real_pct = CityPercentLeft();
      double hue = real_pct*120.0;
      double sat = 1.0;
      double val = 1.0;
      float r,g,b;
      al_color_hsv_to_rgb(hue , sat , val , &r , &g , &b);
      EagleColor c(r,g,b);
      win->DrawTextString(f , StringPrintF("City left : %2.2lf" , real_pct) , sw/2 , sh - 10 , c , HALIGN_CENTER , VALIGN_BOTTOM);
      real_pct = city->ShieldPercent();
      hue = real_pct*120.0;
      al_color_hsv_to_rgb(hue , sat , val , &r , &g , &b);
      EagleColor c2(r,g,b);
      win->DrawTextString(f , StringPrintF("Shield %s!" , city->ShieldDown()?"Down":"Up") , sw/2 , 10 , c2 , HALIGN_CENTER , VALIGN_TOP);
   }
   
   /// Timing
   if (show_fps) {
      win->DrawTextString(f , StringPrintF("FPS : %4.2lf" , fps) , 10 , 10 , EagleColor(255,127,0,255) , HALIGN_LEFT , VALIGN_TOP);
      win->DrawTextString(f , StringPrintF("LPS : %4.2lf" , lps) , 10 , 50 , EagleColor(255,127,0,255) , HALIGN_LEFT , VALIGN_TOP);
      win->DrawTextString(f , StringPrintF("UPS : %4.2lf" , ups) , 10 , 90 , EagleColor(255,127,0,255) , HALIGN_LEFT , VALIGN_TOP);
      win->DrawTextString(f , StringPrintF("FPS : %4.2f" , win->GetFPS()) , sw - 10 , sh - 10 , EagleColor(255,127,0,255) , HALIGN_RIGHT , VALIGN_BOTTOM);
   }

   /// Status
   if (state == WIN) {
      win->DrawTextString(f , "WIN! WIN! WIN! WIN! WIN!" , sw/2 , sh/2 , EagleColor(0,255,0)  ,HALIGN_CENTER , VALIGN_CENTER);
   }
   else if (state == LOSE) {
      win->DrawTextString(f , "YOU LOSE! LOSE! LOSE! LOSE! LOSE!" , sw/2 , sh/2 , EagleColor(255,0,0)  ,HALIGN_CENTER , VALIGN_CENTER);
   }

   /// Pointer
   win->Draw(pointer , mouse_x - pointer->W()/2 , mouse_y - pointer->H()/2);
}



void Game::CheckCollisions() {

   ProgramTime t1;
   ProgramTime t2;
   double dt = 0.0;

   vector<Missile*> pmissiles = player->GetMissiles();
   vector<Missile*> emissiles = enemy->GetMissiles();
   vector<Missile*> missiles(pmissiles);
   missiles.insert(missiles.begin() , emissiles.begin() , emissiles.end());

   t1 = ProgramTime::Now();
   
   /// Check for missile explosions making other missiles explode
   ExplosionsVsMissiles(missiles);
   
   /// Check for missiles blowing up the city
   win->PushDrawingTarget(&city->workingcopy);
   win->SetCopyBlender();
   for (unsigned int i = 0 ; i < missiles.size() ; ++i) {
      Missile* m1 = missiles[i];
      /// We can optimize this by not drawing the implosion after the explosion. It's already been erased.
      if (m1->Exploding() && m1->State() != IMPLODING) {
         int x1 = m1->X();
         int y1 = m1->Y();
         int r1 = m1->CRad();
         /// Missiles exploding above the city do no damage
         if (y1 >= city->Y() - r1) {
            city->Destroy(win,x1,y1,r1);/// This draws the explosion onto the city
         }
      }
   }
   win->RestoreLastBlendingState();
   win->PopDrawingTarget();

   /// Check for enemy missiles hitting the city
   
   if (!city->ShieldDown()) {
      MissilesVsShield(emissiles , city);
   }
   else {
      MissilesVsCity(emissiles , city);
   }
   
   LasersVsMissiles(player->GetLaserBlasts() , emissiles);
   
   t2 = ProgramTime::Now();
   
   dt = t2 - t1;
   (void)dt;
///   EagleLog() << StringPrintF("Collision frame took %2.8lf seconds.\n" , dt) << std::endl;
}



STATE Game::CheckGameState() {
   if (CityPercentLeft() <= 0.0f) {
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



double Game::CityPercentLeft() {
   double pct_left = city->PercentLeft();
   double pct_allowed = config.city_left;
   return (pct_left - pct_allowed)/(1.0 - pct_allowed);
}



Game::Game(ConfigFile* configfile) :
   cities(),
   citystr(""),
   city(0),
   cbuffer(dynamic_cast<Allegro5Image*>(win->CreateImage(sw,sh,VIDEO_IMAGE,"CollisionBuffer"))),
   bgoriginal(dynamic_cast<Allegro5Image*>(win->LoadImageFromFile("Data/Images/CloudySky.png"))),
   bg(0),
   nopointer(dynamic_cast<Allegro5Image*>(win->LoadImageFromFile(nopointer_file))),
   okpointer(dynamic_cast<Allegro5Image*>(win->LoadImageFromFile(okpointer_file))),
   pointer(nopointer),
   state(INTRO),
   difficulty(),
   status(0),
   enemy(0),
   player(0),
   cf(configfile),
   gameconfig(),
   config(),
   config_changed(false),
   show_hud(false),
   show_fps(false),
   show_mask(false),
   frame_skip(0),
   lps(0.0),
   fps(0.0),
   ups(0.0)
{
   
   bool loaded_config = gameconfig.LoadConfig();
   
   if (!loaded_config) {
      throw EagleException("Failed to load config!\n");
   }
   
   SetupCities(win->Width() , win->Height());
   city = cities.begin()->second;
   if (!cbuffer->Valid()) {
      throw EagleException("Failed to allocate cbuffer!\n");
   }
   if (!bgoriginal->Valid()) {
      throw EagleException("Failed to load background.\n");
   }
   if (!okpointer->Valid()) {
      throw EagleException(StringPrintF("Failed to load okay pointer (%s)!\n" , okpointer_file.c_str()));
   }
   if (!nopointer->Valid()) {
      throw EagleException(StringPrintF("Failed to load no pointer (%s)!\n" , nopointer_file.c_str()));
   }
   
   
   
   enemy = new EnemyAI;
   player = new PlayerAI;
   
   ConvertMaskColorToAlphaZero(okpointer , EagleColor(255,0,255,255));
   ConvertMaskColorToAlphaZero(nopointer , EagleColor(255,0,255,255));
   
   bg = dynamic_cast<Allegro5Image*>(win->CreateImage(win->Width() , win->Height() , VIDEO_IMAGE , "City::BG"));
   win->SetDrawingTarget(bg);
   win->Clear(EagleColor(0,0,0,0));
   win->SetCopyBlender();
   win->DrawStretched(bgoriginal , Rectangle(0,0,win->Width(),win->Height()));
   win->RestoreLastBlendingState();
   
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
   win->FreeImage(bgoriginal);
   win->FreeImage(bg);
   win->FreeImage(cbuffer);
   win->FreeImage(okpointer);
   win->FreeImage(nopointer);
   bgoriginal = 0;
   bg = 0;
   cbuffer = 0;
   okpointer = 0;
   nopointer = 0;
}



void Game::FreeCities() {
   for (map<string , City*>::iterator it = cities.begin() ; it != cities.end() ; ++it) {
      City* c = it->second;
      delete c;
   }
   cities.clear();
}



void Game::SetupCities(int screenw , int screenh) {
   FreeCities();
   
   ConfigSection* cs = &(*cf)["Cities"];
   
   std::vector<std::string> citykeys = cs->GetKeys();
   
   int ncities = (int)citykeys.size();
   
   if (!ncities) {
      throw EagleException("No cities detected!\n");
   }
   for (int i = 0 ; i < ncities ; ++i) {
      cities[citykeys[i]] = new City(citykeys[i] , (*cs)[citykeys[i]] , screenw , screenh);
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
   
   int ccount = 0;
   double coll_time = 0.0;
   
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
///               if (true) {/// Run full speed
                  ProgramTime pt1 = ProgramTime::Now();
                  Update(ee.timer.eagle_timer_source->SPT());
                  ProgramTime pt2 = ProgramTime::Now();
                  ccount++;
                  double dt = pt2 - pt1;
                  coll_time += dt;
                  if (ccount >= 60) {
                     ups = ccount/coll_time;
                     ccount = 0;
                     coll_time = 0.0;
                  }
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
            
            std::map<std::string , City*>::iterator it;
            
            const char* text[5] = {
               "ESC : Quit",
               "A   : Easy",
               "B   : Medium",
               "C   : Hard",
               "D   : Insane"
            };
            EagleColor colors[5] = {
               EagleColor(255,255,255),
               EagleColor(0,255,255),
               EagleColor(0,255,0),
               EagleColor(255,255,0),
               EagleColor(255,0,0)
            };
            int lineheight = menu_font->Height() + 10;
            for (int i = 0 ; i < 5 ; ++i) {
               win->DrawTextString(menu_font , text[i] , sw/4.0 , sh/2 - 40 + i*lineheight , colors[i]*((i - 1) == seldiff?1.0f:0.5f)  , HALIGN_LEFT , VALIGN_CENTER);
            }
            int l = 0;
            for (it = cities.begin() ; it != cities.end() ; ++it , ++l) {
               win->DrawTextString(menu_font , StringPrintF("%i : %s" , l + 1 , it->first.c_str()) ,
                                   3*sw/4.0 , sh/2 - 40 + l*lineheight , 
                                   (l == index)?EagleColor(255,255,255):EagleColor(64,64,64) , HALIGN_RIGHT , VALIGN_CENTER);
            }
            
            win->DrawTextString(menu_font , "ENTER to play" , sw/2.0 , 3*sh/4.0 , EagleColor(255,255,255) , HALIGN_CENTER , VALIGN_TOP);
            
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
         {
            ProgramTime t1 = ProgramTime::Now();

            enemy->Update(dt);
            player->Update(dt);
            CheckCollisions();
            state = CheckGameState();

            ProgramTime t2 = ProgramTime::Now();
            double dt = t2 - t1;
            (void)dt;
///            EagleLog() << StringPrintF("Update took %2.8lf seconds.\n" , dt);
         }
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
         for (int i = 0 ; i < 4 ; ++i) {
            if (ee.keyboard.keycode == diffkeys[i]) {
               seldiff = (DIFFICULTY)i;
            }
         }
         
         if (ee.keyboard.keycode >= EAGLE_KEY_1 && ee.keyboard.keycode <= EAGLE_KEY_9) {
            
            std::map<std::string , City*>::iterator it = cities.begin();

            selcity = ee.keyboard.keycode - EAGLE_KEY_1;
            if (selcity < 0) {selcity = 0;}
            if (selcity >= (int)cities.size()) {selcity = (int)cities.size() - 1;}
            EAGLE_ASSERT(selcity >= 0);
            
            index = 0;
            while (it != cities.end() && index < selcity) {
               ++it;
               ++index;
            }
            city = cities[it->first];
         }
         
         if (ee.keyboard.keycode == EAGLE_KEY_ENTER) {
            
            EAGLE_ASSERT(city);

            city->Reset();
            
            gameconfig.SetOverallDifficulty(seldiff);

            config = gameconfig.GetSelectedConfig();
            
            city->SetupShield(Pos2D(sw/2.0 , city->Y()) , 3.0*sh/2.0 ,  (double)config.shield_depth);

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
            if (ee.keyboard.keycode == EAGLE_KEY_H) {
               show_hud = !show_hud;
            }
            if (ee.keyboard.keycode == EAGLE_KEY_M) {
               show_mask = !show_mask;
            }
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
            pointer = okpointer;
         }
         else {
            pointer = nopointer;
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


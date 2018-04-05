

#include <fstream>
using std::ifstream;

#include <iostream>
using std::ios;
using std::endl;

#include "Game.hpp"
#include "Config.hpp"
#include "Globals.hpp"
#include "Blend.hpp"


#include "Eagle/backends/Allegro5Backend.hpp"
#include "Eagle/InputHandler.hpp"



const int FPS = 60;

const float SPT = 1.0f/float(FPS);



/// ----------------------------------     Game class     -------------------------------------------

string okpointer_file("Data/Images/OkPointer.bmp");
string nopointer_file("Data/Images/NoPointer.bmp");




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
   int x = 0;
   int y = 0;
   int w = 0;
   int h = 0;
   for (int i = 0 ; i < nlaunchers ; ++i) {
      ret = sscanf(cstr , "%i,%i,%i,%i%n" , &x , &y , &w , &h , &count);
      if (4 != ret) {
         throw EagleException(StringPrintF("Failed to read launcher #%i's zone from '%s'.\n" , i + 1 , cstr));
      }
      cstr += count;
      MissileBattery* mbtry = new MissileBattery();
      Rectangle r(x,y,w,h);
      EnemyAI* enemyai = new EnemyAI(r , c.enemy_nmsl/nlaunchers , c);
      if (nlaunchers > 1) {
         enemyai->DelayLaunchBy(c.enemy_tbl*((float)i/(float)(nlaunchers - 1)));
      }
      mbtry->SetAI(enemyai);
      enemy_mbs.push_back(mbtry);
   }
   player_ai = new PlayerAI(Rectangle(ww/2,wh,1,1) , c.player_nmsl , c);
   player_mb.SetAI(player_ai);
}



void Game::DrawGame() {
   win->SetDrawingTarget(&cbuffer);
   win->Clear();
   SetAdditiveBlender();
   player_lasers.Draw();

   win->DrawToBackBuffer();
   al_set_blender(ALLEGRO_ADD , ALLEGRO_ONE , ALLEGRO_ZERO);
   win->Draw(&cbuffer , 0 , 0);
   
   al_set_blender(ALLEGRO_ADD , ALLEGRO_ONE , ALLEGRO_INVERSE_ALPHA);
   
   city->Display(win);

   for (unsigned int i = 0 ; i < enemy_mbs.size() ; ++i) {
      enemy_mbs[i]->Display();
   }
   player_mb.Display();

   
   if (state == WIN) {
      win->DrawTextString(win->DefaultFont() , "WIN! WIN! WIN! WIN! WIN!" , ww/2 , wh/2 , EagleColor(0,255,0)  ,HALIGN_CENTER , VALIGN_CENTER);
   }
   else if (state == LOSE) {
      win->DrawTextString(win->DefaultFont() , "YOU LOSE! LOSE! LOSE! LOSE! LOSE!" , ww/2 , wh/2 , EagleColor(255,0,0)  ,HALIGN_CENTER , VALIGN_CENTER);
   }

   win->Draw(pointer , mouse_x - pointer->W()/2 , mouse_y - pointer->H()/2);
}



void Game::CheckCollisions() {
   /// TODO : Pre build database of missiles? Not worth it. There will probably never be enough missiles to matter. Maybe...
   vector<MissileBattery*> mbs = enemy_mbs;
   mbs.push_back(&player_mb);
   vector<Missile*> missiles;
   vector<Missile*> emissiles;
   for (unsigned int i = 0 ; i < mbs.size() ; ++i) {
      MissileBattery* mbtry = mbs[i];
      for (list<Missile*>::iterator it = mbtry->missiles.begin() ; it != mbtry->missiles.end() ; ++it) {
         if (i != mbs.size() - 1) {
            emissiles.push_back(*it);
         }
         missiles.push_back(*it);
      }
   }
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
      if (e1) {city->Destroy(win,x1,y1,r1);}
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
      }
   }
   /// Check for player lasers hitting missiles
///   vector<Laser*> lasers = player_lasers.GetActiveLaserBeams();
   ALLEGRO_BITMAP* bmp = cbuffer.AllegroBitmap();
   ALLEGRO_LOCKED_REGION* lock = al_lock_bitmap(bmp , ALLEGRO_PIXEL_FORMAT_ANY_32_WITH_ALPHA , ALLEGRO_LOCK_READONLY);
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
      

/**
      for (unsigned int j = 0 ; j < lasers.size() ; ++j) {
         Laser* l = lasers[j];
         if (l->Hit(m->X() , m->Y())) {
            if (!m->Exploding()) {
               m->Explode();
            }
         }
      }
   }
//*/
}



STATE Game::CheckGameState() {
   city->Recount();
   float percent_allowed = GetConfig().city_left;

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
   int count = 0;
   for (unsigned int i = 0 ; i < enemy_mbs.size() ; ++i) {
      MissileBattery* mbtry = enemy_mbs[i];
      count += mbtry->nmissiles;
      count += mbtry->nmissilesleft;
   }
   return count;
}



Game::Game(string cityfile) :
   cities(),
   citystr(),
   city(0),
   cbuffer(ww,wh),
   nopointer(nopointer_file),
   okpointer(okpointer_file),
   pointer(&nopointer),
   state(INTRO),
   difficulty(EASY),
   status(0),
   enemy_mbs(),
   player_mb(),
   player_ai(0),
   player_lasers(),
   last_config_settings(),
   current_config_settings(),
   current_config(),
   config_changed(false)
{
/**
   padding(pad),
   enemy_nmsl_setting("Number of enemy missiles" , pad),
   player_nmsl_setting("Number of player missiles" , pad),
   enemy_tbl_setting("Enemy time between launch" , pad),
   player_tbl_setting("Player time between launch" , pad),
   city_left_setting("Percent of city left to win" , pad),
   enemy_mspd_setting("Enemy missile speed" , pad),
   player_mspd_setting("Player missile speed" , pad),
   enemy_mrad_setting("Enemy missile radius" , pad),
   player_mrad_setting("Player missile radius" , pad),
   enemy_explode_time_setting("Enemy explosion duration" , pad),
   player_explode_time_setting("Player explosion duration" , pad),
//   GenericRadioButton(std::string name , RADIO_TYPE rtype , Rectangle position , int padding = 0 , UINT wflags = DEFAULT_FLAGS);
   nl_radio("nl_radio" , RADIO_HORIZONTAL , Rectangle(0,0,1,1) , pad),
//   TextWidget(std::string name , std::string text_str , int tx , int ty , UINT wflags = DEFAULT_FLAGS);
   nl_text("nl_text" , "Number of launchers" , 0 , 0),
//   Button(std::string name , BUTTON_SHAPE shape , BTN_ACTION_TYPE atype , FONT* textfont , 
//         std::string label , const InputGroup& input , const Rectangle& position , UINT wflags = DEFAULT_FLAGS);
   alldiff_radio("alldiff_radio" , RADIO_HORIZONTAL , Rectangle(0,0,1,1) , pad),
   alldiff_text("alldiff_text" , "Set all difficulties" , 0 , 0),
   play_btn("play_btn" , RECTANGLE_BTN , SPRING_BTN , font , "Play" , input_key_press(KEY_NONE) , Rectangle(0,0,54,14)),
   quit_btn("quit_btn" , RECTANGLE_BTN , SPRING_BTN , font , "Quit" , input_key_press(KEY_NONE) , Rectangle(0,0,54,14)),
   save_changes_btn("save_changes_btn" , RECTANGLE_BTN , SPRING_BTN , font , "Save" , input_key_press(KEY_NONE) , Rectangle(0,0,54,14)),
   revert_changes_btn("revert_changes_btn" , RECTANGLE_BTN , SPRING_BTN , font , "Revert" , input_key_press(KEY_NONE) , Rectangle(0,0,54,14)),
//   DropDownList(std::string name , FONT* textfont , int num_items_shown , const Rectangle& position ,
//                DROP_DOWN_LIST_DIRECTION direction = DDL_OPEN_BELOW , UINT wflags = DEFAULT_OVERLAP_FLAGS);
   city_ddl("city_ddl" , font , 3 , Rectangle(0,0,108 + pad , 14) , DDL_OPEN_ABOVE),
   city_text("city_text" , "Select A City" , 0 , 0),
//   WidgetHandler(std::string name , BITMAP* surface , const Rectangle& position , UINT wflags = DEFAULT_FLAGS);   
   menu_gui("menu_gui" , screen , Rectangle(0,0,800,600)),
*/
/**
   last_config_settings(GetConfigSettings()),
   current_config_settings(GetConfigSettings()),
   current_config(GetConfig()),
   config_changed(false)
*/
///{
   SetupCities(cityfile , ww , wh);
   if (!cbuffer.Valid()) {
      throw EagleException("Failed to allocate cbuffer!\n");
   }
   if (!okpointer.Load(okpointer_file)) {
      throw EagleException(StringPrintF("Failed to load okay pointer (%s)!\n" , okpointer_file.c_str()));
   }
   if (!nopointer.Load(nopointer_file)) {
      throw EagleException(StringPrintF("Failed to load no pointer (%s)!\n" , nopointer_file.c_str()));
   }
   
   ConvertMaskColorToAlphaZero(&okpointer , EagleColor(255,0,255,255));
   ConvertMaskColorToAlphaZero(&nopointer , EagleColor(255,0,255,255));
   
   srand(current_config_settings.seed);
}



Game::~Game() {
   Free();
}



void Game::Free() {
   FreeMissileBatteries();
   FreeCities();
}



void Game::FreeMissileBatteries() {
   for (unsigned int i = 0 ; i < enemy_mbs.size() ; ++i) {
      delete enemy_mbs[i];
   }
   enemy_mbs.clear();
}



void Game::FreeCities() {
   for (map<string , City*>::iterator it = cities.begin() ; it != cities.end() ; ++it) {
      City* c = it->second;
      delete c;
   }
   cities.clear();
}



void Game::SetupCities(string file , int screenw , int screenh) {
   Free();
   ifstream strm(file.c_str() , ios::binary);
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

   while (state != QUIT) {
      Display();
      do {
         EagleEvent ee = sys->WaitForSystemEventAndUpdateState();
         if (ee.type == EAGLE_EVENT_DISPLAY_CLOSE) {
            state = QUIT;
         }
         if (ee.type == EAGLE_EVENT_TIMER) {
            Update(ee.timer.eagle_timer_source->SPT());
         }
         else {
            CheckInputs();
         }
         state = HandleEvent(ee);
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
               win->DrawTextString(win->DefaultFont() , text[i] , ww/2.0 , wh/2 - 40 + i*20 , colors[i] , HALIGN_CENTER , VALIGN_CENTER);
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
         for (unsigned int i = 0 ; i < enemy_mbs.size() ; ++i) {
            enemy_mbs[i]->Update(dt);
         }
         player_mb.Update(dt);
         player_lasers.Update(dt);
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
            current_config = current_config_settings.configs[d[ee.keyboard.keycode - EAGLE_KEY_1]];
            SetupMissileBatteries(current_config);
            state = GAME;
         }
         if (ee.keyboard.keycode == EAGLE_KEY_ESCAPE) {
            state = QUIT;
         }
      }
      break;
   case GAME :
      {
         if (ee.type == EAGLE_EVENT_KEY_DOWN && ee.keyboard.keycode == EAGLE_KEY_ESCAPE) {
            state = MENUE;
         }
         if (ee.type == EAGLE_EVENT_MOUSE_AXES) {
            player_lasers.AimAt(ee.mouse.x , ee.mouse.y);
         }
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
         for (unsigned int i = 0 ; i < enemy_mbs.size() ; ++i) {
            MissileBattery* mbtry = enemy_mbs[i];
            mbtry->CheckInputs();
         }
         player_mb.CheckInputs();
         player_lasers.CheckInputs();
         if (player_mb.Ready()) {
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


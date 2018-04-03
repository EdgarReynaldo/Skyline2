



#include "SettingsScreen.hpp"



const char* checkbox_checked_file = "Data/Images/Checkbox_Checked.bmp";
const char* checkbox_unchecked_file = "Data/Images/Checkbox_Unchecked.bmp";
const char* checkbox_hover_file = "Data/Images/Checkbox_Hover.bmp";





void Game::SetAlldiffRadio() {
   DIFFICULTY d = enemy_nmsl_setting.SelectedDifficulty();
   SettingWidget* settings[] = {
      &player_nmsl_setting,
      &enemy_tbl_setting,
      &player_tbl_setting,
      &city_left_setting,
      &enemy_mspd_setting,
      &player_mspd_setting,
      &enemy_mrad_setting,
      &player_mrad_setting,
      &enemy_explode_time_setting,
      &player_explode_time_setting
   };
   UINT diff = d;
   for (UINT i = 0 ; i < sizeof(settings)/sizeof(SettingWidget*) ; ++i) {
      if (d != settings[i]->SelectedDifficulty()) {
         diff = NUM_DIFFICULTIES;
         break;
      }
   }
   alldiff_radio.SetActiveItem(diff);
}



void Game::SetupRadioButtons() {
//   BitmapHandler(const char* filepath , RGB* pal , BMP_MEM_TYPE bmp_mem_type , BMP_DRAW_TYPE bmp_draw_type ,
//                  bool load_now , ResourceRegistry* reg = &resource_registry);
   BitmapHandler up(checkbox_unchecked_file , 0 , MEMORY , TRANSPARENT , true , 0);
   BitmapHandler down(checkbox_checked_file , 0 , MEMORY , TRANSPARENT , true , 0);
   BitmapHandler hover(checkbox_hover_file , 0 , MEMORY , TRANSPARENT , true , 0);
   if (!up || !down || !hover) {
      if (!up) {
         throw EagleError(StringPrintF("Failed to load unchecked checkbox file '%s'\n" , checkbox_unchecked_file));
      }
      if (!down) {
         throw EagleError(StringPrintF("Failed to load checked checkbox file '%s'\n" , checkbox_checked_file));
      }
      if (!hover) {
         throw EagleError(StringPrintF("Failed to load hover checkbox file '%s'\n" , checkbox_hover_file));
      }
   }
   enemy_nmsl_setting.SetupRadioButtonItems(up , down , hover);
   player_nmsl_setting.SetupRadioButtonItems(up , down , hover);
   enemy_tbl_setting.SetupRadioButtonItems(up , down , hover);
   player_tbl_setting.SetupRadioButtonItems(up , down , hover);
   city_left_setting.SetupRadioButtonItems(up , down , hover);
   enemy_mspd_setting.SetupRadioButtonItems(up , down , hover);
   player_mspd_setting.SetupRadioButtonItems(up , down , hover);
   enemy_mrad_setting.SetupRadioButtonItems(up , down , hover);
   player_mrad_setting.SetupRadioButtonItems(up , down , hover);
   enemy_explode_time_setting.SetupRadioButtonItems(up , down , hover);
   player_explode_time_setting.SetupRadioButtonItems(up , down , hover);
   
   alldiff_radio.ClearItems();
   alldiff_radio.SetDrawDimensions((NUM_DIFFICULTIES + 1)*up.W() + NUM_DIFFICULTIES*padding , up.H());
   for (int i = 0 ; i < NUM_DIFFICULTIES ; ++i) {
//   IconButton(std::string name , ICON_DISPLAY_TYPE dtype , BUTTON_SHAPE shape , BTN_ACTION_TYPE atype ,
//               const InputGroup& input , const Rectangle& position , UINT wflags = DEFAULT_FLAGS);
      IconButton* iconbtn = new IconButton(string("alldiff_radio_btn_") + DiffToStr(DIFFICULTY(i)) , ICON_DISPLAY_CENTER , RECTANGLE_BTN , TOGGLE_BTN ,
                                            input_key_press(KEY_NONE) , Rectangle(0 , 0 , up.W() , up.H()));
      iconbtn->SetImages(up , down , hover , 0 , MEMORY , TRANSPARENT);
      alldiff_radio.AddItem(iconbtn , true);
   }
   IconButton* iconbtn = new IconButton(string("alldiff_radio_btn_") + "mixed" , ICON_DISPLAY_CENTER , RECTANGLE_BTN , TOGGLE_BTN ,
                                       input_key_press(KEY_NONE) , Rectangle(0 , 0 , up.W() , up.H()));
   iconbtn->SetImages(up , down , hover , 0 , MEMORY , TRANSPARENT);
   alldiff_radio.AddItem(iconbtn , true);
   SetAlldiffRadio();
}


void Game::SetGuiValues(ConfigSettings cset) {
   current_config_settings = cset;
   Difficulty d = current_config_settings.diffs;
   Config easy = current_config_settings.configs[EASY];
   Config medium = current_config_settings.configs[MEDIUM];
   Config hard = current_config_settings.configs[HARD];
   Config custom = current_config_settings.configs[CUSTOM];
   enemy_nmsl_setting.SetValues(d.enemy_nmsl_diff , easy.enemy_nmsl , medium.enemy_nmsl , hard.enemy_nmsl , custom.enemy_nmsl ,
                                 1 , 1000);
   player_nmsl_setting.SetValues(d.player_nmsl_diff , easy.player_nmsl , medium.player_nmsl , hard.player_nmsl , custom.player_nmsl ,
                                 1 , 1000);
   enemy_tbl_setting.SetValues(d.enemy_tbl_diff , easy.enemy_tbl , medium.enemy_tbl , hard.enemy_tbl , custom.enemy_tbl ,
                                0.01 , 25.0);
   player_tbl_setting.SetValues(d.player_tbl_diff , easy.player_tbl , medium.player_tbl , hard.player_tbl , custom.player_tbl ,
                                 0.01 , 25.0);
   city_left_setting.SetValues(d.city_left_diff , easy.city_left , medium.city_left , hard.city_left , custom.city_left ,
                                0.01 , 1.00);
   enemy_mspd_setting.SetValues(d.enemy_mspd_diff , easy.enemy_mspd , medium.enemy_mspd , hard.enemy_mspd , custom.enemy_mspd ,
                                 1.0 , 1000.0);
   player_mspd_setting.SetValues(d.player_mspd_diff , easy.player_mspd , medium.player_mspd , hard.player_mspd , custom.player_mspd ,
                                  1.0 , 1000.0);
   enemy_mrad_setting.SetValues(d.enemy_mrad_diff , easy.enemy_mrad , medium.enemy_mrad , hard.enemy_mrad , custom.enemy_mrad ,
                                 1 , 500);
   player_mrad_setting.SetValues(d.player_mrad_diff , easy.player_mrad , medium.player_mrad , hard.player_mrad , custom.player_mrad ,
                                  1 , 500);
   enemy_explode_time_setting.SetValues(d.enemy_explode_time_diff , easy.enemy_explode_time , medium.enemy_explode_time , hard.enemy_explode_time , custom.enemy_explode_time ,
                                         0.01 , 30.0 );
   player_explode_time_setting.SetValues(d.player_explode_time_diff , easy.player_explode_time , medium.player_explode_time , hard.player_explode_time , custom.player_explode_time ,
                                          0.01 , 30.0);
   
   nl_radio.SetActiveItem(d.nl_diff);
}
   


void Game::ResetGuiPositions() {
   // 11
   int total_settings_height = 0;
   int setting_text_width = 0;
   int value_text_width = 0;
   SettingWidget* settings[11] = {
      &enemy_nmsl_setting,
      &player_nmsl_setting,
      &enemy_tbl_setting,
      &player_tbl_setting,
      &city_left_setting,
      &enemy_mspd_setting,
      &player_mspd_setting,
      &enemy_mrad_setting,
      &player_mrad_setting,
      &enemy_explode_time_setting,
      &player_explode_time_setting
   };

   for (int i = 0 ; i < 11 ; ++i) {
      total_settings_height += settings[i]->Area().H();
      int stw = settings[i]->SettingTextWidth();
      if (stw > setting_text_width) {setting_text_width = stw;}
      int vtw = settings[i]->ValueTextWidth();
      if (vtw > value_text_width) {value_text_width = vtw;}
   }
   total_settings_height += 10*padding;
   int maxwidth = 0;
   for (int i = 0 ; i < 11 ; ++i) {
      settings[i]->SetColumnWidths(setting_text_width , value_text_width);
      int w = settings[i]->Area().W();
      if (w > maxwidth) {maxwidth = w;}
   }
   int x = (800 - maxwidth)/2;
   int y = (600 - total_settings_height)/2;
   alldiff_radio.SetDrawPos(x + enemy_nmsl_setting.SettingTextWidth() + padding , y - (alldiff_radio.Area().H() + padding));
   alldiff_text.SetTextPosEx(alldiff_radio.Area().X() - padding , alldiff_radio.Area().Y() + alldiff_radio.Area().H()/2 , TEXT_RIGHT , TEXT_VCENTER);
//   Rectangle r = enemy_nmsl_setting.RadioButtonArea(EASY);
   for (int i = 0 ; i < 11 ; ++i) {
      settings[i]->SetDrawPos(x,y);
      y += settings[i]->Area().H() + padding;
   }
   x = player_explode_time_setting.RadioButtonX();
   nl_radio.SetDrawPos(x , y);
   nl_text.SetTextPosEx(nl_radio.Area().X() - padding , y + nl_radio.Area().H()/2 , TEXT_RIGHT , TEXT_VCENTER);
   y += nl_radio.Area().H() + padding;
   
   city_ddl.SetDrawPos(x,y);
   city_text.SetTextPosEx(city_ddl.Area().X() - padding , y + city_ddl.Area().H()/2 , TEXT_RIGHT , TEXT_VCENTER);

   y += city_ddl.Area().H() + padding;

   //revert  save
   //quit    play
   revert_changes_btn.SetDrawPos(x - revert_changes_btn.Area().W() , y);
   quit_btn.SetDrawPos(x - quit_btn.Area().W() , y + revert_changes_btn.Area().H() + padding);
   x = player_explode_time_setting.RadioButtonArea(CUSTOM).BRX() + 1;
   save_changes_btn.SetDrawPos(x , y);
   play_btn.SetDrawPos(x , y + save_changes_btn.Area().H() + padding);
   
}



void Game::SetupGui() {
   const char* nl_btn_text[3] = {"1" , "2" , "3"};
   nl_radio.ClearItems();
   nl_radio.SetDrawDimensions(30*3 + padding*2 , 14);
   for (int i = 0 ; i < 3 ; ++i) {
//   Button(std::string name , BUTTON_SHAPE shape , BTN_ACTION_TYPE atype , FONT* textfont , 
//         std::string label , const InputGroup& input , const Rectangle& position , UINT wflags = DEFAULT_FLAGS);
      Button* btn = new Button(string("nl_radio_btn_") + nl_btn_text[i] , RECTANGLE_BTN , TOGGLE_BTN , font ,
                               nl_btn_text[i] , input_key_press(KEY_NONE) , Rectangle(0,0,30,14));
      nl_radio.AddItem(btn , true);
   }
   SetupRadioButtons();
   SetGuiValues(GetConfigSettings());
   ResetGuiPositions();
   
   SetAlldiffRadio();
   
   city_ddl.ClearItems();
   for (map<string , City*>::iterator it = cities.begin() ; it != cities.end() ; ++it) {
      city_ddl.AddItem(it->first);
   }
   
   menu_gui.SetupBitmapHandlers(BG_SOLID , MEMORY , SOLID);
   
   menu_gui << 
      &enemy_nmsl_setting <<
      &player_nmsl_setting <<
      &enemy_tbl_setting <<
      &player_tbl_setting <<
      &city_left_setting <<
      &enemy_mspd_setting <<
      &player_mspd_setting <<
      &enemy_mrad_setting <<
      &player_mrad_setting <<
      &enemy_explode_time_setting <<
      &player_explode_time_setting <<
      &nl_radio <<
      &nl_text <<
      &alldiff_radio <<
      &alldiff_text <<
      &play_btn <<
      &quit_btn <<
      &save_changes_btn <<
      &revert_changes_btn <<
      &city_ddl <<
      &city_text;
   menu_gui.SetWidgetColors(GetColorConfig());
   
   OutputLog() << endl << menu_gui << endl;
}



Difficulty Game::ReadGuiDifficulty() {
   Difficulty d;
   d.enemy_nmsl_diff = enemy_nmsl_setting.SelectedDifficulty();
   d.player_nmsl_diff = player_nmsl_setting.SelectedDifficulty();
   d.enemy_tbl_diff = enemy_tbl_setting.SelectedDifficulty();
   d.player_tbl_diff = player_tbl_setting.SelectedDifficulty();
   d.nl_diff = (DIFFICULTY)nl_radio.ActiveItemIndex();
   d.city_left_diff = city_left_setting.SelectedDifficulty();
   d.enemy_mspd_diff = enemy_mspd_setting.SelectedDifficulty();
   d.player_mspd_diff = player_mspd_setting.SelectedDifficulty();
   d.enemy_mrad_diff = enemy_mrad_setting.SelectedDifficulty();
   d.player_mrad_diff = player_mrad_setting.SelectedDifficulty();
   d.enemy_explode_time_diff = enemy_explode_time_setting.SelectedDifficulty();
   d.player_explode_time_diff = player_explode_time_setting.SelectedDifficulty();
   return d;
}



Config Game::ReadGuiCustomConfig() {
   Config c;
   c.enemy_nmsl = enemy_nmsl_setting.GetCustomIntegerValue();
   c.player_nmsl = player_nmsl_setting.GetCustomIntegerValue();
   c.enemy_tbl = enemy_tbl_setting.GetCustomFloatValue();
   c.player_tbl = player_tbl_setting.GetCustomFloatValue();
///   c.nl_str = GetConfigSettings().configs[nl_radio.ActiveItemIndex()].nl_str;
   c.city_left = city_left_setting.GetCustomFloatValue();
   c.enemy_mspd = enemy_mspd_setting.GetCustomFloatValue();
   c.player_mspd = player_mspd_setting.GetCustomFloatValue();
   c.enemy_mrad = enemy_mrad_setting.GetCustomIntegerValue();
   c.player_mrad = player_mrad_setting.GetCustomIntegerValue();
   c.enemy_explode_time = enemy_explode_time_setting.GetCustomFloatValue();
   c.player_explode_time = player_explode_time_setting.GetCustomFloatValue();
   return c;
}




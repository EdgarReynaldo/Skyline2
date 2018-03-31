

#include <fstream>
using std::ifstream;

#include <iostream>
using std::ios;
using std::endl;

#include "Game.hpp"
#include "Config.hpp"



volatile bool pausegame = false;

volatile int Game::ticks = 0;

const int BPS = 60;

const float SPT = 1.0f/float(BPS);

void GameTicker() {++Game::ticks;} END_OF_FUNCTION(GameTicker);

string okpointer_file("Data/Images/OkPointer.bmp");
string nopointer_file("Data/Images/NoPointer.bmp");

const char* checkbox_checked_file = "Data/Images/Checkbox_Checked.bmp";
const char* checkbox_unchecked_file = "Data/Images/Checkbox_Unchecked.bmp";
const char* checkbox_hover_file = "Data/Images/Checkbox_Hover.bmp";

const unsigned int TOPIC_SETTING_WIDGET = NextFreeTopicId();


void SettingWidget::ResetPositions() {
   int maxheight = MaxHeight();
   setting_text.SetTextPosEx(setting_text_width - 1, maxheight/2 , TEXT_RIGHT , TEXT_VCENTER);
   int x = setting_text_width + padding;
   difficulty_radio.SetDrawPos(x , (maxheight - difficulty_radio.Area().H())/2);
   x += difficulty_radio.Area().W() + padding;
   value_text.SetTextPosEx(x + value_text_width - 1, maxheight/2 , TEXT_RIGHT , TEXT_VCENTER);
   x += value_text_width + padding;
   input_box.SetDrawPos(x , (maxheight - input_box.Area().H())/2);
   WidgetBase::SetDrawDimensions(setting_text_width + padding + difficulty_radio.Area().W() + padding +
                                 value_text_width + padding + input_box.Area().W() , maxheight);
}



int SettingWidget::MaxHeight() {
   int maxh = setting_text.Area().H();
   int h = difficulty_radio.Area().H();
   if (h > maxh) {maxh = h;}
   h = value_text.Area().H();
   if (h > maxh) {maxh = h;}
   h = input_box.Area().H();
   if (h > maxh) {maxh = h;}
   return maxh;
}



SettingWidget::SettingWidget(const char* description , int pad) :
      WidgetBase(string("SettingWidget : ") + description , Rectangle(0,0,1,1)),
      setting_text(string("description text - ") + description , description , 0 , 0),
      difficulty_radio(string(description) + " - radio" , RADIO_HORIZONTAL , Rectangle(0,0,1,1) , pad),
      value_text(string("value text - ") + description , "" , 0 , 0),
      input_box(string("input box - ") + description , INTEGER_INPUT , font , 7 , 7 , 0 , 0),
      setting_text_width(0),
      value_text_width(0),
      padding(pad),
      ivars(),
      fvars(),
      ival(0),
      fval(0.0f),
      input_type(TEXT_INPUT),
      difficulty(EASY)
{
   setting_text_width = setting_text.Area().W();
   setting_text.SetParent(this);
   difficulty_radio.SetParent(this);
   value_text.SetParent(this);
   input_box.SetParent(this);
   setting_text.UseWidgetColorset(wcols);
   difficulty_radio.UseWidgetColorset(wcols);
   value_text.UseWidgetColorset(wcols);
   input_box.UseWidgetColorset(wcols);
}



WidgetMsg SettingWidget::Update(double tsec) {
   return input_box.Update(tsec);
}



WidgetMsg SettingWidget::CheckInputs(int msx , int msy) {
   int lmsx = msx - area.X();
   int lmsy = msy - area.Y();
   if (flags & HOVER) {
      difficulty_radio.SetHoverState(difficulty_radio.Area().Contains(lmsx , lmsy));
   }
   int msg = difficulty_radio.CheckInputs(lmsx , lmsy).Messages();
   msg |= input_box.CheckInputs(lmsx , lmsy).Messages();
   return WidgetMsg(this , TOPIC_DIALOG , msg);
}



void SettingWidget::DisplayOn(BITMAP* bmp , int x , int y) {
   int lx = area.X();
   int ly = area.Y();
   setting_text.DisplayOn(bmp , x + lx , y + ly);
   difficulty_radio.DisplayOn(bmp , x + lx , y + ly);
   value_text.DisplayOn(bmp , x + lx , y + ly);
   input_box.DisplayOn(bmp , x + lx , y + ly);
   ClearRedrawFlag();
}



void SettingWidget::QueueUserMessage(const WidgetMsg& wmsg) {
   const WidgetMsg radio_click(&difficulty_radio , TOPIC_RADIO_BUTTON , RADIO_BTN_CLICKED);
   const WidgetMsg input_change(&input_box , TOPIC_TEXT_INPUT , TEXT_VALUE_ENTERED);
   WidgetMsg qmsg = wmsg;
   if (wmsg == radio_click) {
      difficulty = (DIFFICULTY)difficulty_radio.ActiveItemIndex();
      switch (input_type) {
         case INTEGER_INPUT :
            ival = ivars[difficulty];
            input_box.SetIntegerValue(ival);
            value_text.SetText(ival);
            break;
         case FLOATING_POINT_INPUT :
            fval = fvars[difficulty];
            input_box.SetFloatValue(fval);
            value_text.SetText(fval , 8 , 2);
            break;
         default :
            throw EagleError("Setting widget input type is wrong wrong wrong...\n");
      }
      if (difficulty == CUSTOM) {
         input_box.ShowWidget();
      }
      else {
         input_box.HideWidget();
      }
//      SetBgRedrawFlag();
      qmsg = WidgetMsg(this , TOPIC_SETTING_WIDGET , DIFFICULTY_SELECTION_CHANGED);
   }
   else if (wmsg == input_change) {
      switch (input_type) {
         case INTEGER_INPUT :
            ivars[CUSTOM] = input_box.GetIntValue();
            ival = ivars[CUSTOM];
            value_text.SetText(ival);
            break;
         case FLOATING_POINT_INPUT :
            fvars[CUSTOM] = input_box.GetFloatValue();
            fval = fvars[CUSTOM];
            value_text.SetText(fval , 8 , 2);
            break;
         default :
            throw EagleError("Setting widget input type is wrong wrong wrong...\n");
      }
//      SetBgRedrawFlag();
      qmsg = WidgetMsg(this , TOPIC_SETTING_WIDGET , CUSTOM_VALUE_CHANGED);
   }
   WidgetBase::QueueUserMessage(qmsg);
}



void SettingWidget::SetWidgetColors(const WidgetColorset& colors) {
   WidgetBase::SetWidgetColors(colors);
   setting_text.UseWidgetColorset(wcols);
   difficulty_radio.UseWidgetColorset(wcols);
   value_text.UseWidgetColorset(wcols);
   input_box.UseWidgetColorset(wcols);
}



void SettingWidget::UseWidgetColorset(WidgetColorset* colorset) {
   WidgetBase::UseWidgetColorset(colorset);
   setting_text.UseWidgetColorset(colorset);
   difficulty_radio.UseWidgetColorset(colorset);
   value_text.UseWidgetColorset(colorset);
   input_box.UseWidgetColorset(colorset);
}



void SettingWidget::UseIndividualColorset() {
   WidgetBase::UseIndividualColorset();
   setting_text.UseWidgetColorset(wcols);
   difficulty_radio.UseWidgetColorset(wcols);
   value_text.UseWidgetColorset(wcols);
   input_box.UseWidgetColorset(wcols);
}



void SettingWidget::SetDrawPos(int xpos , int ypos) {
   WidgetBase::SetDrawPos(xpos,ypos);
}



void SettingWidget::SetDrawDimensions(int width , int height) {
   // does nothing
}



void SettingWidget::SetArea(int xpos , int ypos , int width , int height) {
   SetDrawPos(xpos,ypos);
}



void SettingWidget::SetArea(const Rectangle& r) {
   SetDrawPos(r.X() , r.Y());
}



void SettingWidget::SetHoverState(bool state) {
   WidgetBase::SetHoverState(state);
   if (!state) {
      difficulty_radio.SetHoverState(false);
   }
   SetRedrawFlag();
}



void SettingWidget::SetFocusState(bool state) {
   WidgetBase::SetFocusState(state);
   input_box.SetFocusState(state);
}



void SettingWidget::SetColumnWidths(int setting_text_box_width , int value_text_box_width) {
   int stw = setting_text_box_width;
   int vtw = value_text_box_width;
   if (stw < setting_text.Area().W()) {stw = setting_text.Area().W();}
   if (vtw < value_text.Area().W()) {stw = value_text.Area().W();}
   setting_text_width = stw;
   value_text_width = vtw;
   ResetPositions();
}



void SettingWidget::SetupRadioButtonItems(BITMAP* btn_up , BITMAP* btn_down , BITMAP* btn_hover) {
//   bool LoadImages(const char* up_pic_file , const char* down_pic_file , const char* hover_pic_file , const char* disabled_pic_file ,
//                     BMP_MEM_TYPE bmp_mem_type , BMP_DRAW_TYPE bmp_draw_type);
//   IconRadioButtonItem(std::string text_label , std::string name);
   ASSERT(btn_up);
   ASSERT(btn_down);
   difficulty_radio.ClearItems();
   difficulty_radio.SetDrawDimensions(NUM_DIFFICULTIES*btn_up->w + (NUM_DIFFICULTIES - 1)*padding , btn_up->h);
   for (int i = 0 ; i < NUM_DIFFICULTIES ; ++i) {
//IconButton::IconButton(std::string name , ICON_DISPLAY_TYPE dtype , Rectangle position , UINT wflags) :
      IconButton* iconbtn = new IconButton(setting_text.Text() + "_iconbtn_" + DiffToStr(DIFFICULTY(i)) ,
                                           ICON_DISPLAY_CENTER , Rectangle(0 , 0 , btn_up->w , btn_up->h) , DEFAULT_FLAGS);
      iconbtn->SetImages(btn_up , btn_down , btn_hover , 0 , MEMORY , TRANSPARENT);
      difficulty_radio.AddItem(iconbtn , true);
   }
   difficulty_radio.SetActiveItem(UINT(difficulty));
   ResetPositions();
}



void SettingWidget::SetValues(DIFFICULTY d , int easy , int medium , int hard , int custom , int min , int max) {
   input_type = INTEGER_INPUT;

   if (max    < min) {max    = min;}

   if (easy   < min) {easy   = min;}
   if (medium < min) {medium = min;}
   if (hard   < min) {hard   = min;}
   if (custom < min) {custom = min;}
   if (easy   > max) {easy   = max;}
   if (medium > max) {medium = max;}
   if (hard   > max) {hard   = max;}
   if (custom > max) {custom = max;}

   difficulty = d;

   ivars[EASY] = easy;
   ivars[MEDIUM] = medium;
   ivars[HARD] = hard;
   ivars[CUSTOM] = custom;
   ival = ivars[difficulty];
   input_box.SetIntegerRange(min , ival , max);
   value_text.SetText(input_box.GetIntValue());
   value_text_width = value_text.Area().W();

   difficulty_radio.SetActiveItem(UINT(difficulty));
   
   if (difficulty == CUSTOM) {
      input_box.ShowWidget();
   }
   else {
      input_box.HideWidget();
   }
}



void SettingWidget::SetValues(DIFFICULTY d , float easy , float medium , float hard , float custom , float min , float max) {
   input_type = FLOATING_POINT_INPUT;

   if (max    < min) {max    = min;}

   if (easy   < min) {easy   = min;}
   if (medium < min) {medium = min;}
   if (hard   < min) {hard   = min;}
   if (custom < min) {custom = min;}
   if (easy   > max) {easy   = max;}
   if (medium > max) {medium = max;}
   if (hard   > max) {hard   = max;}
   if (custom > max) {custom = max;}

   difficulty = d;

   fvars[EASY] = easy;
   fvars[MEDIUM] = medium;
   fvars[HARD] = hard;
   fvars[CUSTOM] = custom;
   fval = fvars[difficulty];
   input_box.SetFloatRange(min , fval , max);
   value_text.SetText(input_box.GetFloatValue() , 8 , 2);
   value_text_width = value_text.Area().W();
   
   difficulty_radio.SetActiveItem(UINT(difficulty));
   
   if (difficulty == CUSTOM) {
      input_box.ShowWidget();
   }
   else {
      input_box.HideWidget();
   }
}



void SettingWidget::SetDifficulty(DIFFICULTY d) {
   difficulty = d;
   difficulty_radio.SetActiveItem((UINT)d);
   switch (input_type) {
      case INTEGER_INPUT :
         ival = ivars[d];
         input_box.SetIntegerValue(ival);
         value_text.SetText(input_box.GetIntValue());
         ivars[d] = ival = input_box.GetIntValue();
         break;
      case FLOATING_POINT_INPUT :
         fval = fvars[d];
         input_box.SetFloatValue(fval);
         value_text.SetText(input_box.GetFloatValue() , 8 , 2);
         fvars[d] = fval = input_box.GetFloatValue();
         break;
      case TEXT_INPUT :
         throw EagleError("Huh? Why is the SettingWidget's input_type equal to TEXT_INPUT?\n");
         break;
   }
   if (difficulty == CUSTOM) {
      input_box.ShowWidget();
   }
   else {
      input_box.HideWidget();
   }
}



void SettingWidget::SetCustomValue(int value) {
   ASSERT(input_type == INTEGER_INPUT);
   if (value < input_box.GetIntMin()) {value = input_box.GetIntMin();}
   if (value > input_box.GetIntMax()) {value = input_box.GetIntMax();}
   ivars[CUSTOM] = value;
   if (difficulty == CUSTOM) {
      ival = value;
      input_box.SetIntegerValue(ival);
      value_text.SetText(ival);
   }
}



void SettingWidget::SetCustomValue(float value) {
   ASSERT(input_type == FLOATING_POINT_INPUT);
   if (value < input_box.GetFloatMin()) {value = input_box.GetFloatMin();}
   if (value > input_box.GetFloatMax()) {value = input_box.GetFloatMax();}
   fvars[CUSTOM] = value;
   if (difficulty == CUSTOM) {
      fval = value;
      input_box.SetFloatValue(fval);
      value_text.SetText(fval , 8 , 2);
   }
}



Rectangle SettingWidget::RadioButtonArea(DIFFICULTY d) {
   Rectangle r = difficulty_radio.ItemIndexToButton(d)->Area();
   r.MoveBy(area.X() , area.Y());
   return r;
}



int SettingWidget::GetCustomIntegerValue() {
   ASSERT(input_type == INTEGER_INPUT);
   return ivars[CUSTOM];
}



float SettingWidget::GetCustomFloatValue() {
   ASSERT(input_type == FLOATING_POINT_INPUT);
   return fvars[CUSTOM];
}



void SettingWidget::PrintTo(std::ostream& os , Indenter& indent) const {
   os << indent << "Setting widget {" << endl;
   ++indent;
   PrintBaseTo(os , indent);
   os << indent << "setting_text_width = " << setting_text_width << " , value_text_width = " << value_text_width << " , padding = " << padding << endl;
   os << indent << "input_type = " << ToString(input_type) << " , difficulty = " << DiffToStr(difficulty) << endl;
   os << indent << "ivars = " << StringPrintF("%i,%i,%i,%i" , ivars[EASY] , ivars[MEDIUM] , ivars[HARD] , ivars[CUSTOM]) << endl;
   os << indent << "fvars = " << StringPrintF("%f,%f,%f,%f" , fvars[EASY] , fvars[MEDIUM] , fvars[HARD] , fvars[CUSTOM]) << endl;
   setting_text.PrintTo(os , indent);
   difficulty_radio.PrintTo(os , indent);
   value_text.PrintTo(os , indent);
   input_box.PrintTo(os , indent);
   --indent;
   os << indent << "}" << endl;
}



std::ostream& operator<<(std::ostream& os , const SettingWidget& sw) {
   Indenter i(0,3);
   sw.PrintTo(os , i);
   return os;
}



/// ----------------------------------     Game class     -------------------------------------------



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



void Game::SetupMissileBatteries(Config c) {
   FreeMissileBatteries();
   const char* cstr = c.nl_str.c_str();
   int count = 0;
   int nlaunchers = 0;
   int ret = 0;
   ret = sscanf(cstr , "%i%n" , &nlaunchers , &count);
///   OutputLog() << StringPrintF("Num launchers scan returned %i items.\n" , ret);
   if (1 != ret) {
      throw EagleError(StringPrintF("Failed to read number of enemy launchers from '%s'\n" , cstr));
   }
   cstr += count;
   int x = 0;
   int y = 0;
   int w = 0;
   int h = 0;
   for (int i = 0 ; i < nlaunchers ; ++i) {
      ret = sscanf(cstr , "%i,%i,%i,%i%n" , &x , &y , &w , &h , &count);
      if (4 != ret) {
         throw EagleError(StringPrintF("Failed to read launcher #%i's zone from '%s'.\n" , i + 1 , cstr));
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
   player_ai = new PlayerAI(Rectangle(400,599,1,1) , c.player_nmsl , c);
   player_mb.SetAI(player_ai);
}



void Game::DrawGame(BITMAP* bmp) {
   blit(background , buffer , 0 , 0 , 0 , 0 , background.W() , background.H());
   city->Display(buffer);
   for (unsigned int i = 0 ; i < enemy_mbs.size() ; ++i) {
      enemy_mbs[i]->Display(buffer);
   }
   player_mb.Display(buffer);
   if (state == WIN) {
      textout_centre_ex(buffer , font , "WIN! WIN! WIN! WIN! WIN!" , 400 , 296 , makecol(0,255,0) , -1);
   }
   else if (state == LOSE) {
      textout_centre_ex(buffer , font , "YOU LOSE! LOSE! LOSE! LOSE! LOSE!" , 400 , 296 , makecol(255,0,0) , -1);
   }
   pointer->DrawTo(buffer , mx - pointer->W()/2 , my - pointer->H()/2);
   blit(buffer , bmp , 0 , 0 , 0 , 0 , buffer.W() , buffer.H());
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
   for (unsigned int i = 0 ; i < missiles.size() ; ++i) {
      Missile* m1 = missiles[i];
      int x1 = m1->X();
      int y1 = m1->Y();
      int r1 = m1->CRad();
//      MISSILE_STATE s1 = m1->State();
      bool e1 = m1->Exploding();
      if (e1) {city->Destroy(x1,y1,r1);}
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
   /// Check for enemy missiles hitting the city
   for (unsigned int i = 0 ; i < emissiles.size() ; ++i) {
      Missile* m = emissiles[i];
      if (city->Hit(m->X() , m->Y())) {
         m->Explode();
      }
   }

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



Game::Game(string cityfile , int screenw , int screenh , int pad) :
   cities(),
   citystr(),
   city(0),
   background(screenw , screenh , MEMORY , SOLID),
   buffer(screenw , screenh , MEMORY , SOLID),
   nopointer(&resource_registry),
   okpointer(&resource_registry),
   pointer(&nopointer),
   state(INTRO),
   difficulty(EASY),
   status(0),
   redraw(true),
   mousex(0),
   mousey(0),
   enemy_mbs(),
   player_mb(),
   player_ai(0),
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
   last_config_settings(GetConfigSettings()),
   current_config_settings(GetConfigSettings()),
   current_config(GetConfig()),
   config_changed(false)
{
   SetupCities(cityfile , screenw , screenh);
   LOCK_FUNCTION(GameTicker);
   LOCK_VARIABLE(ticks);
   install_int_ex(GameTicker , BPS_TO_TIMER(BPS));
   background.ClearTo(makecol(0,0,0));
   buffer.ClearTo(makecol(0,0,0));

   okpointer_file = get_config_string("MOUSE" , "okpointer" , okpointer_file.c_str());
   nopointer_file = get_config_string("MOUSE" , "nopointer" , nopointer_file.c_str());
//   bool LoadImage(const char* filepath , RGB* pal , BMP_MEM_TYPE bmp_mem_type , BMP_DRAW_TYPE bmp_draw_type);
   if (!okpointer.LoadImage(okpointer_file.c_str() , 0 , MEMORY , TRANSPARENT)) {
      throw EagleError(StringPrintF("Failed to load okay pointer (%s)!\n" , okpointer_file.c_str()));
   }
   if (!nopointer.LoadImage(nopointer_file.c_str() , 0 , MEMORY , TRANSPARENT)) {
      throw EagleError(StringPrintF("Failed to load no pointer (%s)!\n" , nopointer_file.c_str()));
   }
   srand(current_config_settings.seed);

   checkbox_checked_file = get_config_string("GUI" , "checkbox_checked_file" , checkbox_checked_file);
   checkbox_unchecked_file = get_config_string("GUI" , "checkbox_unchecked_file" , checkbox_unchecked_file);
   checkbox_hover_file = get_config_string("GUI" , "checkbox_hover_file" , checkbox_hover_file);
   
   SetupGui();
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
      throw EagleError(StringPrintF("Could not open game setup file (%s)!\n" , file.c_str()));
   }
   int count = 0;
   while (!strm.eof()) {
      string line;
      GetLinePN(strm , line);
      OutputLog() << StringPrintF("line read for city = '%s'" , line.c_str()) << std::endl;
      if (line.size() == 0) {continue;}
      ++count;
      unsigned int index = line.find_first_of(',');
      if (index == string::npos) {
         throw EagleError("No comma detected. Bad city file format.\nShould be 'cityname,path/to/city.bmp'\n");
      }
      string cityname = line.substr(0 , index);
      string path = line.substr(index + 1 , string::npos);
      cities[cityname] = new City(cityname , path , screenw , screenh);
   }
   if (!count) {
      throw EagleError("No cities detected!\n");
   }
}



int Game::Run() {
   ticks = 1;
   show_mouse(screen);
   while (1) {
      Display(screen);
/*
      scare_mouse();
      acquire_screen();
      Display(screen);
      release_screen();
      unscare_mouse();
*/
      while (ticks < 1) {rest(1);}
      ticks = 0;
      if (Update(SPT) == QUIT) {break;}
      UpdateInputHandler(SPT);
      if (CheckInputs(mouse_x , mouse_y) == QUIT) {break;}
   }
   return status;
}



STATE Game::Update(double dt) {
   if (pausegame) {
      while (pausegame) {rest(50);}
      ticks = 0;
   }
   switch (state) {
      case INTRO :
         state = MENUE;
         break;
      case MENUE :
         menu_gui.Update(dt);
         break;
      case GAME :
         for (unsigned int i = 0 ; i < enemy_mbs.size() ; ++i) {
            enemy_mbs[i]->Update(dt);
         }
         player_mb.Update(dt);
         CheckCollisions();
         state = CheckGameState();
         redraw = true;
         break;
      case WIN :
         break;
      case LOSE :
         break;
      case QUIT :
         break;
   }
   return state;
}

STATE Game::CheckInputs(int msx , int msy) {
   switch (state) {
      case INTRO :
         state=MENUE;
         if (AnyKeysPressed()) {
            state = MENUE;
            show_mouse(screen);
         }
         break;
      case MENUE :
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
         
         
         break;
      case GAME :
         for (unsigned int i = 0 ; i < enemy_mbs.size() ; ++i) {
            MissileBattery* mbtry = enemy_mbs[i];
            mbtry->CheckInputs(msx,msy);
         }
         player_mb.CheckInputs(msx,msy);
         if (player_mb.Ready()) {
            pointer = &okpointer;
         }
         else {
            pointer = &nopointer;
         }
         if (input_key_press(KEY_ESC)) {state = MENUE;}
         break;
      case WIN :
         if (AnyKeysPressed()) {state = MENUE;}
         break;
      case LOSE :
         if (AnyKeysPressed()) {state = MENUE;}
         break;
      case QUIT :
         break;
   }
   return state;
}

void Game::Display(BITMAP* bmp) {
   redraw = true;/// TODO : Remove
   if (redraw) {
      switch (state) {
         case INTRO :
            clear_to_color(bmp , makecol(0,0,0));
            break;
         case MENUE :
            menu_gui.DisplayOn(bmp , 0 , 0);
/*
            {
               clear_to_color(buffer , makecol(0,0,0));
               const char* text[4] = {
                  "ESC : Quit",
                  "1   : Easy",
                  "2   : Medium",
                  "3   : Hard"
               };
               for (int i = 0 ; i < 4 ; ++i) {
                  textout_ex(buffer , font , text[i] , 350 , 250 + i*16 , makecol(0,255,0) , -1);
               }
               blit(buffer , bmp , 0 , 0 , 0 , 0 , buffer.W() , buffer.H());
            }
*/
            break;
         case GAME :
            DrawGame(bmp);
            break;
         case WIN :
            DrawGame(bmp);
            break;
         case LOSE :
            DrawGame(bmp);
            break;
         case QUIT :
            break;
      }
   }
//      redraw = false;
}


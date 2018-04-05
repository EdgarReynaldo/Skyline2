



#ifndef SettingsScreen_HPP
#define SettingsScreen_HPP


class SettingsScreen {

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




   void SetAlldiffRadio();
   void SetupRadioButtons();
   void SetGuiValues(ConfigSettings cset);
   void ResetGuiPositions();
   void SetupGui();

   Difficulty ReadGuiDifficulty();
   Config ReadGuiCustomConfig();
};


#endif // SettingsScreen_HPP




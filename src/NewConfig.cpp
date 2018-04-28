



#include "NewConfig.hpp"

#include "Eagle/Exception.hpp"
#include "Eagle/StringWork.hpp"
#include "Eagle/FileSystem.hpp"

#include <cstring>



static const char* setting_names[NUM_GAME_SETTINGS] = {
   "enemy_num_missiles",
   "enemy_time_between_launch",
   "enemy_missile_speed",
   "enemy_missile_exp_radius",
   "enemy_missile_exp_duration",
   "enemy_launcher_string",
   "player_num_missiles",
   "player_time_between_launch",
   "player_missile_speed",
   "player_missile_exp_radius",
   "player_missile_exp_duration",
   "player_launcher_string",
   "player_laser_color_string",
   "player_laser_blend_mode_string",
   "player_laser_width",
   "player_laser_duration",
   "player_percent_city_left",
   "player_shield_depth"
};



static const char* diffstrs[NUM_DIFFICULTIES] = {
   "easy",
   "medium",
   "hard",
   "insane",
   "custom"
};

std::string DiffToStr(DIFFICULTY d) {
   EAGLE_ASSERT(d != NUM_DIFFICULTIES);
   return diffstrs[d];
}



DIFFICULTY StrToDiff(const char* str) {
   std::string s(str);
   for (char& c : s) {
      if (isupper(c)) {c = tolower(c);}
   }
   for (int i = 0 ; i < NUM_DIFFICULTIES ; ++i) {
      if (s.compare(diffstrs[i]) == 0) {
         return DIFFICULTY(i);
      }
   }
   return NUM_DIFFICULTIES;
}



std::string GetConfigKeyName(GAME_SETTING setting , DIFFICULTY d) {
   std::string str = setting_names[setting];
   str += "_";
   str += DiffToStr(d);
   return str;
}




void GameConfig::SetupDefaultConfig() {
   ConfigFile& cf = config_file;
   cf.Clear();

   cf["RAND"]["seed"] = "232";
   
   ConfigSection& cs = cf["AI"];
   
   /// Enemy

   cs.AddSpacer();

   cs.AddComment("Adjust the number of enemy missiles here");
   cs[GetConfigKeyName(GSE_NMSL , EASY  )] = "45";
   cs[GetConfigKeyName(GSE_NMSL , MEDIUM)] = "90";
   cs[GetConfigKeyName(GSE_NMSL , HARD  )] = "180";
   cs[GetConfigKeyName(GSE_NMSL , INSANE)] = "250";
   cs[GetConfigKeyName(GSE_NMSL , CUSTOM)] = "90";
   cs.AddSpacer();

   cs.AddComment("Enemy time between launches");
   cs[GetConfigKeyName(GSE_TBL , EASY  )] = "3.0";
   cs[GetConfigKeyName(GSE_TBL , MEDIUM)] = "2.5";
   cs[GetConfigKeyName(GSE_TBL , HARD  )] = "1.75";
   cs[GetConfigKeyName(GSE_TBL , INSANE)] = "1.5";
   cs[GetConfigKeyName(GSE_TBL , CUSTOM)] = "2.5";
   cs.AddSpacer();

   cs.AddComment("Enemy missile speed");
   cs[GetConfigKeyName(GSE_MSPD , EASY  )] = "50";
   cs[GetConfigKeyName(GSE_MSPD , MEDIUM)] = "70";
   cs[GetConfigKeyName(GSE_MSPD , HARD  )] = "90";
   cs[GetConfigKeyName(GSE_MSPD , INSANE)] = "110";
   cs[GetConfigKeyName(GSE_MSPD , CUSTOM)] = "70";
   cs.AddSpacer();
   
   cs.AddComment("Enemy missile radius");
   cs[GetConfigKeyName(GSE_MRAD , EASY  )] = "20";
   cs[GetConfigKeyName(GSE_MRAD , MEDIUM)] = "40";
   cs[GetConfigKeyName(GSE_MRAD , HARD  )] = "60";
   cs[GetConfigKeyName(GSE_MRAD , INSANE)] = "80";
   cs[GetConfigKeyName(GSE_MRAD , CUSTOM)] = "40";
   cs.AddSpacer();

   cs.AddComment("Enemy explosion time");
   cs[GetConfigKeyName(GSE_ETIME , EASY  )] = "4.0";
   cs[GetConfigKeyName(GSE_ETIME , MEDIUM)] = "2.0";
   cs[GetConfigKeyName(GSE_ETIME , HARD  )] = "0.5";
   cs[GetConfigKeyName(GSE_ETIME , INSANE)] = "0.25";
   cs[GetConfigKeyName(GSE_ETIME , CUSTOM)] = "2.0";
   cs.AddSpacer();

   
   cs.AddComment("Enemy launcher string : #launchers followed by fraction x positions (# [0.0,1.0] [0.0,1.0] ...");
   cs[GetConfigKeyName(GSE_NLSTR , EASY  )] = "1 0.5";
   cs[GetConfigKeyName(GSE_NLSTR , MEDIUM)] = "2 0.25 0.75";
   cs[GetConfigKeyName(GSE_NLSTR , HARD  )] = "3 0.25 0.5 0.75";
   cs[GetConfigKeyName(GSE_NLSTR , INSANE)] = "4 0.15 0.35 0.65 0.85";
   cs[GetConfigKeyName(GSE_NLSTR , CUSTOM)] = "2 0.25 0.75";
   cs.AddSpacer();

   /// Player
   
   cs.AddComment("Player number of missiles");
   cs[GetConfigKeyName(GSP_NMSL , EASY  )] = "80";
   cs[GetConfigKeyName(GSP_NMSL , MEDIUM)] = "180";
   cs[GetConfigKeyName(GSP_NMSL , HARD  )] = "360";
   cs[GetConfigKeyName(GSP_NMSL , INSANE)] = "1000";
   cs[GetConfigKeyName(GSP_NMSL , CUSTOM)] = "180";
   cs.AddSpacer();

   cs.AddComment("Player time between launches");
   cs[GetConfigKeyName(GSP_TBL , EASY  )] = "0.25";
   cs[GetConfigKeyName(GSP_TBL , MEDIUM)] = "0.5";
   cs[GetConfigKeyName(GSP_TBL , HARD  )] = "0.5";
   cs[GetConfigKeyName(GSP_TBL , INSANE)] = "0.75";
   cs[GetConfigKeyName(GSP_TBL , CUSTOM)] = "0.5";
   cs.AddSpacer();

   cs.AddComment("Player missile speed");
   cs[GetConfigKeyName(GSP_MSPD , EASY  )] = "250";
   cs[GetConfigKeyName(GSP_MSPD , MEDIUM)] = "200";
   cs[GetConfigKeyName(GSP_MSPD , HARD  )] = "150";
   cs[GetConfigKeyName(GSP_MSPD , INSANE)] = "100";
   cs[GetConfigKeyName(GSP_MSPD , CUSTOM)] = "200";
   cs.AddSpacer();
   
   cs.AddComment("Player missile radius");
   cs[GetConfigKeyName(GSP_MRAD , EASY  )] = "100";
   cs[GetConfigKeyName(GSP_MRAD , MEDIUM)] = "80";
   cs[GetConfigKeyName(GSP_MRAD , HARD  )] = "60";
   cs[GetConfigKeyName(GSP_MRAD , INSANE)] = "40";
   cs[GetConfigKeyName(GSP_MRAD , CUSTOM)] = "80";
   cs.AddSpacer();

   cs.AddComment("Player explosion time");
   cs[GetConfigKeyName(GSP_ETIME , EASY  )] = "5.0";
   cs[GetConfigKeyName(GSP_ETIME , MEDIUM)] = "3.5";
   cs[GetConfigKeyName(GSP_ETIME , HARD  )] = "2.0";
   cs[GetConfigKeyName(GSP_ETIME , INSANE)] = "1.0";
   cs[GetConfigKeyName(GSP_ETIME , CUSTOM)] = "3.5";
   cs.AddSpacer();

   cs.AddComment("Player launcher string : #number of missiles per spread");
   cs[GetConfigKeyName(GSP_NLSTR , EASY  )] = "1";
   cs[GetConfigKeyName(GSP_NLSTR , MEDIUM)] = "3";
   cs[GetConfigKeyName(GSP_NLSTR , HARD  )] = "5";
   cs[GetConfigKeyName(GSP_NLSTR , INSANE)] = "7";
   cs[GetConfigKeyName(GSP_NLSTR , CUSTOM)] = "3";
   cs.AddSpacer();

   /// Laser
   
   cs.AddComment("Player laser color : valid values are 'RGB' 'CMY' or 'WWW'");
   cs[GetConfigKeyName(GSP_LCOLOR , EASY  )] = "RGB";
   cs[GetConfigKeyName(GSP_LCOLOR , MEDIUM)] = "RGB";
   cs[GetConfigKeyName(GSP_LCOLOR , HARD  )] = "CMY";
   cs[GetConfigKeyName(GSP_LCOLOR , INSANE)] = "CMY";
   cs[GetConfigKeyName(GSP_LCOLOR , CUSTOM)] = "RGB";
   cs.AddSpacer();

   cs.AddComment("Player laser blend mode : valid values are 'SOLID' or 'BLEND'");
   cs[GetConfigKeyName(GSP_LBLEND , EASY  )] = "SOLID";
   cs[GetConfigKeyName(GSP_LBLEND , MEDIUM)] = "BLEND";
   cs[GetConfigKeyName(GSP_LBLEND , HARD  )] = "SOLID";
   cs[GetConfigKeyName(GSP_LBLEND , INSANE)] = "BLEND";
   cs[GetConfigKeyName(GSP_LBLEND , CUSTOM)] = "SOLID";
   cs.AddSpacer();

   cs.AddComment("Player laser width");
   cs[GetConfigKeyName(GSP_LWIDTH , EASY  )] = "40";
   cs[GetConfigKeyName(GSP_LWIDTH , MEDIUM)] = "30";
   cs[GetConfigKeyName(GSP_LWIDTH , HARD  )] = "20";
   cs[GetConfigKeyName(GSP_LWIDTH , INSANE)] = "10";
   cs[GetConfigKeyName(GSP_LWIDTH , CUSTOM)] = "30";
   cs.AddSpacer();

   cs.AddComment("Player laser duration");
   cs[GetConfigKeyName(GSP_LDURATION , EASY  )] = "1.5";
   cs[GetConfigKeyName(GSP_LDURATION , MEDIUM)] = "1.0";
   cs[GetConfigKeyName(GSP_LDURATION , HARD  )] = "0.5";
   cs[GetConfigKeyName(GSP_LDURATION , INSANE)] = "0.25";
   cs[GetConfigKeyName(GSP_LDURATION , CUSTOM)] = "1.0";
   cs.AddSpacer();
   
   cs.AddComment("Percentage of city left to win");
   cs[GetConfigKeyName(GSP_CITY_LEFT , EASY  )] = "0.1";
   cs[GetConfigKeyName(GSP_CITY_LEFT , MEDIUM)] = "0.3";
   cs[GetConfigKeyName(GSP_CITY_LEFT , HARD  )] = "0.5";
   cs[GetConfigKeyName(GSP_CITY_LEFT , INSANE)] = "0.6";
   cs[GetConfigKeyName(GSP_CITY_LEFT , CUSTOM)] = "0.3";
   cs.AddSpacer();
   
   cs.AddComment("Depth of shield");
   cs[GetConfigKeyName(GSP_SHIELD_DEPTH , EASY  )] = "50";
   cs[GetConfigKeyName(GSP_SHIELD_DEPTH , MEDIUM)] = "80";
   cs[GetConfigKeyName(GSP_SHIELD_DEPTH , HARD  )] = "110";
   cs[GetConfigKeyName(GSP_SHIELD_DEPTH , INSANE)] = "150";
   cs[GetConfigKeyName(GSP_SHIELD_DEPTH , CUSTOM)] = "80";
   cs.AddSpacer();
   
   
}



void GameConfig::ReadStoredConfig() {
   for (DIFFICULTY d = EASY ; d < NUM_DIFFICULTIES ; d = (DIFFICULTY)((int)d + 1)) {
      Config* c = &configs[d];
      ConfigFile& cf = config_file;
      ConfigSection& cs = cf["AI"];
      
      /// Enemy
      c->enemy_nmsl = std::stoi(cs[GetConfigKeyName(GSE_NMSL , d)]);
      c->enemy_tbl  = std::stof(cs[GetConfigKeyName(GSE_TBL , d)]);
      c->enemy_mspd = std::stof(cs[GetConfigKeyName(GSE_MSPD , d)]);
      c->enemy_mrad = std::stoi(cs[GetConfigKeyName(GSE_MRAD , d)]);
      c->enemy_explode_time = std::stof(cs[GetConfigKeyName(GSE_ETIME , d)]);
      c->enemy_nl_str = cs[GetConfigKeyName(GSE_NLSTR , d)];

      /// Player
      c->player_nmsl         = std::stoi(cs[GetConfigKeyName(GSP_NMSL , d)]);
      c->player_tbl          = std::stof(cs[GetConfigKeyName(GSP_TBL , d)]);
      c->player_mspd         = std::stof(cs[GetConfigKeyName(GSP_MSPD , d)]);
      c->player_mrad         = std::stoi(cs[GetConfigKeyName(GSP_MRAD , d)]);
      c->player_explode_time = std::stof(cs[GetConfigKeyName(GSP_ETIME , d)]);
      c->player_nl_str       = cs[GetConfigKeyName(GSP_NLSTR , d)];
      
      /// Laser
      c->laser_color      = cs[GetConfigKeyName(GSP_LCOLOR , d)];
      c->laser_blend_mode = cs[GetConfigKeyName(GSP_LBLEND , d)];
      c->laser_width      = std::stof(cs[GetConfigKeyName(GSP_LWIDTH , d)]);
      c->laser_duration   = std::stof(cs[GetConfigKeyName(GSP_LDURATION , d)]);
      
      /// City
      c->city_left    = std::stof(cs[GetConfigKeyName(GSP_CITY_LEFT , d)]);
      c->shield_depth = std::stoi(cs[GetConfigKeyName(GSP_SHIELD_DEPTH , d)]);
      
   }
}



GameConfig::GameConfig() :
      config_file(),
      configs(),
      diffs()
{
   memset(configs , 0 , sizeof(Config)*NUM_DIFFICULTIES);
   memset(diffs , 0 , sizeof(DIFFICULTY)*NUM_GAME_SETTINGS);
}



bool GameConfig::LoadConfig() {
   const char* fpath = "Data/Config.txt";
   FSInfo finfo = GetFileInfo(fpath);
   if (!finfo.Exists()) {
      SetupDefaultConfig();
      config_file.SaveToFile(fpath);
   }

   config_file.Clear();
   bool load = config_file.LoadFromFile(fpath);
   if (!load) {
      EagleCritical() << StringPrintF("Could not load config from file '%s'" , fpath) << std::endl;
   }
   config_file.SaveToFile("Data/Config2.txt");

   ReadStoredConfig();

   return true;
}



void GameConfig::SetDifficulty(GAME_SETTING gs , DIFFICULTY d) {
   diffs[gs] = d;
}



void GameConfig::SetOverallDifficulty(DIFFICULTY d) {
   EAGLE_ASSERT(d != NUM_DIFFICULTIES);
   for (int i = 0 ; i < NUM_GAME_SETTINGS ; ++i) {
      diffs[i] = d;
   }
}



Config GameConfig::GetSelectedConfig() {
   Config c;
   c.enemy_nmsl          = configs[diffs[GSE_NMSL]].enemy_nmsl;
   c.enemy_tbl           = configs[diffs[GSE_TBL]].enemy_tbl;
   c.enemy_mspd          = configs[diffs[GSE_MSPD]].enemy_mspd;
   c.enemy_mrad          = configs[diffs[GSE_MRAD]].enemy_mrad;
   c.enemy_explode_time  = configs[diffs[GSE_ETIME]].enemy_explode_time;
   c.enemy_nl_str        = configs[diffs[GSE_NLSTR]].enemy_nl_str;
   c.player_nmsl         = configs[diffs[GSP_NMSL]].player_nmsl;
   c.player_tbl          = configs[diffs[GSP_TBL]].player_tbl;
   c.player_mspd         = configs[diffs[GSP_MSPD]].player_mspd;
   c.player_mrad         = configs[diffs[GSP_MRAD]].player_mrad;
   c.player_explode_time = configs[diffs[GSP_ETIME]].player_explode_time;
   c.player_nl_str       = configs[diffs[GSP_NLSTR]].player_nl_str;
   c.laser_color         = configs[diffs[GSP_LCOLOR]].laser_color;
   c.laser_blend_mode    = configs[diffs[GSP_LBLEND]].laser_blend_mode;
   c.laser_width         = configs[diffs[GSP_LWIDTH]].laser_width;
   c.laser_duration      = configs[diffs[GSP_LDURATION]].laser_duration;
   c.city_left           = configs[diffs[GSP_CITY_LEFT]].city_left;
   c.shield_depth        = configs[diffs[GSP_SHIELD_DEPTH]].shield_depth;
   
   return c;
}



int GameConfig::GetSeed() {
   return std::stoi(config_file["RAND"]["seed"]);
}





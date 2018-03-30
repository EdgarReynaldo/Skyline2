

#include "Config.hpp"
#include <allegro.h>
#include <cstdio>



ConfigSettings settings;
WidgetColorset gui_colors;

void MakeGuiColors();



const char* const gui_color_names[NUMCOLS] = {
   "SDCOL",
   "BGCOL",
   "MGCOL",
   "FGCOL",
   "HLCOL",
   "TXTCOL"
};

const char* gui_color_strs[NUMCOLS] = {
   "0,0,0,255",
   "64,64,64,255",
   "128,128,128,255",
   "192,192,192,255",
   "255,255,255,254",
   "255,255,255,254"
};



void MakeGuiColors() {
   int r,g,b,a;
   for (int i = 0 ; i < NUMCOLS ; ++i) {
      if (4 != sscanf(gui_color_strs[i] , "%i,%i,%i,%i" , &r , &g , &b , &a)) {
         throw EagleError(StringPrintF("Gui color string #%i (%s) is invalid - proper format is r,g,b,a.\n" , i , gui_color_strs[i]));
      }
      gui_colors.SetRGBA((WIDGETCOLOR)i , r , g , b , a);
   }
}



const char* DiffToStr(DIFFICULTY d) {
   switch (d) {
      case EASY   : return "EASY";
      case MEDIUM : return "MEDIUM";
      case HARD   : return "HARD";
      case CUSTOM : return "CUSTOM";
      default     : return "EASY";
   }
   return 0;
}



DIFFICULTY StrToDiff(const char* str) {
   if (strcmp(str , "EASY") == 0) {return EASY;}
   else if (strcmp(str , "MEDIUM") == 0) {return MEDIUM;}
   else if (strcmp(str , "HARD") == 0) {return HARD;}
   else if (strcmp(str , "CUSTOM") == 0) {return CUSTOM;}
   return EASY;
}


const char* const diff_strs[] = {
   "enemy_nmsl_diff",
   "player_nmsl_diff",
   "enemy_tbl_diff",
   "player_tbl_diff",
   "nl_diff",
   "city_left_diff",
   "enemy_mspd_diff",
   "player_mspd_diff",
   "enemy_mrad_diff",
   "player_mrad_diff",
   "enemy_explode_time_diff",
   "player_explode_time_diff"
};

DIFFICULTY* const diff_ptrs[] = {
   &(settings.diffs.enemy_nmsl_diff),
   &(settings.diffs.player_nmsl_diff),
   &(settings.diffs.enemy_tbl_diff),
   &(settings.diffs.player_tbl_diff),
   &(settings.diffs.nl_diff),
   &(settings.diffs.city_left_diff),
   &(settings.diffs.enemy_mspd_diff),
   &(settings.diffs.player_mspd_diff),
   &(settings.diffs.enemy_mrad_diff),
   &(settings.diffs.player_mrad_diff),
   &(settings.diffs.enemy_explode_time_diff),
   &(settings.diffs.player_explode_time_diff)
};

const char* const enemy_nmsl_strs[NUM_DIFFICULTIES] = 
   {"enemy_nmsl_easy" , "enemy_nmsl_medium" , "enemy_nmsl_hard" , "enemy_nmsl_custom"};

const char* const player_nmsl_strs[NUM_DIFFICULTIES] = 
   {"player_nmsl_easy" , "player_nmsl_medium" , "player_nmsl_hard" , "player_nmsl_custom"};

const char* const enemy_tbl_strs[NUM_DIFFICULTIES] = 
   {"enemy_tbl_easy" , "enemy_tbl_medium" , "enemy_tbl_hard" , "enemy_tbl_custom"};

const char* const player_tbl_strs[NUM_DIFFICULTIES] = 
   {"player_tbl_easy" , "player_tbl_medium" , "player_tbl_hard" , "player_tbl_custom"};

const char* const nl_strs[NUM_DIFFICULTIES] = 
   {"nl_easy" , "nl_medium" , "nl_hard" , "nl_custom"};

const char* const city_left_strs[NUM_DIFFICULTIES] = 
   {"city_left_easy" , "city_left_medium" , "city_left_hard" , "city_left_custom"};

const char* const enemy_mspd_strs[NUM_DIFFICULTIES] = 
   {"enemy_mspd_easy" , "enemy_mspd_medium" , "enemy_mspd_hard" , "enemy_mspd_custom"};

const char* const player_mspd_strs[NUM_DIFFICULTIES] = 
   {"player_mspd_easy" , "player_mspd_medium" , "player_mspd_hard" , "player_mspd_custom"};

const char* const enemy_mrad_strs[NUM_DIFFICULTIES] = 
   {"enemy_mrad_easy" , "enemy_mrad_medium" , "enemy_mrad_hard" , "enemy_mrad_custom"};

const char* const player_mrad_strs[NUM_DIFFICULTIES] = 
   {"player_mrad_easy" , "player_mrad_medium" , "player_mrad_hard" , "player_mrad_custom"};

const char* const enemy_explode_time_strs[NUM_DIFFICULTIES] = 
   {"enemy_explode_time_easy" , "enemy_explode_time_medium" , "enemy_explode_time_hard" , "enemy_explode_time_custom"};

const char* const player_explode_time_strs[NUM_DIFFICULTIES] = 
   {"player_explode_time_easy" , "player_explode_time_medium" , "player_explode_time_hard" , "player_explode_time_custom"};





Difficulty::Difficulty() :
      enemy_nmsl_diff(EASY),
      player_nmsl_diff(EASY),
      enemy_tbl_diff(EASY),
      player_tbl_diff(EASY),
      nl_diff(EASY),
      city_left_diff(EASY),
      enemy_mspd_diff(EASY),
      player_mspd_diff(EASY),
      enemy_mrad_diff(EASY),
      player_mrad_diff(EASY),
      enemy_explode_time_diff(EASY),
      player_explode_time_diff(EASY)
{}



ConfigSettings::ConfigSettings() :
      configs(),
      diffs(),
      seed(232),
      gui_padding(4)
{
   configs[EASY].enemy_nmsl = 45;
   configs[MEDIUM].enemy_nmsl = 90;
   configs[HARD].enemy_nmsl = 180;
   configs[CUSTOM].enemy_nmsl = 90;

   configs[EASY].player_nmsl = 90;
   configs[MEDIUM].player_nmsl = 180;
   configs[HARD].player_nmsl = 360;
   configs[CUSTOM].player_nmsl = 180;
   
   configs[EASY].enemy_tbl = 8.0f;
   configs[MEDIUM].enemy_tbl = 4.0f;
   configs[HARD].enemy_tbl = 3.0f;
   configs[CUSTOM].enemy_tbl = 4.0f;

   configs[EASY].player_tbl = 0.25f;
   configs[MEDIUM].player_tbl = 0.5f;
   configs[HARD].player_tbl = 0.5f;
   configs[CUSTOM].player_tbl = 0.5f;
   
   configs[EASY].nl_str = "1 0,-100,800,100";
   configs[MEDIUM].nl_str = "2 0,-100,400,100 400,-100,400,100";
   configs[HARD].nl_str = "3 0,-100,267,100 267,-100,267,100 534,-100,267,100";
   configs[CUSTOM].nl_str = "4 0,-100,200,100 200,-100,200,100 400,-100,200,100 600,-100,200,100";
   
   configs[EASY].city_left = 0.1f;
   configs[MEDIUM].city_left = 0.25f;
   configs[HARD].city_left = 0.5f;
   configs[CUSTOM].city_left = 0.25f;
   
   configs[EASY].enemy_mspd = 25.0f;
   configs[MEDIUM].enemy_mspd = 50.0f;
   configs[HARD].enemy_mspd = 75.0f;
   configs[CUSTOM].enemy_mspd = 50.0f;

   configs[EASY].player_mspd = 250.0f;
   configs[MEDIUM].player_mspd = 200.0f;
   configs[HARD].player_mspd = 150.0f;
   configs[CUSTOM].player_mspd = 200.0f;
   
   configs[EASY].enemy_mrad = 20;
   configs[MEDIUM].enemy_mrad = 40;
   configs[HARD].enemy_mrad = 60;
   configs[CUSTOM].enemy_mrad = 40;

   configs[EASY].player_mrad = 100;
   configs[MEDIUM].player_mrad = 75;
   configs[HARD].player_mrad = 50;
   configs[CUSTOM].player_mrad = 75;
   
   configs[EASY].enemy_explode_time = 4.0f;
   configs[MEDIUM].enemy_explode_time = 2.0f;
   configs[HARD].enemy_explode_time = 0.5f;
   configs[CUSTOM].enemy_explode_time = 2.0f;
   
   configs[EASY].player_explode_time = 5.0f;
   configs[MEDIUM].player_explode_time = 3.5f;
   configs[HARD].player_explode_time = 2.0f;
   configs[CUSTOM].player_explode_time = 3.5f;
}



ConfigSettings::ConfigSettings(const ConfigSettings& cset) :
      configs(),
      diffs(cset.diffs),
      seed(cset.seed),
      gui_padding(cset.gui_padding)
{
   for (int i = 0 ; i < NUM_DIFFICULTIES ; ++i) {
      configs[i] = cset.configs[i];
   }
}



ConfigSettings& ConfigSettings::operator=(const ConfigSettings& cset) {
   for (int i = 0 ; i < NUM_DIFFICULTIES ; ++i) {
      configs[i] = cset.configs[i];
   }
   diffs = cset.diffs;
   seed = cset.seed;
   gui_padding = cset.gui_padding;
   return *this;
}



void CreateConfigFile(const char* filepath) {
   const char* lines[] = {
      "[AI]",
      "# Number of missiles",
      "enemy_nmsl_easy=45",
      "enemy_nmsl_medium=90",
      "enemy_nmsl_hard=180",
      "enemy_nmsl_custom=90",
      "",
      "player_nmsl_easy=80",
      "player_nmsl_medium=180",
      "player_nmsl_hard=360",
      "player_nmsl_custom=180",
      "",
      "# Time between launches",
      "enemy_tbl_easy=8.0",
      "enemy_tbl_medium=4.0",
      "enemy_tbl_hard=2.0",
      "enemy_tbl_custom=2.0",
      "",
      "player_tbl_easy=0.25",
      "player_tbl_medium=0.5",
      "player_tbl_hard=0.5",
      "player_tbl_custom=0.5",
      "",
      "# Number of launchers and positions - Number of launchers followed by that many space separated",
      "# x,y,w,h quadruplets defining the zone they fire from",
      "nl_easy=1 0,-100,800,100",
      "nl_medium=2 0,-100,400,100 400,-100,400,100",
      "# 800 / 3 = 267",
      "nl_hard=3 0,-100,267,100 267,-100,267,100 534,-100,267,100",
      "nl_custom=4 0,-100,200,100 200,-100,200,100 400,-100,200,100 600,-100,200,100",
      "",
      "# Percentages of city left before game over",
      "city_left_easy=0.1",
      "city_left_medium=0.25",
      "city_left_hard=0.5",
      "city_left_custom=0.25",
      "",
      "# Missile speeds (pixels per second)",
      "enemy_mspd_easy=25.0",
      "enemy_mspd_medium=50.0",
      "enemy_mspd_hard=75.0",
      "enemy_mspd_custom=50.0",
      "",
      "player_mspd_easy=250.0",
      "player_mspd_medium=200.0",
      "player_mspd_hard=150.0",
      "player_mspd_custom=200.0",
      "",
      "# Missile sizes (pixel radius)",
      "enemy_mrad_easy=20",
      "enemy_mrad_medium=40",
      "enemy_mrad_hard=60",
      "enemy_mrad_custom=40",
      "",
      "player_mrad_easy=100",
      "player_mrad_medium=75",
      "player_mrad_hard=50",
      "player_mrad_custom=75",
      "",
      "# Missile explosion rates",
      "enemy_explode_time_easy=4.0",
      "enemy_explode_time_medium=2.0",
      "enemy_explode_time_hard=0.5",
      "enemy_explode_time_custom=2.0",
      "",
      "player_explode_time_easy=5.0",
      "player_explode_time_medium=3.5",
      "player_explode_time_hard=2.0",
      "player_explode_time_custom=3.5",
      "",
      "# Difficulty settings",
      "enemy_nmsl_diff=EASY",
      "player_nmsl_diff=EASY",
      "enemy_tbl_diff=EASY",
      "player_tbl_diff=EASY",
      "# Number of launchers difficulty setting - do NOT use CUSTOM for this setting.",
      "nl_diff=EASY",
      "city_left_diff=EASY",
      "enemy_mspd_diff=EASY",
      "player_mspd_diff=EASY",
      "enemy_mrad_diff=EASY",
      "player_mrad_diff=EASY",
      "enemy_explode_time_diff=EASY",
      "player_explode_time_diff=EASY",
      "",
      "[MOUSE]",
      "okpointer=Data/Images/OkPointer.bmp",
      "nopointer=Data/Images/NoPointer.bmp",
      "",
      "[GUI]",
      "checkbox_checked_file=Data/Images/Checkbox_Checked20x20.bmp",
      "checkbox_unchecked_file=Data/Images/Checkbox_Unchecked20x20.bmp",
      "checkbox_hover_file=Data/Images/Checkbox_Hover20x20.bmp",
      "",
      "padding=4",
      "",
      "# Gui colors - format is r,g,b,a (shadow, background, middle ground, foreground, highlight, text color)",
      "SDCOL=0,0,0,255",
      "BGCOL=64,64,64,255",
      "MGCOL=127,127,127,255",
      "FGCOL=192,192,192,255",
      "HLCOL=255,255,255,254",
      "TXTCOL=255,255,255,254",
      "",
      "[RAND]",
      "seed=232"
   };
   
   FILE* file = fopen(filepath , "wb");
   if (!file) {
      OutputLog() << "Warning : Failed to create config file. :/" << std::endl;
      return;
   }
   for (int i = 0 ; i < (int)sizeof(lines)/(int)sizeof(const char*) ; ++i) {
      fprintf(file , "%s\r\n" , lines[i]);
   }
   fclose(file);
}



void LoadConfig() {

   Config* c = settings.configs;
   for (int d = 0 ; d < NUM_DIFFICULTIES ; ++d) {
      c[d].enemy_nmsl = get_config_int("AI" , enemy_nmsl_strs[d] , c[d].enemy_nmsl);
      c[d].player_nmsl = get_config_int("AI" , player_nmsl_strs[d] , c[d].player_nmsl);
      c[d].enemy_tbl = get_config_float("AI" , enemy_tbl_strs[d] , c[d].enemy_tbl);
      c[d].player_tbl = get_config_float("AI" , player_tbl_strs[d] , c[d].player_tbl);
      c[d].nl_str = get_config_string("AI" , nl_strs[d] , c[d].nl_str.c_str());
      c[d].city_left = get_config_float("AI" , city_left_strs[d] , c[d].city_left);
      c[d].enemy_mspd = get_config_float("AI" , enemy_mspd_strs[d] , c[d].enemy_mspd);
      c[d].player_mspd = get_config_float("AI" , player_mspd_strs[d] , c[d].player_mspd);
      c[d].enemy_mrad = get_config_int("AI" , enemy_mrad_strs[d] , c[d].enemy_mrad);
      c[d].player_mrad = get_config_int("AI" , player_mrad_strs[d] , c[d].player_mrad);
      c[d].enemy_explode_time = get_config_float("AI" , enemy_explode_time_strs[d] , c[d].enemy_explode_time);
      c[d].player_explode_time = get_config_float("AI" , player_explode_time_strs[d] , c[d].player_explode_time);
   }
   settings.seed = get_config_int("RAND" , "seed" , settings.seed);
   
   for (int i = 0 ; i < (int)sizeof(diff_strs)/(int)sizeof(const char*) ; ++i) {
      *(diff_ptrs[i]) = StrToDiff(get_config_string("AI" , diff_strs[i] , DiffToStr(*(diff_ptrs[i]))));
   }
   
   for (int i = 0 ; i < NUMCOLS ; ++i) {
      gui_color_strs[i] = get_config_string("GUI" , gui_color_names[i] , gui_color_strs[i]);
   }
   MakeGuiColors();
   
   settings.gui_padding = get_config_int("GUI" , "padding" , settings.gui_padding);
}



Config GetConfig(ConfigSettings cset) {
   Config c;
   Config* configs = cset.configs;
   Difficulty d = cset.diffs;
   c.enemy_nmsl = configs[d.enemy_nmsl_diff].enemy_nmsl;
   c.player_nmsl = configs[d.player_nmsl_diff].player_nmsl;
   c.enemy_tbl = configs[d.enemy_tbl_diff].enemy_tbl;
   c.player_tbl = configs[d.player_tbl_diff].player_tbl;
   c.nl_str = configs[d.nl_diff].nl_str;
   c.city_left = configs[d.city_left_diff].city_left;
   c.enemy_mspd = configs[d.enemy_mspd_diff].enemy_mspd;
   c.player_mspd = configs[d.player_mspd_diff].player_mspd;
   c.enemy_mrad = configs[d.enemy_mrad_diff].enemy_mrad;
   c.player_mrad = configs[d.player_mrad_diff].player_mrad;
   c.enemy_explode_time = configs[d.enemy_explode_time_diff].enemy_explode_time;
   c.player_explode_time = configs[d.player_explode_time_diff].player_explode_time;
   return c;
}



Config GetConfig() {
   return GetConfig(settings);
}



ConfigSettings GetConfigSettings() {
   return settings;
}



Difficulty GetDifficulties() {
   return settings.diffs;
}



int GetSeed() {return settings.seed;}



WidgetColorset GetColorConfig() {
   return gui_colors;
}



void SetCustomConfig(Config custom_config) {
   settings.configs[CUSTOM] = custom_config;
}



void SetDifficulties(Difficulty difficulties) {
   settings.diffs = difficulties;
}



void SetSeed(int s) {settings.seed = s;}



void WriteCustomConfig() {
//void set_config_int(const char *section, const char *name, int val);
//void set_config_float(const char *section, const char *name, float val);
//void set_config_string(const char *section, const char *name, const char *val);
   Config c = settings.configs[CUSTOM];
   set_config_int("AI" , enemy_nmsl_strs[CUSTOM] , c.enemy_nmsl);
   set_config_int("AI" , player_nmsl_strs[CUSTOM] , c.player_nmsl);
   set_config_float("AI" , enemy_tbl_strs[CUSTOM] , c.enemy_tbl);
   set_config_float("AI" , player_tbl_strs[CUSTOM] , c.player_tbl);
///   set_config_string("AI" , nl_strs[CUSTOM] , c.nl_str.c_str());
   set_config_float("AI" , city_left_strs[CUSTOM] , c.city_left);
   set_config_float("AI" , enemy_mspd_strs[CUSTOM] , c.enemy_mspd);
   set_config_float("AI" , player_mspd_strs[CUSTOM] , c.player_mspd);
   set_config_int("AI" , enemy_mrad_strs[CUSTOM] , c.enemy_mrad);
   set_config_int("AI" , player_mrad_strs[CUSTOM] , c.player_mrad);
   set_config_float("AI" , enemy_explode_time_strs[CUSTOM] , c.enemy_explode_time);
   set_config_float("AI" , player_explode_time_strs[CUSTOM] , c.player_explode_time);
}



void WriteDifficulties() {
   for (int i = 0 ; i < (int)sizeof(diff_strs)/(int)sizeof(const char*) ; ++i) {
      //void set_config_string(const char *section, const char *name, const char *val);
      set_config_string("AI" , diff_strs[i] , DiffToStr(*(diff_ptrs[i])));
   }
}



void WriteConfigSeed() {
   set_config_int("RAND" , "seed" , settings.seed);
}



void SetConfigSettingsAndWrite(ConfigSettings cset) {
   SetDifficulties(cset.diffs);
   SetCustomConfig(cset.configs[CUSTOM]);
   SetSeed(cset.seed);
   WriteDifficulties();
   WriteCustomConfig();
   WriteConfigSeed();
}





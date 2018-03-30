

#ifndef Config_H
#define Config_H

#include <string>
using std::string;

#include <cstring>

#include "Eagle.hpp"


enum DIFFICULTY {
   EASY = 0,
   MEDIUM = 1,
   HARD = 2,
   CUSTOM = 3
};

#define NUM_DIFFICULTIES 4

const char* DiffToStr(DIFFICULTY d);
DIFFICULTY StrToDiff(const char* str);



class Config {
public :
   int enemy_nmsl;
   int player_nmsl;
   float enemy_tbl;
   float player_tbl;
   string nl_str;
   float city_left;
   float enemy_mspd;
   float player_mspd;
   int enemy_mrad;
   int player_mrad;
   float enemy_explode_time;
   float player_explode_time;
};

class Difficulty {
public :
   Difficulty();
   
   DIFFICULTY enemy_nmsl_diff;
   DIFFICULTY player_nmsl_diff;
   DIFFICULTY enemy_tbl_diff;
   DIFFICULTY player_tbl_diff;
   DIFFICULTY nl_diff;
   DIFFICULTY city_left_diff;
   DIFFICULTY enemy_mspd_diff;
   DIFFICULTY player_mspd_diff;
   DIFFICULTY enemy_mrad_diff;
   DIFFICULTY player_mrad_diff;
   DIFFICULTY enemy_explode_time_diff;
   DIFFICULTY player_explode_time_diff;
};



class ConfigSettings {
public :
   ConfigSettings();
   ConfigSettings(const ConfigSettings& cset);
   ConfigSettings& operator=(const ConfigSettings& cset);

   Config configs[NUM_DIFFICULTIES];
   Difficulty diffs;
   int seed;
   int gui_padding;
};

void CreateConfigFile(const char* filepath);

void LoadConfig();

Config GetConfig(ConfigSettings cset);
Config GetConfig();

ConfigSettings GetConfigSettings();
Difficulty GetDifficulties();
int GetSeed();

WidgetColorset GetColorConfig();

void SetCustomConfig(Config custom_config);
void SetDifficulties(Difficulty difficulties);
void SetSeed(int s);

void WriteCustomConfig();// Does not take effect until allegro_exit is called!
void WriteDifficulties();// Does not take effect until allegro_exit is called!
void WriteConfigSeed();// Call SetSeed first! Does not take effect until allegro_exit is called!

void SetConfigSettingsAndWrite(ConfigSettings cset);// Write does not take place until allegro_exit is called



#endif // Config_H


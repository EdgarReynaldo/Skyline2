



#ifndef NewConfig_HPP
#define NewConfig_HPP


#include "Eagle/ConfigFile.hpp"


enum GAME_SETTING {
   GSE_NMSL          = 0,
   GSE_TBL           = 1,
   GSE_MSPD          = 2,
   GSE_MRAD          = 3,
   GSE_ETIME         = 4,
   GSE_NLSTR         = 5,
   
   GSP_NMSL          = 6,
   GSP_TBL           = 7,
   GSP_MSPD          = 8,
   GSP_MRAD          = 9,
   GSP_ETIME         = 10,
   GSP_NLSTR         = 11,
   
   GSP_LCOLOR        = 12,
   GSP_LBLEND        = 13,
   GSP_LWIDTH        = 14,
   GSP_LDURATION     = 15,
   
   GSP_CITY_LEFT     = 16,
   GSP_SHIELD_DEPTH  = 17,
   
   NUM_GAME_SETTINGS = 18
};


class Config {
public :
   int enemy_nmsl;
   float enemy_tbl;
   float enemy_mspd;
   int enemy_mrad;
   float enemy_explode_time;
   std::string enemy_nl_str;


   int player_nmsl;
   float player_tbl;
   float player_mspd;
   int player_mrad;
   float player_explode_time;
   std::string player_nl_str;

   std::string laser_color;
   std::string laser_blend_mode;
   float laser_width;
   float laser_duration;
   
   float city_left;
   int shield_depth;
};




enum DIFFICULTY {
   EASY   = 0,
   MEDIUM = 1,
   HARD   = 2,
   INSANE = 3,
   CUSTOM = 4,
   NUM_DIFFICULTIES = 5
};


std::string DiffToStr();
DIFFICULTY StrToDiff(const char* str);


std::string GetConfigKeyName(GAME_SETTING setting , DIFFICULTY d);



class GameConfig {
   
protected :
   ConfigFile config_file;
   
   Config configs[NUM_DIFFICULTIES];
   
   DIFFICULTY diffs[NUM_GAME_SETTINGS];
   
   
   void SetupDefaultConfig();

   void ReadStoredConfig();
   
public :
   
   GameConfig();
   
   bool LoadConfig();
   
   void SetDifficulty(GAME_SETTING gs , DIFFICULTY d);
   void SetOverallDifficulty(DIFFICULTY d);

   Config GetSelectedConfig();
   
   int GetSeed();
   
};


#endif // NewConfig_HPP

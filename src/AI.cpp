



#include "AI.hpp"
#include "Missiles.hpp"
#include "Lasers.hpp"
#include "Globals.hpp"
#include "RNG.hpp"




AI::AI() :
      mb(0)
{}



std::vector<Missile*> AI::GetMissiles() {
   if (mb) {
      return mb->GetMissiles();
   }
   return std::vector<Missile*>();
}



int AI::NMissilesLeft() {
   if (mb) {
      return mb->NMissilesLeft();
   }
   return 0;
}





/// ---------------------------------     EnemyAI     -----------------------------------



EnemyAI::EnemyAI() :
      AI()
{}



EnemyAI::~EnemyAI() {
   Free();
}



void EnemyAI::Free() {
   if (mb) {
      delete mb;
      mb = 0;
   }
}



void EnemyAI::Setup(const Config& c) {
   Free();
   MISSILEDATA mdat(c.enemy_mspd , c.enemy_explode_time , c.enemy_mrad);
   mb = new MissileBattery(c.enemy_nmsl , PlainMissileCreator , mdat);
   string lstr = c.enemy_nl_str;
   const char* s = lstr.c_str();
   int NLAUNCHERS = 0;
   int NREAD = 0;
   if (1 != sscanf(s , "%d%n" , &NLAUNCHERS , &NREAD)) {
      throw EagleException(StringPrintF("EnemyAI::Setup - could not read number of launchers from nlstr '%s'\n" , s));
   }
   EAGLE_ASSERT(NLAUNCHERS > 0);
   s += NREAD;
   float xpos = 0.0f;
   for (int i = 0 ; i < NLAUNCHERS ; ++i) {
      if (1 != sscanf(s , "%f%n" , &xpos , &NREAD)) {
         throw EagleException(StringPrintF("EnemyAI::Setup - could not read launcher position #%d from '%s'\n" , i , s));
      }
      mb->AddLauncher(SingleMissileLauncher(PlainMissileCreator , Pos2D(xpos*sw , -100) , c.enemy_tbl));/// TODO : Single missile launcher?
      s += NREAD;
   }
}



void EnemyAI::Update(double dt) {
   if (mb) {
      mb->Update(dt);
   }
}



void EnemyAI::HandleEvent(EagleEvent ee) {
   (void)ee;
}



void EnemyAI::CheckInputs() {
   if (mb) {
      if (mb->Ready()) {
         mb->Launch(sw*Percent() , sh);
      }
   }
}



void EnemyAI::Display() {
   if (mb) {
///      mb->Display();
   }
}



/// ---------------------------------     PlayeerAI     -----------------------------------




PlayerAI::PlayerAI() :
      AI(),
      lb(0),
      aim(sw/2,sh/2),
      mtrigger(input_mouse_press(LMB)),
      mtriggerheld(input_mouse_held(LMB)),
      ltrigger(input_mouse_press(RMB)),
      ltriggerheld(input_mouse_held(RMB)),
      lcreator(0)
{}



PlayerAI::~PlayerAI() {
   Free();
}



void PlayerAI::Free() {
   if (mb) {
      delete mb;
      mb = 0;
   }
   if (lb) {
      delete lb;
      lb = 0;
   }
}



void PlayerAI::Setup(const Config& c) {
   Free();
   
   lb = new LaserBattery();
   lb->Setup(c);
   
   MISSILEDATA mdat(c.player_mspd , c.player_explode_time , c.player_mrad);
   mb = new MissileBattery(c.player_nmsl , AAMissileCreator , mdat);
   
   std::string lstr = c.player_nl_str;
   const char* str = lstr.c_str();
   unsigned int NSPREAD = 0;
   int NREAD = 0;
   if (1 != sscanf(str , "%u%n" , &NSPREAD , &NREAD)) {
      throw new EagleException(StringPrintF("Failed to read spread number from player launcher str '%s'" , str));
   }
   EagleLog() << StringPrintF("Player::Setup - spread is %u\n" , NSPREAD) << std::endl;
   lcreator = 0;///SingleMissileLauncher;
   switch (NSPREAD) {
   case 1 :
      lcreator = SingleMissileLauncher;
      break;
   case 3 :
      lcreator = TripleMissileLauncher;
      break;
   case 5 :
      lcreator = PentupleMissileLauncher;
      break;
   case 7 :
      lcreator = SeptupleMissileLauncher;
      break;
   default :
      break;
   }
   if (!lcreator) {
      throw new EagleException(StringPrintF("Player::Setup - spread number is invalid (%d)\n" , NSPREAD));
   }
   mb->AddLauncher(lcreator(AAMissileCreator , Pos2D(sw/2.0 + 0.5 , sh) , c.player_tbl));
}



void PlayerAI::Update(double dt) {
   if (mb) {
      mb->Update(dt);
   }
   if (lb) {
      lb->Update(dt);
   }
}



void PlayerAI::HandleEvent(EagleEvent ee) {
   if (ee.type == EAGLE_EVENT_MOUSE_AXES) {
      aim.SetPos(ee.mouse.x , ee.mouse.y);
      if (lb) {
         lb->AimAt(aim.X() , aim.Y());
      }
   }
}



void PlayerAI::CheckInputs() {
   if (mb) {
      if (mtrigger || mtriggerheld) {
         if (mb->Ready()) {
            mb->Launch(aim.X() , aim.Y());
         }
      }
   }
   if (lb) {
      if (ltrigger || ltriggerheld) {
         lb->Fire();
      }
   }
}



void PlayerAI::Display() {
   if (mb) {
      mb->Display();
   }
}



void PlayerAI::DrawLasers() {
   lb->Draw();
}



bool PlayerAI::MissilesReady() {
   return mb?mb->Ready():false;
}









#include "Globals.hpp"
#include "Missiles.hpp"
#include "Explosion.hpp"


#define MIN(a,b) ((a<b)?a:b)



/// ------------------------------------     Missile class     --------------------------------------



Missile::Missile(float startx , float starty , float speed , float angle , int radius , float time) :
      sx(startx),
      sy(starty),
      xp(startx),
      yp(starty),
      xv(0.0f),
      yv(0.0f),
      theta(angle),
      crad(0.0f),
      radspeed(0.0f),
      explodetime(time),
      rad(radius),
      state(NORMAL)
{
   xv = speed*cos(theta);
   yv = speed*sin(theta);
   radspeed = (float)rad/explodetime;
}



void Missile::Update(double dt) {
   switch (state) {
      case NORMAL :
         xp += xv*dt;
         yp += yv*dt;
         break;
      case EXPLODING :
         crad += radspeed*dt;
         if (crad > (float)rad) {
            crad = rad;
            state = EXPLODED;
         }
         break;
      case EXPLODED :
         state = IMPLODING;
         break;
      case IMPLODING :
         crad -= radspeed*dt;
         if (crad < 0.0f) {
            crad = 0.0f;
            state = TOAST;
         }
      case TOAST :
         break;
   }
}

void Missile::Display() {
   switch (state) {
      case NORMAL :
         win->DrawFilledCircle(xp , yp , 3.0f , EagleColor(255,0,0));
///         putpixel(bmp , (int)xp , (int)yp , makecol(255,255,255));
///         circlefill(bmp , (int)xp , (int)yp , 3 , makecol(255,0,0));
         break;
      case EXPLODING :
      case EXPLODED :
      case IMPLODING :
         DrawExplosion(xp + 0.5 , yp + 0.5 , crad , EagleColor(255,0,0,127) , EagleColor(255,255,0,255));
//void ring_gradient(BITMAP* bmp , int cx , int cy , int inner_radius , int outer_radius , int inner_color , int outer_color);
///         circlefill(bmp , (int)xp , (int)yp , (int)crad , makecol(255,255,255));
///         ring_gradient(bmp , (int)xp , (int)yp , 0 , (int)crad , makecol(255,0,0) , makecol(255,255,0));
//         ring_gradient(bmp , (int)xp , (int)yp , 0 , (int)crad , makecol(0,64,255) , makecol(255,255,255));
         break;
      case TOAST :
         break;
   }
}



void Missile::Explode() {
   if (state == NORMAL) {state = EXPLODING;}
}



void Missile::Destroy() {
   state = TOAST;
}



/// ------------------------------------     AAMissile     --------------------------------------



AAMissile::AAMissile(float startx , float starty , float speed , float angle , int radius , float time , int destx , int desty) :
      Missile(startx , starty , speed , angle , radius , time),
      dx(destx),
      dy(desty),
      mspeed(speed),
      distance_left(0)
{
   int deltax = destx - (int)startx;
   int deltay = desty - (int)starty;
   distance_left = sqrt((float)deltax*deltax + (float)deltay*deltay);
}



void AAMissile::Update(double dt) {
   switch (state) {
      case NORMAL :
         xp += xv*dt;
         yp += yv*dt;
         distance_left -= mspeed*dt;
         if (distance_left < 0.0) {
            state = EXPLODING;
            xp = dx;
            yp = dy;
         }
         break;
      case EXPLODING :
         crad += radspeed*dt;
         if (crad > (float)rad) {
            crad = rad;
            state = EXPLODED;
         }
         break;
      case EXPLODED :
         state = IMPLODING;
         break;
      case IMPLODING :
         crad -= radspeed*dt;
         if (crad < 0.0f) {
            crad = 0.0f;
            state = TOAST;
         }
      case TOAST :
         break;
   }
}



void AAMissile::Display() {
///   Missile::Display(bmp);

//*
   switch (state) {
      case NORMAL :
         win->DrawFilledCircle(xp , yp , 3.0 , EagleColor(255,255,255));
///         circlefill(bmp , (int)xp , (int)yp , 3 , makecol(255,255,0));
         break;
      case EXPLODING :
      case EXPLODED :
      case IMPLODING :
         DrawExplosion(xp + 0.5 , yp + 0.5 , crad , EagleColor(0.0f , 0.25f , 1.0f , 0.5f) , EagleColor(1.0f,1.0f,1.0f , 1.0f));
         break;
      case TOAST :
         break;
   }
//*/
}



/// ------------------------------------   Missile Battery class       --------------------------------------


MissileBattery::MissileBattery() :
      missiles(),
      nmissiles(0),
      nmissilesleft(0),
      ttnl(0.0f),
      tbl(0.0f),
      ai(0)
{}



MissileBattery::~MissileBattery() {
   FreeAI();
   FreeMissiles();
}



void MissileBattery::FreeAI() {
   if (ai) {delete ai;}
   ai = 0;
}



void MissileBattery::FreeMissiles() {
   for (list<Missile*>::iterator it = missiles.begin() ; it != missiles.end() ; ++it) {
      delete *it;
   }
   missiles.clear();
}



void MissileBattery::SetAI(AI* new_ai) {
   EAGLE_ASSERT(new_ai);
   FreeAI();
   ai = new_ai;
   ai->ControlMissileBattery(this);
}



void MissileBattery::Update(double dt) {
   EAGLE_ASSERT(ai);
   for (list<Missile*>::iterator it = missiles.begin() ; it != missiles.end() ; ) {
      Missile* m = *it;
      m->Update(dt);
      if (m->Y() >= 600) {m->Destroy();}
      if (m->State() == TOAST) {
         delete m;
         it = missiles.erase(it);
         --nmissiles;
      } else {
         ++it;
      }
   }
   ai->Update(dt);
}



void MissileBattery::CheckInputs() {
   EAGLE_ASSERT(ai);
   ai->CheckInputs();
}



void MissileBattery::Display() {
   for (list<Missile*>::iterator it = missiles.begin() ; it != missiles.end() ; ++it) {
      Missile* m = *it;
      m->Display();
   }
}




/// ------------------------------------   AI class       --------------------------------------



AI::AI(Rectangle launch_zone , int num_missiles , Config config) :
   mb(0),
   zone(launch_zone),
   nmissiles(num_missiles),
   c(config)
{}



void AI::DelayLaunchBy(double dt) {
   if (dt < 0.0) {dt = 0.0;}
   if (mb) {
      mb->ttnl += dt;
   }
}



/// ------------------------------------   EnemyAI class       --------------------------------------



void EnemyAI::Launch() {
   EAGLE_ASSERT(mb);
   if (!(mb->nmissilesleft)) {return;}
   --(mb->nmissilesleft);
   ++(mb->nmissiles);
   int startx = zone.X() + rand()%zone.W();
   int starty = zone.Y() + rand()%zone.H();
   int destx = rand()%800;
   int desty = 600;
   float angle = atan2(desty - starty , destx - startx);
//   Missile(float startx , float starty , float speed , float angle , int radius , float time);
   mb->missiles.push_back(
      new Missile(startx , starty , c.enemy_mspd , angle , c.enemy_mrad , c.enemy_explode_time)
   );
}



EnemyAI::EnemyAI(Rectangle launch_zone , int num_missiles , Config config) :
   AI(launch_zone , num_missiles , config)
{}



void EnemyAI::ControlMissileBattery(MissileBattery* battery) {
   EAGLE_ASSERT(battery);
   mb = battery;
   mb->FreeMissiles();
/*
   list<Missile*> missiles;
   int nmissiles;
   int nmissilesleft;
   float ttnl;// time to next launch
   float tbl;// time between launches
*/   
   mb->nmissiles = 0;
   mb->nmissilesleft = nmissiles;
   mb->tbl = c.enemy_tbl;
   mb->ttnl = mb->tbl;
}



void EnemyAI::Update(double dt) {
   double time_left = dt;
   while (time_left > 0.0) {
      // time_left
      // time_to_next_launch
      // time_between_launches
      // MIN(ttnl , MIN(time_left , tbl));
      double time_passed = MIN(mb->ttnl , MIN(time_left , mb->tbl));
      time_left -= time_passed;
      mb->ttnl -= time_passed;
      if (mb->ttnl <= 0.0) {
         Launch();
         mb->ttnl += mb->tbl;
      }
   }
}



void EnemyAI::CheckInputs() {(void)0;}



/// ------------------------------------   PlayerAI class       --------------------------------------



void PlayerAI::Launch(int dx , int dy) {
   EAGLE_ASSERT(mb);
   if (!(mb->nmissilesleft)) {return;}
   --(mb->nmissilesleft);
   int startx = zone.X() + rand()%zone.W();
   int starty = zone.Y() + rand()%zone.H();
   float angle = atan2(dy - starty , dx - startx);
//   AAMissile(float startx , float starty , float speed , float angle , int radius , float time , int destx , int desty);
   mb->missiles.push_back(
      new AAMissile(startx , starty , c.player_mspd , angle , c.player_mrad , c.player_explode_time , dx , dy)
   );
   mb->ttnl = mb->tbl;
}



PlayerAI::PlayerAI(Rectangle launch_zone , int num_missiles , Config config) :
      AI(launch_zone , num_missiles , config)
{}



void PlayerAI::ControlMissileBattery(MissileBattery* battery) {
   EAGLE_ASSERT(battery);
   mb = battery;
   mb->FreeMissiles();
/*
   list<Missile*> missiles;
   int nmissiles;
   int nmissilesleft;
   float ttnl;// time to next launch
   float tbl;// time between launches
*/   
   mb->nmissiles = 0;
   mb->nmissilesleft = nmissiles;
   mb->tbl = c.player_tbl;
   mb->ttnl = mb->tbl;
}



void PlayerAI::Update(double dt) {
   EAGLE_ASSERT(mb);
   mb->ttnl -= dt;
   
}



void PlayerAI::CheckInputs() {
   EAGLE_ASSERT(mb);
   if (input_mouse_press(LMB) || input_mouse_held(LMB)) {
      if (mb->ttnl <= 0.0) {
         Launch(mouse_x , mouse_y);
      }
   }
}








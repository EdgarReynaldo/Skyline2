

#include "Globals.hpp"
#include "Missiles.hpp"
#include "Explosion.hpp"
#include "OpenGLDrawing.hpp"

#define MIN(a,b) ((a<b)?a:b)


#include "GL/gl.h"




/// ------------------------------------     Missile class     --------------------------------------




Missile::Missile(Pos2D startpos , Pos2D destpos , MISSILEDATA data) :
      start(startpos),
      dest(destpos),
      cpos(startpos),
      cvel(0.0 , 0.0),
      theta(0.0),
      speed(data.mspeed),
      distleft(-1.0),
      crad(0.0),
      radspeed(0.0),
      explodetime(data.etime),
      rad(data.eradius),
      state(NORMAL)
{
   SetDestination(destpos);
   radspeed = (double)rad/explodetime;
}



void Missile::SetDestination(Pos2D destpos) {
   dest = destpos;
   Pos2D move = dest - start;
   distleft = move.Length();
   cvel = move;
   cvel.Normalize();
   cvel *= speed;
}



void Missile::Update(double dt) {
   switch (state) {
      case NORMAL :
         {
            Pos2D mv = cvel*dt;
            cpos += mv;
         }
         distleft -= speed*dt;
         if (distleft < 0.0) {
            state = EXPLODING;
            cpos = dest;
         }
         break;
      case EXPLODING :
         crad += radspeed*dt;
         if (crad > (double)rad) {
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
   static const double TRAIL_LENGTH = 150.0;
   float xp = cpos.X();
   float yp = cpos.Y();
   EagleColor mc = MissileColor();
   switch (state) {
      case NORMAL :
         win->DrawFilledCircle(xp , yp , 3.0f , mc);

         DrawJet(cpos , dest.AngleToPoint(start) , IExplosionColor() , OExplosionColor());
         
         if (false) {/// Draw a trail
            Pos2D b1 = start - dest;
            b1.Normalize();
            b1 *= TRAIL_LENGTH;
            Pos2D b2 = cpos + b1;
            glBegin(GL_LINES);
            glColor4d(mc.R() , mc.G() , mc.B() , mc.A());
            glVertex2d(cpos.X() , cpos.Y());
            glColor4d(0.0,0.0,0.0,0.0);
            glVertex2d(b2.X() , b2.Y());
            glEnd();
///            win->DrawLine(cpos.X() , cpos.Y() , b2.X() , b2.Y() , 1.0 , mc);
         }
         break;
      case EXPLODING :
      case EXPLODED :
      case IMPLODING :
         DrawExplosion(xp + 0.5 , yp + 0.5 , crad , IExplosionColor() , OExplosionColor());
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



AAMissile::AAMissile(Pos2D startpos , Pos2D destpos , MISSILEDATA data) :
      Missile(startpos , destpos , data)
{}



/// -----------------    Missile creators     --------------------------



Missile* PlainMissileCreator(Pos2D startpos , Pos2D destpos , MISSILEDATA data) {
   return new Missile(startpos , destpos , data);
}



Missile* AAMissileCreator(Pos2D startpos , Pos2D destpos , MISSILEDATA data) {
   return new AAMissile(startpos , destpos , data);
}




/// ------------------------------------   Missile Battery class       --------------------------------------


MissileLauncher::~MissileLauncher() {
   FreeMissiles();
}



void MissileLauncher::Launch(Pos2D destpos , MISSILEDATA data) {
   EAGLE_ASSERT(mcreator);
   missiles.push_back(mcreator(lpos , destpos , data));
   ttnl = tbl;
}



void MissileLauncher::FreeMissiles() {
   for (std::vector<Missile*>::iterator it = missiles.begin() ; it != missiles.end() ; ++it) {
      delete *it;
   }
   missiles.clear();
}



void MissileLauncher::Update(double dt) {
   for (std::vector<Missile*>::iterator it = missiles.begin() ; it != missiles.end() ; ) {
      Missile* m = *it;
      m->Update(dt);
      if (m->Y() >= sh) {m->Destroy();}
      if (m->State() == TOAST) {
         delete m;
         it = missiles.erase(it);
      } else {
         ++it;
      }
   }
   ttnl -= dt;
}



void MissileLauncher::Display() {
   for (std::vector<Missile*>::iterator it = missiles.begin() ; it != missiles.end() ; ++it) {
      Missile* m = *it;
      m->Display();
   }
}



int MissileLauncher::NMissilesActive() {
   return missiles.size();
}



/// --------------------       SpreadMissileLauncher       --------------------------



SpreadMissileLauncher::SpreadMissileLauncher(MISSILECREATOR creator , Pos2D pos , double time_between_launches) :
      MissileLauncher(creator , pos , time_between_launches),
      nmsl(1),
      arcrad(0.0)
{}



void SpreadMissileLauncher::Launch(Pos2D destpos , MISSILEDATA data) {
   LaunchSpread(nmsl , arcrad , destpos , data.mspeed , data.etime , data.eradius);
}



void SpreadMissileLauncher::SetSpread(int nmissiles , double arcdeg) {
   nmsl = (nmissiles > 0)?nmissiles:1;
   arcrad = arcdeg*M_PI/180.0;
}



void SpreadMissileLauncher::LaunchSpread(int nmissiles , double arc , Pos2D destpos , double mspeed , double etime , int mradius) {
   double theta = lpos.AngleToPoint(destpos);
   double harc = arc/2.0;
   int NM = nmissiles/2;
   double dtheta = harc/NM;
   if (nmissiles % 2 == 1) {
      MissileLauncher::Launch(destpos , MISSILEDATA(mspeed , etime , mradius));
   }
   Pos2D radius = destpos - lpos;
   double r = radius.Length();
   for (int i = 1 ; i < NM + 1 ; ++i) {
      Pos2D dp1 = Vector(lpos , r , theta + i*dtheta);
      Pos2D dp2 = Vector(lpos , r , theta - i*dtheta);
      MissileLauncher::Launch(dp1 , MISSILEDATA(mspeed , etime , mradius));
      MissileLauncher::Launch(dp2 , MISSILEDATA(mspeed , etime , mradius));
   }
}



/// -------------------------     Missile Launcher creators     ------------------------------



MissileLauncher* SingleMissileLauncher(MISSILECREATOR creator , Pos2D pos , double time_between_launches) {
   return new MissileLauncher(creator , pos , time_between_launches);
}



MissileLauncher* TripleMissileLauncher(MISSILECREATOR creator , Pos2D pos , double time_between_launches) {
   SpreadMissileLauncher* m = new SpreadMissileLauncher(creator , pos , time_between_launches);
   m->SetSpread(3 , 10.0);
   return m;
}



MissileLauncher* PentupleMissileLauncher(MISSILECREATOR creator , Pos2D pos , double time_between_launches) {
   SpreadMissileLauncher* m = new SpreadMissileLauncher(creator , pos , time_between_launches);
   m->SetSpread(5 , 30.0);
   return m;
}



MissileLauncher* SeptupleMissileLauncher(MISSILECREATOR creator , Pos2D pos , double time_between_launches) {
   SpreadMissileLauncher* m = new SpreadMissileLauncher(creator , pos , time_between_launches);
   m->SetSpread(7 , 60.0);
   return m;
}



/// ---------------------      MissileBattery      ------------------------------------



MissileBattery::MissileBattery(int nmissiles , MISSILECREATOR mcreator , MISSILEDATA data) :
      launchers(),
      nmissilestotal(nmissiles),
      nmissilesleft(nmissiles),
      clauncher(-1),
      mcfunc(mcreator),
      mdata(data)
{}



MissileBattery::~MissileBattery() {
   Free();
}
   


void MissileBattery::Free() {
   for (int i = 0 ; i < (int)launchers.size() ; ++i) {
      delete launchers[i];
   }
   launchers.clear();
   clauncher = -1;
}



void MissileBattery::AddLauncher(MissileLauncher* ml) {
   EAGLE_ASSERT(ml);
   launchers.push_back(ml);
   clauncher = 0;
}



void MissileBattery::Launch(int destx , int desty) {
   if (nmissilesleft < 1) {return;}
   for (int i = 0 ; i < (int)launchers.size() ; ++i) {
      int lnum = (clauncher + i)%(int)launchers.size();
      if (launchers[lnum]->Ready()) {
         launchers[lnum]->Launch(Pos2D(destx + 0.5 , desty + 0.5) , mdata);
         clauncher = (lnum + 1)%(int)launchers.size();
         --nmissilesleft;
         break;
      }
   }
}



void MissileBattery::Update(double dt) {
   for (unsigned int i = 0 ; i < launchers.size() ; ++i) {
      launchers[i]->Update(dt);
   }
}



void MissileBattery::Display() {
   for (unsigned int i = 0 ; i < launchers.size() ; ++i) {
      MissileLauncher* ml = launchers[i];
      ml->Display();
   }
}



bool MissileBattery::Ready() {
   for (int i = 0 ; i < (int)launchers.size() ; ++i) {
      if (launchers[i]->Ready()) {return true;}
   }
   return false;
}



int MissileBattery::NMissilesLeft() {
   return nmissilesleft + NMissilesActive();
}



int MissileBattery::NMissilesActive() {
   int total = 0;
   for (int i = 0 ; i < (int)launchers.size() ; ++i) {
      total += launchers[i]->NMissilesActive();
   }
   return total;
}



std::vector<Missile*> MissileBattery::GetMissiles() {
   std::vector<Missile*> missiles;
   for (unsigned int i = 0 ; i < launchers.size() ; ++i) {
      MissileLauncher* ml = launchers[i];
      std::vector<Missile*> submissiles = ml->GetMissiles();
      missiles.insert(missiles.end() , submissiles.begin() , submissiles.end());
   }
   return missiles;
}



;

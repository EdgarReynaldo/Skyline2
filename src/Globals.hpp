



#ifndef Globals_HPP
#define Globals_HPP


#include "SoundMan.hpp"



extern int sw;
extern int sh;

extern int FPS;

class EagleSystem;
class EagleGraphicsContext;
class EagleEventHandler;
class EagleTimer;
class EagleFont;

extern EagleSystem* sys;
extern EagleGraphicsContext* win;
extern EagleEventHandler* q;
extern EagleTimer* t;
extern EagleFont* f;

extern EagleFont* menu_font;

extern SoundManager soundman;

extern SOUNDID entrysid;
extern SOUNDID explodesid;
extern SOUNDID launchsid;

#endif // Globals_HPP





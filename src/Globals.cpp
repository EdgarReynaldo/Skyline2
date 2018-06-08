


#include "Globals.hpp"

#include "Eagle.hpp"

int sw = 1024;
int sh = 768;

int FPS = 60;

EagleSystem* sys = 0;
EagleGraphicsContext* win = 0;
EagleEventHandler* q = 0;
EagleTimer* t = 0;
EagleFont* f = 0;
EagleFont* menu_font = 0;

SoundManager soundman;

SOUNDID entrysid = BADSOUNDID;
SOUNDID explodesid = BADSOUNDID;
SOUNDID launchsid = BADSOUNDID;

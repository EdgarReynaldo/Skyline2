



#ifndef SoundMan_HPP
#define SoundMan_HPP



#include "allegro5/allegro.h"
#include "allegro5/allegro_audio.h"



#include <string>
#include <map>


typedef unsigned int SOUNDID;

extern const SOUNDID BADSOUNDID;

class Sound {
   SOUNDID id;
   ALLEGRO_SAMPLE* sample;
   
public :
   Sound();
   ~Sound();
   
   void Free();
   bool Load(std::string sndfile);

   SOUNDID ID() {return id;}
   ALLEGRO_SAMPLE* Sample() {return sample;}
};


class SoundManager {
   
   std::map<SOUNDID , Sound*> sounds;
   
   std::multimap<Sound* , ALLEGRO_SAMPLE_INSTANCE*> instances;
   
public :
   
   SOUNDID LoadSound(std::string sndfile);

   ALLEGRO_SAMPLE_INSTANCE* Play(SOUNDID sid , ALLEGRO_MIXER* mixer ,
                                 float gain = 1.0f , float pan = ALLEGRO_AUDIO_PAN_NONE ,
                                 float speed = 1.0f , ALLEGRO_PLAYMODE mode = ALLEGRO_PLAYMODE_ONCE);


   
};



#endif // SoundMan_HPP






#ifndef Library_HPP
#define Library_HPP

/**
#include <vector>
#include <string>

#include "allegro5/allegro.h"
#include "allegro5/allegro_audio.h"




typedef unsigned int SOUNDID;

SOUNDID GetNewSoundID();

class Sound : public ResourceBase {
   
}

class Allegro5Sound : public Sound {

protected :
   FilePath fpath;
   
   ALLEGRO_SAMPLE* sample;
   
   std::vector<ALLEGRO_SAMPLE_INSTANCE*> instances;
   
   Sound(const Sound& s);
   Sound& operator=(const Sound& s);

public :
   Sound();

   void Free();

   ALLEGRO_SAMPLE* Load(std::string path , std::string file);
   
   ALLEGRO_SAMPLE_INSTANCE* Play(ALLEGRO_MIXER* mixer , float gain , float pan , float speed , ALLEGRO_PLAYMODE mode);
   
   void CullStoppedSounds();
   
};



ALLEGRO_SAMPLE_INSTANCE* PlaySound(ALLEGRO_SAMPLE* sample , ALLEGRO_MIXER* mixer , float gain , float pan , float speed , ALLEGRO_PLAYMODE mode);


#include <map>
#include <string>

class SoundLibrary {

protected :   
   std::map<SOUNDID , Sound*> soundmap;

public :
   
   void Free();

   int Load(std::string directory);
   
///   SoundLibrary();
   SoundLibrary() :
         soundmap()
   {}
///   ~SoundLibrary();
   ~SoundLibrary() {Free();}


   ALLEGRO_SAMPLE_INSTANCE* Play(SOUNDID id , ALLEGRO_MIXER* mixer , float gain , float pan , float speed , ALLEGRO_PLAYMODE mode);
   
   ALLEGRO_SAMPLE* GetSample(SOUNDID id);
   
   void CullStoppedSounds();
};

//*/



#endif // Library_HPP




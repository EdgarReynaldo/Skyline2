




#ifndef Library_HPP
#define Library_HPP


#include <vector>
#include <string>

#include "allegro5/allegro.h"
#include "allegro5/allegro_audio.h"


class Sound {

public :
   
   std::string path;
   std::string file;
   
   ALLEGRO_SAMPLE* sample;
   
   std::vector<ALLEGRO_SAMPLE_INSTANCE*> instances;
   


   Sound();

   void Free();

   ALLEGRO_SAMPLE* Load(std::string path , std::string file);
   
   ALLEGRO_SAMPLE_INSTANCE* Play(ALLEGRO_MIXER* mixer , float gain , float pan , float speed , ALLEGRO_PLAYMODE mode);
   
   void CullStoppedSounds();
   
};



ALLEGRO_SAMPLE_INSTANCE* PlaySound(ALLEGRO_SAMPLE* sample , ALLEGRO_MIXER* mixer , float gain , float pan , float speed , ALLEGRO_PLAYMODE mode);



class SoundLibrary {
   
public :
   std::vector<Sound*> sounds;
   
///   SoundLibrary();

   void Free();

   int Load(std::string directory);
   


SoundLibrary() :
      sounds()
{}
~SoundLibrary() {Free();}


   ALLEGRO_SAMPLE_INSTANCE* Play(unsigned int sound_index , ALLEGRO_MIXER* mixer , float gain , float pan , float speed , ALLEGRO_PLAYMODE mode);
   
   ALLEGRO_SAMPLE* GetSample(int index);
   
   void CullStoppedSounds();
};

#endif // Library_HPP

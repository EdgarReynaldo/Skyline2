



#include "SoundMan.hpp"


const SOUNDID BADSOUNDID = ~0;



SOUNDID NewSoundId() {
   static SOUNDID cid = 0;
   return cid++;
}



Sound::Sound() :
      id(NewSoundId()),
      sample(0)
{}



Sound::~Sound() {Free();}



void Sound::Free() {
   if (sample) {
      al_destroy_sample(sample);
      sample = 0;
   }
}



bool Sound::Load(std::string sndfile) {
   Free();
   sample = al_load_sample(sndfile.c_str());
   return sample;
}



/// ------------------------      SoundManager     -----------------------------------



SOUNDID SoundManager::LoadSound(std::string sndfile) {
   Sound* s = new Sound();
   if (s->Load(sndfile)) {
      SOUNDID nsid = s->ID();
      sounds[nsid] = s;
      return nsid;
   }
   else {
      delete s;
   }
   return BADSOUNDID;
}

ALLEGRO_SAMPLE_INSTANCE* SoundManager::Play(SOUNDID sid , ALLEGRO_MIXER* mixer , 
                                            float gain , float pan , 
                                            float speed , ALLEGRO_PLAYMODE mode) 
{
   
   ALLEGRO_SAMPLE_INSTANCE* inst = 0;
   
   std::map<SOUNDID , Sound*>::iterator it = sounds.find(sid);
   if (it != sounds.end()) {
      inst = al_create_sample_instance(it->second->Sample());
      if (inst) {
         al_set_sample_instance_gain       (inst , gain );
         al_set_sample_instance_pan        (inst , pan  );
         al_set_sample_instance_speed      (inst , speed);
         al_set_sample_instance_playmode   (inst , mode );
         al_set_sample_instance_playing    (inst , true );
         al_attach_sample_instance_to_mixer(inst , mixer);
         instances.insert(std::pair<Sound* , ALLEGRO_SAMPLE_INSTANCE*>(it->second , inst));
      }
   }
   return inst;
}









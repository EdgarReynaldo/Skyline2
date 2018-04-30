


#include "SoundLibrary.hpp"

/**


SOUNDID GetNewSoundID() {
   static unsigned int sound_id = 0;
   return sound_id++;
}



Sound::Sound(const Sound& s) {
   (void)s;
}



Sound& Sound::operator=(const Sound& s) {
   (void)s;
   return *this;
}



Sound::Sound() :
      sid(GetNewSoundID()),
      path(""),
      file(""),
      sample(0),
      instances()
{}



void Sound::Free() {
   for (unsigned int i = 0 ; i < instances.size() ; ++i) {
      al_destroy_sample_instance(instances[i]);
   }
   instances.clear();
   if (sample) {
      al_destroy_sample(sample);
      sample = 0;
   }
}



ALLEGRO_SAMPLE* Sound::Load(std::string path , std::string file) {
   Free();
   std::string fpath = path + file;
   sample = al_load_sample(fpath.c_str());
   return sample;
}



ALLEGRO_SAMPLE_INSTANCE* Sound::Play(ALLEGRO_MIXER* mixer , float gain , float pan , float speed , ALLEGRO_PLAYMODE mode) {
   ALLEGRO_SAMPLE_INSTANCE* instance = PlaySound(sample , mixer , gain , pan , speed , mode);
   instances.push_back(instance);
   return instance;
}



ALLEGRO_SAMPLE_INSTANCE* PlaySound(ALLEGRO_SAMPLE* sample , ALLEGRO_MIXER* mixer , float gain , float pan , float speed , ALLEGRO_PLAYMODE mode) {
   ALLEGRO_SAMPLE_INSTANCE* instance = al_create_sample_instance(sample);
   al_attach_sample_instance_to_mixer(instance , mixer);
   al_set_sample_instance_gain(instance , gain);
   al_set_sample_instance_pan(instance , pan);
   al_set_sample_instance_speed(instance , speed);
   al_set_sample_instance_playmode(instance , mode);
   al_set_sample_instance_playing(instance , true);
   return instance;
}



void Sound::CullStoppedSounds() {
   std::vector<ALLEGRO_SAMPLE_INSTANCE*>::iterator it = instances.begin();
   while (it != instances.end()) {
      if (!al_get_sample_instance_playing(*it)) {
         it = instances.erase(it);
      }
      else {
         ++it;
      }
   }
}



/// -------------------------------      SoundLibrary      --------------------------------






void SoundLibrary::Free() {
   for (unsigned int i = 0 ; i < sounds.size() ; ++i) {
      delete sounds[i];
   }
   sounds.clear();
}



int SoundLibrary::Load(std::string directory) {
   Free();
   char buf[4096] = {0};
   std::string oldpath = getcwd(buf , 4095);
   
   al_change_directory(directory.c_str());
   ALLEGRO_FS_ENTRY* dir = al_create_fs_entry(directory.c_str());
   if (al_open_directory(dir)) {
      ALLEGRO_FS_ENTRY* fs = 0;
      while ((fs = al_read_directory(dir))) {
         Sound* newsound = new Sound();
         std::string fullpath = al_get_fs_entry_name(fs);
         printf("Found %s\n" , fullpath.c_str());
         unsigned int i = std::string::npos;
         if ((i = fullpath.find_last_of(ALLEGRO_NATIVE_PATH_SEP)) != std::string::npos) {
            std::string name = fullpath.substr(i + 1 , std::string::npos);
            std::string path = fullpath.substr(0 , i + 1);
            
            ALLEGRO_SAMPLE* sample = newsound->Load(path , name);
            if (sample) {
               sounds.push_back(newsound);
            }
            else {
               delete newsound;
            }
         }
         
         al_destroy_fs_entry(fs);
      }
      al_close_directory(dir);
   }
   
   al_change_directory(oldpath.c_str());
   
   return sounds.size();
}



ALLEGRO_SAMPLE_INSTANCE* SoundLibrary::Play(unsigned int sound_index , ALLEGRO_MIXER* mixer ,
                              float gain , float pan , float speed , ALLEGRO_PLAYMODE mode) {
   return sounds[sound_index]->Play(mixer , gain , pan , speed , mode);
}



ALLEGRO_SAMPLE* SoundLibrary::GetSample(int index) {
   return sounds[index]->sample;
}



void SoundLibrary::CullStoppedSounds() {
   for (int i = 0 ; i < (int)sounds.size() ; ++i) {
      sounds[i]->CullStoppedSounds();
   }
}

//*/


   
   
   

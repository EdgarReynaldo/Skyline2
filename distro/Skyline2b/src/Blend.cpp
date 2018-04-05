



#include "Blend.hpp"

#include "allegro5/allegro.h"



#include "GL/gl.h"
#include "GL/glu.h"


void SetAdditiveBlender() {
   
///   al_set_blender(ALLEGRO_ADD , ALLEGRO_ALPHA , ALLEGRO_DEST_COLOR);
   
   glBlendFunc(GL_ONE , GL_ONE);
   
///   glEnable(GL_BLEND);
///   glBlendEquationSeparate(GL_FUNC_ADD , GL_FUNC_ADD);
}


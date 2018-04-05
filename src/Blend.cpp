



#include "Blend.hpp"


#include "GL/gl.h"
#include "GL/glu.h"


void SetAdditiveBlender() {
   
   glBlendFunc(GL_ONE , GL_ONE);
   
///   glEnable(GL_BLEND);
///   glBlendEquationSeparate(GL_FUNC_ADD , GL_FUNC_ADD);
}


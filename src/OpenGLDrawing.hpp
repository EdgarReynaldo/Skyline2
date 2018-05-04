



#ifndef OpenGLDrawing_HPP
#define OpenGLDrawing_HPP

#include "Eagle/Color.hpp"
#include "Eagle/Position.hpp"



#include "GL/gl.h"

inline void eglColor(EagleColor c) {glColor4d(c.R() , c.G() , c.B() , c.A());}


void DrawJet(Pos2D p , double theta , EagleColor icol , EagleColor ocol);

#endif // OpenGLDrawing_HPP


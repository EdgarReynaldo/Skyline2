



#include "Utility.hpp"



Rectangle ScaleToFit(Rectangle scale , Rectangle fit) {
   double saspect = fit.W()/(double)fit.H();
   double faspect = fit.W()/(double)fit.H();
   
   double yscale = (double)fit.H()/scale.H();
   double xscale = (double)fit.W()/scale.W();
   double minscale = (xscale < yscale)?xscale:yscale;
   
   int newwidth = minscale*scale.W();
   int newheight = minscale*scale.H();
   
   /// Return a scaled to fit rectangle with the aspect of the scale rectangle, centered
   return Rectangle((fit.W() - newwidth)/2.0 , (fit.H() - newheight)/2.0 , newwidth , newheight);
}



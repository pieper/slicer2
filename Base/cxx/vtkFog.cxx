
#include "vtkFog.h"
#include "vtkRenderWindow.h"
#include "vtkObjectFactory.h"

#ifndef VTK_IMPLEMENT_MESA_CXX
#include <GL/gl.h>
#endif


//----------------------------------------------------------------------
//
//
vtkFog::vtkFog()
//      ------
{
  this->FogEnabled = 0;
  this->FogStart   = 0;
  this->FogEnd     = 100;

} // constructor vtkFog()

//----------------------------------------------------------------------
// return the correct type of Renderer 
//
vtkFog *vtkFog::New()
//              ---
{ 

  //  printf("vtkFog::New() \n");
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkFog");
  if(ret)
  {
    return (vtkFog*)ret;
  }

  return  new vtkFog;
} // New()



// Implement base class method.
void vtkFog::Render(vtkRenderer *ren)
{

  //  printf("vtkFog::Render() begin \n");

    vtkRenderWindow* renwin;

    renwin = ren->GetRenderWindow();
    renwin->MakeCurrent();

  // Fog,  Karl Krissian
  if (!this->FogEnabled)
    {
      //      fprintf(stderr,"glDisable(GL_FOG) %d ", this->Fog);
      glDisable (GL_FOG);
    }
  else 
    {
      //  fprintf(stderr,"glEnable(GL_FOG)");
      glEnable(GL_FOG);
      glFogi(  GL_FOG_MODE,  GL_LINEAR);
      glFogfv( GL_FOG_COLOR, (ren->GetBackground()));
               //      glFogf(  GL_FOG_DENSITY, _density);
      glHint(  GL_FOG_HINT,    GL_DONT_CARE);
      glFogf(  GL_FOG_START,   FogStart);
      glFogf(  GL_FOG_END,     FogEnd);
      
    }

  //  printf("vtkFog::Render() end \n");

}


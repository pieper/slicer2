/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================auto=*/

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
      float bg[3];
      for (int i = 0; i < 3; i++)
        {
        bg[i] = ren->GetBackground()[i];
        }
      glFogfv( GL_FOG_COLOR, bg);
               //      glFogf(  GL_FOG_DENSITY, _density);
      glHint(  GL_FOG_HINT,    GL_DONT_CARE);
      glFogf(  GL_FOG_START,   FogStart);
      glFogf(  GL_FOG_END,     FogEnd);
      
    }

  //  printf("vtkFog::Render() end \n");

}


/*=auto=========================================================================

(c) Copyright 2002 Massachusetts Institute of Technology

Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for any purpose, 
provided that the above copyright notice and the following three paragraphs 
appear on all copies of this software.  Use of this software constitutes 
acceptance of these terms and conditions.

IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================auto=*/

#ifndef __vtkFog_h
#define __vtkFog_h

#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"


class VTK_EXPORT vtkFog : public vtkObject
{
public:

  vtkTypeMacro(vtkFog,vtkObject);
  //  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Create Fog Effect
  static vtkFog *New();

  // Description:
  // Turn on/off fog. 
  vtkSetMacro(    FogEnabled,int);
  vtkGetMacro(    FogEnabled,int);
  vtkBooleanMacro(FogEnabled,int);




  // Description:
  //   GL_FOG_START        params is a single integer or  floating-
  //                       point  value  that  specifies start, the
  //                       near distance used  in  the  linear  fog
  //                       equation.   The initial near distance is
  //                       0.
  //  for more details, try man glFog
  // Set/Get the parameter FogStart
  vtkSetMacro(FogStart, float);
  vtkGetMacro(FogStart, float);

  // Description:
  //   GL_FOG_END          params is a single integer or  floating-
  //                       point  value that specifies end, the far
  //                       distance used in the  linear  fog  equa-
  //                       tion.  The initial far distance is 1.
  //  for more details, try man glFog
  // Set/Get the parameter FogEnd
  vtkSetMacro(FogEnd, float);
  vtkGetMacro(FogEnd, float);

  // Description:
  // Set the fog parameter to the specified Renderer
  // 
  void Render(vtkRenderer *);


protected:
  vtkFog();

  int   FogEnabled;
  float FogStart;
  float FogEnd;

};



#endif //  __vtkFog_h

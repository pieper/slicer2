/*=auto=========================================================================
Copyright (c) 2000 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy (but NOT distribute) this
software and its documentation for any NON-COMMERCIAL purpose, provided
that existing copyright notices are retained verbatim in all copies.
The authors grant permission to modify this software and its documentation 
for any NON-COMMERCIAL purpose, provided that such modifications are not 
distributed without the explicit consent of the authors and that existing
copyright notices are retained in all copies. Some of the algorithms
implemented by this software are patented, observe all applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
=========================================================================auto=*/
#ifndef __vtkMathUtils_h
#define __vtkMathUtils_h

#include "vtkObject.h"
#include "vtkPoints.h"
#include "vtkScalars.h"
#include "vtkVectors.h"
#include "vtkMatrix4x4.h"

class VTK_EXPORT vtkMathUtils : public vtkObject
{
public:
  static vtkMathUtils *New() {return new vtkMathUtils;};
  const char *GetClassName() {return "vtkMathUtils";};

  static int PrincipalMomentsAndAxes( vtkPoints *Points, vtkScalars *Weights,
                                      vtkScalars *Values, vtkVectors *Vectors );
  static int AlignPoints( vtkPoints *Data, vtkPoints *Ref,
                          vtkMatrix4x4 *Xform );
  static void SVD3x3( float A[][3], float U[][3], float W[], float V[][3] );
};

#endif

am/*=auto=========================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy, and distribute this
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
// .NAME vtkMrmlTransformNode - Writes images to files.
// .SECTION Description
// vtkMrmlTransformNode writes images to files with any data type. The data type of

#ifndef __vtkMrmlTransformNode_h
#define __vtkMrmlTransformNode_h

#include <iostream.h>
#include <fstream.h>
#include "vtkMrmlNode.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"

class VTK_EXPORT vtkMrmlTransformNode : public vtkMrmlNode
{
public:
  static vtkMrmlTransformNode *New();
  const char *GetClassName() {return "vtkMrmlTransformNode";};
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlTransformNode *node);

  // Description:
  // Get/Set for Transform
  vtkGetObjectMacro(Transform, vtkTransform);
  vtkSetObjectMacro(Transform, vtkTransform);

  // Description:
  // Rotate around each axis: x,y, and z in degrees
  void SetRotate(float *rotate)
    {this->Transform->RotateX(rotate[0]);
    this->Transform->RotateY(rotate[1]);
    this->Transform->RotateZ(rotate[2]);}


  // Description:
  // Translate by x= translate[0], y=tra
  void SetTranslate(float *translate)
    {this->Transform->Translate(translate[0], translate[1], translate[2]);}

  void SetScale(float *scale)
    {this->Transform->Scale(scale[0], scale[1], scale[2]);}

  vtkMrmlTransformNode();
  ~vtkMrmlTransformNode();
  
private:

  vtkMrmlTransformNode(const vtkMrmlTransformNode&) {};
  void operator=(const vtkMrmlTransformNode&) {};

  vtkTransform *Transform;
};

#endif


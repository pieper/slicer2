/*=auto=========================================================================
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
// .NAME vtkMrmlModelNode - Writes images to files.
// .SECTION Description
// 

#ifndef __vtkMrmlModelNode_h
#define __vtkMrmlModelNode_h

#include <iostream.h>
#include <fstream.h>
#include "vtkMrmlNode.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"

class VTK_EXPORT vtkMrmlModelNode : public vtkMrmlNode
{
public:
  static vtkMrmlModelNode *New();
  const char *GetClassName() {return "vtkMrmlModelNode";};
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Write the node's attributes
  void Write(ofstream& of, int indent);

  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlModelNode *node);

  // Description:
  // 
  vtkSetStringMacro(Name);
  vtkGetStringMacro(Name);

  // Description:
  // 
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  // Description:
  // 
  vtkSetStringMacro(FullFileName);
  vtkGetStringMacro(FullFileName);

  // Description:
  // 
  vtkSetStringMacro(Color);
  vtkGetStringMacro(Color);

  // Description:
  // 
  vtkSetMacro(Opacity, float);
  vtkGetMacro(Opacity, float);

  // Description:
  // 
  vtkSetVector2Macro(ScalarRange, int);
  vtkGetVector2Macro(ScalarRange, int);

  // Description:
  //
  vtkBooleanMacro(Visibility, int);
  vtkGetMacro(Visibility, int);
  vtkSetMacro(Visibility, int);

  // Description:
  //
  vtkBooleanMacro(ScalarVisibility, int);
  vtkGetMacro(ScalarVisibility, int);
  vtkSetMacro(ScalarVisibility, int);

  // Description:
  //
  vtkBooleanMacro(BackfaceCulling, int);
  vtkGetMacro(BackfaceCulling, int);
  vtkSetMacro(BackfaceCulling, int);

  // Description:
  //
  vtkBooleanMacro(Clipping, int);
  vtkGetMacro(Clipping, int);
  vtkSetMacro(Clipping, int);

  // Description:
  // 
  vtkSetStringMacro(RasToIjkMatrix);
  vtkGetStringMacro(RasToIjkMatrix);

  // Description:
  // 
  void SetRasToWld(vtkMatrix4x4 *reg);
  vtkGetObjectMacro(RasToWld, vtkMatrix4x4);

protected:
  vtkMrmlModelNode();
  ~vtkMrmlModelNode();
  vtkMrmlModelNode(const vtkMrmlModelNode&) {};
  void operator=(const vtkMrmlModelNode&) {};

  char *Name;
  char *FileName;
  char *FullFileName;
  char *Color;
  char *RasToIjkMatrix;

  float Opacity;

  int ScalarRange[2];

  int Visibility;
  int ScalarVisibility;
  int BackfaceCulling;
  int Clipping;

  vtkMatrix4x4 *RasToWld;
};

#endif


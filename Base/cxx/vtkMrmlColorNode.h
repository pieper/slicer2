/*=auto=========================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
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
// .NAME vtkMrmlColorNode - 
// .SECTION Description
// 

#ifndef __vtkMrmlColorNode_h
#define __vtkMrmlColorNode_h

#include <iostream.h>
#include <fstream.h>
#include "vtkMrmlNode.h"

class VTK_EXPORT vtkMrmlColorNode : public vtkMrmlNode
{
public:
  static vtkMrmlColorNode *New();
  const char *GetClassName() {return "vtkMrmlColorNode";};
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Write the node's attributes
  void Write(ofstream& of, int indent);

  // Description:
  // Copy the nodes attributes to this object
  void Copy(vtkMrmlColorNode *node);

  //--------------------------------------------------------------------------
  // Get and Set Functions
  //--------------------------------------------------------------------------

  // Description:
  // Get/Set for Name
  vtkGetStringMacro(Name);
  vtkSetStringMacro(Name);

  // Description:
  // Get/Set for Labels
  vtkGetStringMacro(Labels);
  vtkSetStringMacro(Labels);

  // Description:
  // Get/Set for Ambient
  vtkGetMacro(Ambient, float);
  vtkSetMacro(Ambient, float);

  // Description:
  // Get/Set for Diffuse
  vtkGetMacro(Diffuse, float);
  vtkSetMacro(Diffuse, float);

  // Description:
  // Get/Set for Specular
  vtkGetMacro(Specular, float);
  vtkSetMacro(Specular, float);

  // Description:
  // Get/Set for DiffuseColor
  vtkGetVector3Macro(DiffuseColor, float);
  vtkSetVector3Macro(DiffuseColor, float);

  // Description:
  // Get/Set for Power
  vtkGetMacro(Power, int);
  vtkSetMacro(Power, int);

protected:
  vtkMrmlColorNode();
  ~vtkMrmlColorNode();
  vtkMrmlColorNode(const vtkMrmlColorNode&) {};
  void operator=(const vtkMrmlColorNode&) {};

  char *Name;
  char *Labels;

  float Ambient;
  float Diffuse;
  float Specular;

  float DiffuseColor[3];

  int Power;
};

#endif










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
// .NAME vtkMrmlNode - Writes images to files.
// .SECTION Description
// vtkMrmlNode writes images to files with any data type. The data type of

#ifndef __vtkMrmlNode_h
#define __vtkMrmlNode_h

#include <iostream.h>
#include <fstream.h>
#include "vtkObject.h"
#include "vtkMatrix4x4.h"

class VTK_EXPORT vtkMrmlNode : public vtkObject
{
public:
  static vtkMrmlNode *New();
  const char *GetClassName() {return "vtkMrmlNode";};
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Copy the node's parameters to this object
  // except: ID
  void Copy(vtkMrmlNode *node);
  
  // Description:
  // Set/Get a numerical ID for the calling program to use to keep track
  // of its various volume objects.
  vtkSetMacro(ID, int);
  vtkGetMacro(ID, int);

  // Description:
  // Orientation of slices to edit
  vtkSetStringMacro(Description);
  vtkGetStringMacro(Description);

  // Description:
  // Orientation of slices to edit
  vtkSetStringMacro(Options);
  vtkGetStringMacro(Options);

  // Description:
  // Set/Get Ignore value of this node
  vtkGetMacro(Ignore, int);
  vtkSetMacro(Ignore, int);
  vtkBooleanMacro(Ignore, int);

  virtual void Write(ofstream& of, int indent) {};


protected:

  vtkMrmlNode();
  ~vtkMrmlNode();
  vtkMrmlNode(const vtkMrmlNode&) {};
  void operator=(const vtkMrmlNode&) {};

  void SetMatrixToString(vtkMatrix4x4 *m, char *s);
  char* GetMatrixToString(vtkMatrix4x4 *m);

  int ID;
  char *Description;
  char *Options;
  int Ignore;
};

#endif



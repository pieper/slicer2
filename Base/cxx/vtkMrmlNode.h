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
// .NAME vtkMrmlNode - Superclass for all specific types of MRML nodes.
// .SECTION Description
// This node excapsulates the functionality common to all types of MRML nodes.
// This includes member variables for ID, Description, and Options,
// as well as member functions to Copy() and Write().

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
  vtkTypeMacro(vtkMrmlNode,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Copy the node's parameters to this object.
  // But don't copy: ID
  void Copy(vtkMrmlNode *node);
  
  // Description:
  // Set/Get a numerical ID for the calling program to use to keep track
  // of its various volume objects.
  vtkSetMacro(ID, int);
  vtkGetMacro(ID, int);

  // Description:
  // Text description
  vtkSetStringMacro(Description);
  vtkGetStringMacro(Description);

  // Description:
  // Optional attributes not defined in the MRML standard,
  // but recognized by various browsers
  vtkSetStringMacro(Options);
  vtkGetStringMacro(Options);

  // Description:
  // Utility functions for converting between vtkMatrix4x4 and
  // a string of 16 numbers in row-major order.
  void SetMatrixToString(vtkMatrix4x4 *m, char *s);
  char* GetMatrixToString(vtkMatrix4x4 *m);

  // Description:
  // Write this node's information to a MRML file in XML format.
  // Only write attributes that differ from the default values,
  // which are set in the node's constructor.
  // This is a virtual function that all subclasses must overload.
  virtual void Write(ofstream& of, int indent) {};

protected:

  vtkMrmlNode();
  ~vtkMrmlNode();
  vtkMrmlNode(const vtkMrmlNode&) {};
  void operator=(const vtkMrmlNode&) {};

  int ID;
  char *Description;
  char *Options;
};

#endif



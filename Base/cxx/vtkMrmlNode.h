/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for internal 
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
// .NAME vtkMrmlNode - Abstract Superclass for all specific types of MRML nodes.
// .SECTION Description
// This node encapsulates the functionality common to all types of MRML nodes.
// This includes member variables for ID, Description, and Options,
// as well as member functions to Copy() and Write().

#ifndef __vtkMrmlNode_h
#define __vtkMrmlNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkObject.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlNode : public vtkObject
{
  public:
    static vtkMrmlNode *New();
  vtkTypeMacro(vtkMrmlNode,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Copy a MrmlNode's parameters.
  // But don't copy: ID
  void MrmlNodeCopy(vtkMrmlNode *node);

  // Description:
  // Copy everything from another node of the same type.
  // Instances of vtkMrmlNode must define the Copy function.
  // Instances of vtkMrmlNode::Copy should call vtkMrmlNode::CopyMrmlNode
  virtual void Copy(vtkMrmlNode *node) = 0;
  
  // Description:
  // Set/Get a numerical ID for the calling program to use to keep track
  // of its various node objects.
  vtkSetMacro(ID, int);
  vtkGetMacro(ID, int);

  // Description:
  // Text description of this node, to be set by the user
  vtkSetStringMacro(Description);
  vtkGetStringMacro(Description);

  // Description:
  // Name of this node, to be set by the user
  vtkSetStringMacro(Name);
  vtkGetStringMacro(Name);

  // Description:
  // The Title is for use when graphically displaying the 
  // contents of a MRML file.
  char *GetTitle();

  // Description:
  // Node's effect on indentation when displaying the
  // contents of a MRML file. (0, +1, -1)
  vtkGetMacro(Indent, int);

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
  virtual void Write(ofstream& of, int indent);

protected:

  vtkSetMacro(Indent, int);
  vtkMrmlNode();
  // critical to have a virtual descructor!
  virtual ~vtkMrmlNode();
  vtkMrmlNode(const vtkMrmlNode&) {};
  void operator=(const vtkMrmlNode&) {};

  // Description:
  // Only for internal use by the GetTitle function
  vtkSetStringMacro(Title);

  int ID;
  int Indent;
  char *Description;
  char *Options;
  char *Name;
  char *Title;
};

#endif




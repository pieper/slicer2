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
// .NAME vtkMrmlHierarchyNode - MRML node to represent an anatomical hierarchy.
// .SECTION Description
// Hierarchy nodes begin the descriptions of anatomical model hierarchies.
// The hierarchy descriptions have to be followed by EndHierarchy nodes.

#ifndef __vtkMrmlHierarchyNode_h
#define __vtkMrmlHierarchyNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkMrmlNode.h"


class VTK_EXPORT vtkMrmlHierarchyNode : public vtkMrmlNode
{
public:
  static vtkMrmlHierarchyNode *New();
  vtkTypeMacro(vtkMrmlHierarchyNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);

  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlNode *node);

  // Description:
  // Hierarchy ID
  vtkSetStringMacro(HierarchyID);
  vtkGetStringMacro(HierarchyID);

  // Description:
  // Hierarchy type
  vtkSetStringMacro(Type);
  vtkGetStringMacro(Type);

 
protected:
  vtkMrmlHierarchyNode();
  ~vtkMrmlHierarchyNode();
  vtkMrmlHierarchyNode(const vtkMrmlHierarchyNode&) {};
  void operator=(const vtkMrmlHierarchyNode&) {};

  // Strings
  char *HierarchyID;
  char *Type;

};

#endif

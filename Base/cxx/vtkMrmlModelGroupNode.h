/*=auto=========================================================================

(c) Copyright 2001 Massachusetts Institute of Technology

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
// .NAME vtkMrmlModelGroupNode - MRML node to represent a model group.
// .SECTION Description
// Model groups appear inside model hierarchies to group models that belong
// together. Model groups are followed by EndModelGroup nodes.

#ifndef __vtkMrmlModelGroupNode_h
#define __vtkMrmlModelGroupNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkMrmlNode.h"


class VTK_EXPORT vtkMrmlModelGroupNode : public vtkMrmlNode
{
public:
  static vtkMrmlModelGroupNode *New();
  vtkTypeMacro(vtkMrmlModelGroupNode,vtkMrmlNode);
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
  // ID of the model group
  vtkSetStringMacro(ModelGroupID);
  vtkGetStringMacro(ModelGroupID);
  
  // Description:
  // Name of the model group's color, which is defined by a Color node
  vtkSetStringMacro(Color);
  vtkGetStringMacro(Color);
  
  // Description:
  // Opacity of the surface expressed as a number from 0 to 1
  vtkSetMacro(Opacity, float);
  vtkGetMacro(Opacity, float);
  
  // Description:
  // Indicates if the surface is visible
  vtkBooleanMacro(Visibility, int);
  vtkSetMacro(Visibility, int);
  vtkGetMacro(Visibility, int);

  // Description:
  // Indicates if the ModelGroup is displayed expanded or collapsed
  vtkBooleanMacro(Expansion, int);
  vtkSetMacro(Expansion, int);
  vtkGetMacro(Expansion, int);
   
protected:
  vtkMrmlModelGroupNode();
  ~vtkMrmlModelGroupNode();
  vtkMrmlModelGroupNode(const vtkMrmlModelGroupNode&) {};
  void operator=(const vtkMrmlModelGroupNode&) {};

  // Strings
  char *ModelGroupID;
  char *Color;
  
  // Numbers
  float Opacity;
  
  // Booleans
  int Visibility;
  int Expansion;

};

#endif

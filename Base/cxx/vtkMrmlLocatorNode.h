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
// .NAME vtkMrmlLocatorNode - describes locator properties.
// .SECTION Description
// Locator nodes describe the different options of the locator inside Slicer.

#ifndef __vtkMrmlLocatorNode_h
#define __vtkMrmlLocatorNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkMrmlNode.h"


class VTK_EXPORT vtkMrmlLocatorNode : public vtkMrmlNode
{
public:
  static vtkMrmlLocatorNode *New();
  vtkTypeMacro(vtkMrmlLocatorNode,vtkMrmlNode);
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
  // Who or what is driving the locator?
  vtkSetStringMacro(Driver);
  vtkGetStringMacro(Driver);
  
  // Description:
  // Is the locator visible?
  vtkBooleanMacro(Visibility,int);
  vtkSetMacro(Visibility,int);
  vtkGetMacro(Visibility,int);
  
  // Description:
  // Is the transverse visible?
  vtkBooleanMacro(TransverseVisibility,int);
  vtkSetMacro(TransverseVisibility,int);
  vtkGetMacro(TransverseVisibility,int);
  
  // Description:
  // Length of the normal
  vtkSetMacro(NormalLen,int);
  vtkGetMacro(NormalLen,int);
  
  // Description:
  // Lenght of the transverse
  vtkSetMacro(TransverseLen,int);
  vtkGetMacro(TransverseLen,int);
  
  // Description:
  // Radius
  vtkSetMacro(Radius,float);
  vtkGetMacro(Radius,float);
  
  // Description:
  // Locator color
  vtkSetStringMacro(DiffuseColor);
  vtkGetStringMacro(DiffuseColor);

 
protected:
  vtkMrmlLocatorNode();
  ~vtkMrmlLocatorNode();
  vtkMrmlLocatorNode(const vtkMrmlLocatorNode&) {};
  void operator=(const vtkMrmlLocatorNode&) {};

  // Strings
  char *Driver;
  char *DiffuseColor;

  // Numbers
  int Visibility;
  int TransverseVisibility;
  int NormalLen;
  int TransverseLen;
  float Radius;
};

#endif

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
// .NAME vtkMrmlModelStateNode - MRML node to represent the properties of a model.
// .SECTION Description
// ModelState nodes save the properties of a model inside a Slicer scene. They
// can also be used for model groups.

#ifndef __vtkMrmlModelStateNode_h
#define __vtkMrmlModelStateNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkMrmlNode.h"


class VTK_EXPORT vtkMrmlModelStateNode : public vtkMrmlNode
{
public:
  static vtkMrmlModelStateNode *New();
  vtkTypeMacro(vtkMrmlModelStateNode,vtkMrmlNode);
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
  // ID of the referenced model
  vtkSetStringMacro(ModelRefID);
  vtkGetStringMacro(ModelRefID);
  
  // Description:
  // Visibility
  vtkBooleanMacro(Visible,int);
  vtkSetMacro(Visible,int);
  vtkGetMacro(Visible,int);
  
  // Description:
  // Visibility of the opacity slider
  // This is not used in Slicer, but only saved to maintain compatibility
  // with SPLViz
  vtkBooleanMacro(SliderVisible,int);
  vtkSetMacro(SliderVisible,int);
  vtkGetMacro(SliderVisible,int);
  
  // Description:
  // Visibility of the children (for model groups)
  vtkBooleanMacro(SonsVisible,int);
  vtkSetMacro(SonsVisible,int);
  vtkGetMacro(SonsVisible,int);
  
  // Description:
  // Opacity
  vtkSetMacro(Opacity,float);
  vtkGetMacro(Opacity,float);

  // Description:
  // Is clipping active?
  vtkBooleanMacro(Clipping,int);
  vtkSetMacro(Clipping,int);
  vtkGetMacro(Clipping,int);
  
  // Description:
  // Is backface culling active?
  vtkBooleanMacro(BackfaceCulling,int);
  vtkSetMacro(BackfaceCulling,int);
  vtkGetMacro(BackfaceCulling,int); 
  
  
protected:
  vtkMrmlModelStateNode();
  ~vtkMrmlModelStateNode();
  vtkMrmlModelStateNode(const vtkMrmlModelStateNode&) {};
  void operator=(const vtkMrmlModelStateNode&) {};

  // Strings
  char *ModelRefID;
  
  // Numbers
  float Opacity;
  
  // Booleans
  int Visible;
  int SonsVisible;
  int SliderVisible;
  int Clipping;
  int BackfaceCulling;

};

#endif

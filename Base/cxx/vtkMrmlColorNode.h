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
// .NAME vtkMrmlColorNode - MRML node for representing colors.
// .SECTION Description
// Color nodes define colors by describing not only the actual color 
// value, but also its name and a list of label values.  One attribute of 
// model nodes is the name of its color.  When the 3D Slicer displays 
// label maps, it colors each voxel by looking up the color associated 
// with that label value. Thus, when label maps are displayed on 
// reformatted slices, their colors automatically match the corresponding 
// surface models in the 3D view.
 

#ifndef __vtkMrmlColorNode_h
#define __vtkMrmlColorNode_h

#include <iostream.h>
#include <fstream.h>
#include "vtkMrmlNode.h"

class VTK_EXPORT vtkMrmlColorNode : public vtkMrmlNode
{
public:
  static vtkMrmlColorNode *New();
  vtkTypeMacro(vtkMrmlColorNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Write the node's attributes
  void Write(ofstream& of, int indent);

  // Description:
  // Copy the nodes attributes to this object to a MRML file in XML format
  void Copy(vtkMrmlColorNode *node);

  //--------------------------------------------------------------------------
  // Get and Set Functions
  //--------------------------------------------------------------------------

  // Description:
  // Get/Set for DiffuseColor
  vtkGetVector3Macro(DiffuseColor, float);
  vtkSetVector3Macro(DiffuseColor, float);

  // Description:
  // Factor of the affect of ambient light from 0 to 1
  vtkGetMacro(Ambient, float);
  vtkSetMacro(Ambient, float);

  // Description:
  // Factor of the affect of diffuse reflection from 0 to 1
  vtkGetMacro(Diffuse, float);
  vtkSetMacro(Diffuse, float);

  // Description:
  // Factor of the affect of specular reflection from 0 to 1
  vtkGetMacro(Specular, float);
  vtkSetMacro(Specular, float);

  // Description:
  // Specular power in the range of 1 to 100
  vtkGetMacro(Power, int);
  vtkSetMacro(Power, int);

  // Description:
  // List of label values associated with this color
  vtkGetStringMacro(Labels);
  vtkSetStringMacro(Labels);

protected:
  vtkMrmlColorNode();
  ~vtkMrmlColorNode();
  vtkMrmlColorNode(const vtkMrmlColorNode&) {};
  void operator=(const vtkMrmlColorNode&) {};

  // Strings
  char *Labels;

  // Numbers
  float Ambient;
  float Diffuse;
  float Specular;
  int Power;

  // Arrays
  float DiffuseColor[3];
};

#endif










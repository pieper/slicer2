/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
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

//#include <iostream.h>
//#include <fstream.h>
#include "vtkSlicer.h"
#include "vtkMrmlNode.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlColorNode : public vtkMrmlNode
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
  void Copy(vtkMrmlNode *node);

  //--------------------------------------------------------------------------
  // Get and Set Functions
  //--------------------------------------------------------------------------

  // Description:
  // Get/Set for DiffuseColor
  vtkGetVector3Macro(DiffuseColor, vtkFloatingPointType);
  vtkSetVector3Macro(DiffuseColor, vtkFloatingPointType);

  // Description:
  // Factor of the affect of ambient light from 0 to 1
  vtkGetMacro(Ambient, vtkFloatingPointType);
  vtkSetMacro(Ambient, vtkFloatingPointType);

  // Description:
  // Factor of the affect of diffuse reflection from 0 to 1
  vtkGetMacro(Diffuse, vtkFloatingPointType);
  vtkSetMacro(Diffuse, vtkFloatingPointType);

  // Description:
  // Factor of the affect of specular reflection from 0 to 1
  vtkGetMacro(Specular, vtkFloatingPointType);
  vtkSetMacro(Specular, vtkFloatingPointType);

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
  vtkFloatingPointType Ambient;
  vtkFloatingPointType Diffuse;
  vtkFloatingPointType Specular;
  int Power;

  // Arrays
  vtkFloatingPointType DiffuseColor[3];
};

#endif










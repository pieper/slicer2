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
// .NAME vtkMrmlFiducialsNode - MRML node for representing a
// constellation of vtkMrmlPoints.
// .SECTION Description
// A Fiducials is container for a constellation of vtkMrmlPoints

#ifndef __vtkMrmlFiducialsNode_h
#define __vtkMrmlFiducialsNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkMrmlNode.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlFiducialsNode : public vtkMrmlNode
{
public:
  static vtkMrmlFiducialsNode *New();
  vtkTypeMacro(vtkMrmlFiducialsNode,vtkMrmlNode);
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
  // Get/Set for Symbol size
  vtkSetMacro(SymbolSize,float);
  vtkGetMacro(SymbolSize,float);


  // Description:
  // Get/Set for Symbol size
  vtkSetMacro(Visibility,float);
  vtkGetMacro(Visibility,float);

  // Description:
  // Get/Set for Text size
  vtkSetMacro(TextSize,float);
  vtkGetMacro(TextSize,float);

  vtkSetStringMacro(Type);
  vtkGetStringMacro(Type);

void SetTypeToEndoscopic();
void SetTypeToMeasurement();
void SetTypeToDefault();



protected:
  vtkMrmlFiducialsNode();
  ~vtkMrmlFiducialsNode();
  vtkMrmlFiducialsNode(const vtkMrmlFiducialsNode&) {};
  void operator=(const vtkMrmlFiducialsNode&) {};

float SymbolSize;
float TextSize;
int Visibility;
char  *Type;

};

#endif


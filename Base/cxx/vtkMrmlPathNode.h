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
// .NAME vtkMrmlPathNode - MRML node to represent a path.
// .SECTION Description
//

#ifndef __vtkMrmlPathNode_h
#define __vtkMrmlPathNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkMrmlNode.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlPathNode : public vtkMrmlNode
{
public:
  static vtkMrmlPathNode *New();
  vtkTypeMacro(vtkMrmlPathNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);

  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlNode *node);


  // Description:
  // Name of the Camera Path (cPath) color, which is defined by a Color node in a MRML file
 //  vtkSetStringMacro(cPathColor);
 // vtkGetStringMacro(cPathColor);

  // Description:
  // Name of the Focal Point Path (fPath) color, which is defined by a Color node in a MRML file
  // vtkSetStringMacro(fPathColor);
  // vtkGetStringMacro(fPathColor);

  // Description:
  // Name of the Camera Landmarks (cLand) color, which is defined by a Color node in a MRML file
  // vtkSetStringMacro(cLandColor);
  // vtkGetStringMacro(cLandColor);

  // Description:
  // Name of the Focal Point Landmarks (fPath) color, which is defined by a Color node in a MRML file
  // vtkSetStringMacro(fLandColor);
  // vtkGetStringMacro(fLandColor);

protected:
  vtkMrmlPathNode();
  ~vtkMrmlPathNode();
  vtkMrmlPathNode(const vtkMrmlPathNode&) {};
  void operator=(const vtkMrmlPathNode&) {};

//  char *cPathColor;
//  char *fPathColor;
//  char *cLandColor;
//  char *fLandColor;

};

#endif


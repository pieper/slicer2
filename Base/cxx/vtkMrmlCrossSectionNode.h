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
// .NAME vtkMrmlCrossSectionNode - MRML node to represent the properties of a cross section
// .SECTION Description
// CrossSection node describe the properties of the three slices that are
// displayed on the screen. Therefore, the position of a cross section must
// currently be 0, 1 or 2.

#ifndef __vtkMrmlCrossSectionNode_h
#define __vtkMrmlCrossSectionNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkMrmlNode.h"
#include "vtkSlicer.h"


class VTK_SLICER_BASE_EXPORT vtkMrmlCrossSectionNode : public vtkMrmlNode
{
public:
  static vtkMrmlCrossSectionNode *New();
  vtkTypeMacro(vtkMrmlCrossSectionNode,vtkMrmlNode);
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
  // Which cross section is referenced?
  vtkSetMacro(Position,int);
  vtkGetMacro(Position,int);
  
  // Description:
  // Direction
  vtkSetStringMacro(Direction);
  vtkGetStringMacro(Direction);
  
  // Description:
  // Position of the slider
  vtkSetMacro(SliceSlider,int);
  vtkGetMacro(SliceSlider,int);
  
  // Description:
  // Rotation X
  // (not used in Slicer, only for SPLViz compatibility)
  vtkSetMacro(RotatorX,int);
  vtkGetMacro(RotatorX,int);
  
  // Description:
  // Rotation Y
  // (not used in Slicer, only for SPLViz compatibility)
  vtkSetMacro(RotatorY,int);
  vtkGetMacro(RotatorY,int);
  
  // Description:
  // Zoom
  vtkSetMacro(Zoom,float);
  vtkGetMacro(Zoom,float);
  
  // Description:
  // Visibility in 3D view
  vtkBooleanMacro(InModel,int);
  vtkSetMacro(InModel,int);
  vtkGetMacro(InModel,int);
  
  // Description:
  // Background volume
  vtkSetStringMacro(BackVolRefID);
  vtkGetStringMacro(BackVolRefID);
  
  // Description:
  // Foreground volume
  vtkSetStringMacro(ForeVolRefID);
  vtkGetStringMacro(ForeVolRefID);
  
  // Description:
  // Label volume
  vtkSetStringMacro(LabelVolRefID);
  vtkGetStringMacro(LabelVolRefID);
  
  // Description:
  // Clip Type Union or Intersection
  vtkSetStringMacro(ClipType);
  vtkGetStringMacro(ClipType);

  // Description:
  // Clipping state
  //vtkBooleanMacro(ClipState,int);
  vtkSetMacro(ClipState,int);
  vtkGetMacro(ClipState,int);
 
protected:
  vtkMrmlCrossSectionNode();
  ~vtkMrmlCrossSectionNode();
  vtkMrmlCrossSectionNode(const vtkMrmlCrossSectionNode&) {};
  void operator=(const vtkMrmlCrossSectionNode&) {};

  // Strings
  char *Direction;
  char *BackVolRefID;
  char *ForeVolRefID;
  char *LabelVolRefID;
  char *ClipType; // Union or Intersection
  
  // Numbers
  int Position;
  int SliceSlider;
  int RotatorX;
  int RotatorY;
  float Zoom;
  
  // Booleans
  int InModel;

  // int flag - 0, 1, 2
  int ClipState;

};

#endif

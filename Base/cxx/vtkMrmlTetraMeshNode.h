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
// .NAME vtkMrmlTetraMeshNode - MRML node for representing a TetraMesh.
// .SECTION Description
// TetraMesh nodes describe data sets that can be thought of as stacks of 2D 
// images that form a 3D TetraMesh.  TetraMesh nodes describe where the images 
// are stored on disk, how to render the data (window and level), and how 
// to read the files.  This information is extracted from the image 
// headers (if they exist) at the time the MRML file is generated.  
// Consequently, MRML files isolate MRML browsers from understanding how 
// to read the myriad of file formats for medical data. 

#ifndef __vtkMrmlTetraMeshNode_h
#define __vtkMrmlTetraMeshNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkMrmlNode.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlTetraMeshNode : public vtkMrmlNode
{
  public:
  static vtkMrmlTetraMeshNode *New();
  vtkTypeMacro(vtkMrmlTetraMeshNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlNode *node);

  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);

  //--------------------------------------------------------------------------
  // Non-Header Information
  //--------------------------------------------------------------------------
  
  // Description:
  // TetraMesh ID
  vtkSetStringMacro(TetraMeshID);
  vtkGetStringMacro(TetraMeshID);
  
  // Description:
  // The FileName to save the data to
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  //--------------------------------------------------------------------------
  // Display Information
  //--------------------------------------------------------------------------
  
  // Description:
  // Numerical ID of the color lookup table to use for rendering the TetraMesh
  vtkSetStringMacro(LUTName);
  vtkGetStringMacro(LUTName);

  // Description:
  // Opacity of display, from 0 to 1
  vtkSetMacro(Opacity, float);
  vtkGetMacro(Opacity, float);

  // Description:
  // Opacity of display, from 0 to 1
  vtkSetMacro(Clipping, unsigned short);
  vtkGetMacro(Clipping, unsigned short);

  // Description:
  // Display these things?
  vtkSetMacro(DisplaySurfaces, unsigned short);
  vtkGetMacro(DisplaySurfaces, unsigned short);
  vtkSetMacro(DisplayEdges, unsigned short);
  vtkGetMacro(DisplayEdges, unsigned short);
  vtkSetMacro(DisplayNodes, unsigned short);
  vtkGetMacro(DisplayNodes, unsigned short);
  vtkSetMacro(DisplayScalars, unsigned short);
  vtkGetMacro(DisplayScalars, unsigned short);
  vtkSetMacro(DisplayVectors, unsigned short);
  vtkGetMacro(DisplayVectors, unsigned short);

  // Description
  // Surfaces Use Cell Data or Point Data?
  vtkSetMacro(SurfacesUseCellData, unsigned short);
  vtkGetMacro(SurfacesUseCellData, unsigned short);

  // Description
  // Surfaces Smooth Normals to look good? Or, don't bother?
  vtkSetMacro(SurfacesSmoothNormals, unsigned short);
  vtkGetMacro(SurfacesSmoothNormals, unsigned short);

  // Description:
  // Parameters to display
  vtkSetMacro(NodeScaling, float);
  vtkGetMacro(NodeScaling, float);
  vtkSetMacro(NodeSkip, unsigned short);
  vtkGetMacro(NodeSkip, unsigned short);

  vtkSetMacro(ScalarScaling, float);
  vtkGetMacro(ScalarScaling, float);
  vtkSetMacro(ScalarSkip, unsigned short);
  vtkGetMacro(ScalarSkip, unsigned short);

  vtkSetMacro(VectorScaling, float);
  vtkGetMacro(VectorScaling, float);
  vtkSetMacro(VectorSkip, unsigned short);
  vtkGetMacro(VectorSkip, unsigned short);

protected:
  vtkMrmlTetraMeshNode();
  ~vtkMrmlTetraMeshNode();
  vtkMrmlTetraMeshNode(const vtkMrmlTetraMeshNode&) {};
  void operator=(const vtkMrmlTetraMeshNode&) {};

  void ComputeRasToIjk(vtkMatrix4x4 *matRotate, 
    float ox, float oy, float oz);

  // Strings
  char *TetraMeshID;
  char *FileName;
  char *LUTName;

  float Opacity;
  unsigned short Clipping;
  
  // Numbers
  unsigned short DisplaySurfaces;
  unsigned short   SurfacesUseCellData;
  unsigned short   SurfacesSmoothNormals;
  unsigned short DisplayEdges;
  unsigned short DisplayNodes;
  float            NodeScaling;
  unsigned short   NodeSkip;
  unsigned short DisplayScalars;
  float            ScalarScaling;
  unsigned short   ScalarSkip;
  unsigned short DisplayVectors;
  float            VectorScaling;
  unsigned short   VectorSkip;
};

#endif


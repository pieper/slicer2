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
// .NAME vtkMrmlSceneOptionsNode - MRML node to represent the scene options
// .SECTION Description
// SceneOptions node describe general properties of MRML scenes.

#ifndef __vtkMrmlSceneOptionsNode_h
#define __vtkMrmlSceneOptionsNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkMrmlNode.h"
#include "vtkSlicer.h"


class VTK_SLICER_BASE_EXPORT vtkMrmlSceneOptionsNode : public vtkMrmlNode
{
public:
  static vtkMrmlSceneOptionsNode *New();
  vtkTypeMacro(vtkMrmlSceneOptionsNode,vtkMrmlNode);
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
  // ViewUp (vtk camera option)
  vtkSetStringMacro(ViewUp);
  vtkGetStringMacro(ViewUp);
  
  // Description:
  // Position (vtk camera option)
  vtkSetStringMacro(Position);
  vtkGetStringMacro(Position);
  
  // Description:
  // FocalPoint (vtk camera option)
  vtkSetStringMacro(FocalPoint);
  vtkGetStringMacro(FocalPoint);
  
  // Description:
  // Clipping Range (3D view)
  vtkSetStringMacro(ClippingRange);
  vtkGetStringMacro(ClippingRange);
  
  // Description:
  // View mode (3D, 4x512, 4x256, etc.)
  vtkSetStringMacro(ViewMode);
  vtkGetStringMacro(ViewMode);
  
  // Description:
  // Background color of the 3D view
  vtkSetStringMacro(ViewBgColor);
  vtkGetStringMacro(ViewBgColor);

    // Description:
    // Resolution of the texture
    vtkSetStringMacro(ViewTextureResolution);
    vtkGetStringMacro(ViewTextureResolution);
    
    // Description:
    // Interpolate the slices On or Off
    vtkSetStringMacro(ViewTextureInterpolation);
    vtkGetStringMacro(ViewTextureInterpolation);
  
  // Description:
  // Show 3D axes?
  vtkBooleanMacro(ShowAxes,int);
  vtkSetMacro(ShowAxes,int);
  vtkGetMacro(ShowAxes,int);
  
  // Description:
  // Show 3D cube?
  vtkBooleanMacro(ShowBox,int);
  vtkSetMacro(ShowBox,int);
  vtkGetMacro(ShowBox,int);

  // Description:
  // Show slice window text?
  vtkBooleanMacro(ShowAnnotations,int);
  vtkSetMacro(ShowAnnotations,int);
  vtkGetMacro(ShowAnnotations,int);

  // Description:
  // Show 3D outline around slices?
  vtkBooleanMacro(ShowSliceBounds,int);
  vtkSetMacro(ShowSliceBounds,int);
  vtkGetMacro(ShowSliceBounds,int);
  
  // Description:
  // Show 3D letters?
  vtkBooleanMacro(ShowLetters,int);
  vtkSetMacro(ShowLetters,int);
  vtkGetMacro(ShowLetters,int);

  // Description:
  // Show crosshair?
  vtkBooleanMacro(ShowCross,int);
  vtkSetMacro(ShowCross,int);
  vtkGetMacro(ShowCross,int);
  
  // Description:
  // Show hash marks?
  vtkBooleanMacro(ShowHashes,int);
  vtkSetMacro(ShowHashes,int);
  vtkGetMacro(ShowHashes,int);

  // Description:
  // not used in Slicer, only for SPLViz compatibility
  vtkBooleanMacro(ShowMouse,int);
  vtkSetMacro(ShowMouse,int);
  vtkGetMacro(ShowMouse,int);
  
  // Description:
  // DICOM start directory
  vtkSetStringMacro(DICOMStartDir);
  vtkGetStringMacro(DICOMStartDir);
  
  // Description:
  // File name sort parameter
  vtkSetStringMacro(FileNameSortParam);
  vtkGetStringMacro(FileNameSortParam);
  
  // Description:
  // DICOM data dictionary
  vtkSetStringMacro(DICOMDataDictFile);
  vtkGetStringMacro(DICOMDataDictFile);
  
  // Description:
  // Width of DICOM preview
  vtkSetMacro(DICOMPreviewWidth,int);
  vtkGetMacro(DICOMPreviewWidth,int);
  
  // Description:
  // Height of DICOM preview
  vtkSetMacro(DICOMPreviewHeight,int);
  vtkGetMacro(DICOMPreviewHeight,int);
  
  // Description:
  // Highest value in DICOM preview
  vtkSetMacro(DICOMPreviewHighestValue,int);
  vtkGetMacro(DICOMPreviewHighestValue,int);
 
protected:
  vtkMrmlSceneOptionsNode();
  ~vtkMrmlSceneOptionsNode();
  vtkMrmlSceneOptionsNode(const vtkMrmlSceneOptionsNode&) {};
  void operator=(const vtkMrmlSceneOptionsNode&) {};

  // Strings
  char *ViewUp;
  char *Position;
  char *FocalPoint;
  char *ClippingRange;
  char *ViewMode;
  char *ViewBgColor;
  char *DICOMStartDir;
  char *FileNameSortParam;
  char *DICOMDataDictFile;
    char *ViewTextureResolution;
    char *ViewTextureInterpolation;
    
  // Booleans and Numbers
  int ShowAxes;
  int ShowBox;
  int ShowAnnotations;
  int ShowSliceBounds;
  int ShowLetters;
  int ShowCross;
  int ShowHashes;
  int ShowMouse;
  int DICOMPreviewWidth;
  int DICOMPreviewHeight;
  int DICOMPreviewHighestValue;

};

#endif

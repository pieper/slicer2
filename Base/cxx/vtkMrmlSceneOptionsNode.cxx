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
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "vtkMrmlSceneOptionsNode.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMrmlSceneOptionsNode* vtkMrmlSceneOptionsNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlSceneOptionsNode");
  if(ret)
  {
    return (vtkMrmlSceneOptionsNode*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlSceneOptionsNode;
}

//----------------------------------------------------------------------------
vtkMrmlSceneOptionsNode::vtkMrmlSceneOptionsNode()
{
  // Strings
  this->ViewUp = NULL;
  this->Position = NULL;
  this->FocalPoint = NULL;
  this->ClippingRange = NULL;
  this->ViewMode = NULL;
  this->ViewBgColor = NULL;
  this->DICOMStartDir = NULL;
  this->FileNameSortParam = NULL;
  this->DICOMDataDictFile = NULL;
  
  // Numbers
  this->ShowAxes = 0;
  this->ShowBox = 1;
  this->ShowAnnotations = 1;
  this->ShowSliceBounds = 0;
  this->ShowLetters = 1;
  this->ShowCross = 1;
  this->ShowHashes = 1;
  this->ShowMouse = 1;
  this->DICOMPreviewWidth = 64;
  this->DICOMPreviewHeight = 64;
  this->DICOMPreviewHighestValue = 2048;
}

//----------------------------------------------------------------------------
vtkMrmlSceneOptionsNode::~vtkMrmlSceneOptionsNode()
{
  if (this->ViewUp)
  {
    delete [] this->ViewUp;
    this->ViewUp=NULL;
  }
  if (this->Position)
  {
    delete [] this->Position;
    this->Position=NULL;
  }
  if (this->FocalPoint)
  {
    delete [] this->FocalPoint;
    this->FocalPoint=NULL;
  }
  if (this->ClippingRange)
  {
    delete [] this->ClippingRange;
    this->ClippingRange=NULL;
  }
  if (this->ViewMode)
  {
    delete [] this->ViewMode;
    this->ViewMode=NULL;
  }
  if (this->ViewBgColor)
  {
    delete [] this->ViewBgColor;
    this->ViewBgColor=NULL;
  }
  if (this->DICOMStartDir)
  {
    delete [] this->DICOMStartDir;
    this->DICOMStartDir=NULL;
  }
  if (this->FileNameSortParam)
  {
    delete [] this->FileNameSortParam;
    this->FileNameSortParam=NULL;
  }
  if (this->DICOMDataDictFile)
  {
    delete [] this->DICOMDataDictFile;
    this->DICOMDataDictFile=NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMrmlSceneOptionsNode::Write(ofstream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  vtkIndent i1(nIndent);

  of << i1 << "<SceneOptions";
  
  //Strings
  if (this->ViewUp && strcmp(this->ViewUp,""))
  {
    of << " viewUp='" << this->ViewUp << "'";
  }
  if (this->Position && strcmp(this->Position,""))
  {
    of << " position='" << this->Position << "'";
  }
  if (this->FocalPoint && strcmp(this->FocalPoint,""))
  {
    of << " focalPoint='" << this->FocalPoint << "'";
  }
  if (this->ClippingRange && strcmp(this->ClippingRange,""))
  {
    of << " clippingRange='" << this->ClippingRange << "'";
  }
  if (this->ViewMode && strcmp(this->ViewMode,""))
  {
    of << " viewMode='" << this->ViewMode << "'";
  }
  if (this->ViewBgColor && strcmp(this->ViewBgColor,""))
  {
    of << " viewBgColor='" << this->ViewBgColor << "'";
  }
  if (this->DICOMStartDir && strcmp(this->DICOMStartDir,""))
  {
    of << " DICOMStartDir='" << this->DICOMStartDir << "'";
  }
  if (this->FileNameSortParam && strcmp(this->FileNameSortParam,""))
  {
    of << " FileNameSortParam='" << this->FileNameSortParam << "'";
  }
  if (this->DICOMDataDictFile && strcmp(this->DICOMDataDictFile,""))
  {
    of << " DICOMDataDictFile='" << this->DICOMDataDictFile << "'";
  }
  
  // Numbers
  if (this->ShowAxes != 0)
  {
    of << " showAxes='" << (this->ShowAxes ? "true":"false") << "'";
  }
  if (this->ShowBox != 1)
  {
    of << " showBox='" << (this->ShowBox ? "true":"false") << "'";
  }
  if (this->ShowAnnotations != 1)
  {
    of << " showAnnotations='" << (this->ShowAnnotations ? "true":"false") << "'";
  }
  if (this->ShowSliceBounds != 0)
  {
    of << " showSliceBounds='" << (this->ShowSliceBounds ? "true":"false") << "'";
  }
  if (this->ShowLetters != 1)
  {
    of << " showLetters='" << (this->ShowLetters ? "true":"false") << "'";
  }
  if (this->ShowCross != 1)
  {
    of << " showCross='" << (this->ShowCross ? "true":"false") << "'";
  }
  if (this->ShowHashes != 1)
  {
    of << " showHashes='" << (this->ShowHashes ? "true":"false") << "'";
  }
  if (this->ShowMouse != 1)
  {
    of << " showMouse='" << (this->ShowMouse ? "true":"false") << "'";
  }
  if (this->DICOMPreviewWidth != 64)
  {
    of << " DICOMPreviewWidth='" << this->DICOMPreviewWidth << "'";
  }
  if (this->DICOMPreviewHeight != 64)
  {
    of << " DICOMPreviewHeight='" << this->DICOMPreviewHeight << "'";
  }
  if (this->DICOMPreviewHighestValue != 2048)
  {
    of << " DICOMPreviewHighestValue='" << this->DICOMPreviewHighestValue << "'";
  }
  
  of << "></SceneOptions>\n";
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
void vtkMrmlSceneOptionsNode::Copy(vtkMrmlNode *anode)
{
  vtkMrmlNode::MrmlNodeCopy(anode);
  vtkMrmlSceneOptionsNode *node = (vtkMrmlSceneOptionsNode *) anode;

  // Strings
  this->SetViewUp(node->ViewUp);
  this->SetPosition(node->Position);
  this->SetFocalPoint(node->FocalPoint);
  this->SetClippingRange(node->ClippingRange);
  this->SetViewMode(node->ViewMode);
  this->SetViewBgColor(node->ViewBgColor);
  this->SetDICOMStartDir(node->DICOMStartDir);
  this->SetFileNameSortParam(node->FileNameSortParam);
  this->SetDICOMDataDictFile(node->DICOMDataDictFile);
  
  // Numbers
  this->SetShowAxes(node->ShowAxes);
  this->SetShowBox(node->ShowBox);
  this->SetShowAnnotations(node->ShowAnnotations);
  this->SetShowSliceBounds(node->ShowSliceBounds);
  this->SetShowLetters(node->ShowLetters);
  this->SetShowCross(node->ShowCross);
  this->SetShowHashes(node->ShowHashes);
  this->SetShowMouse(node->ShowMouse);
  this->SetDICOMPreviewWidth(node->DICOMPreviewWidth);
  this->SetDICOMPreviewHeight(node->DICOMPreviewHeight);
  this->SetDICOMPreviewHighestValue(node->DICOMPreviewHighestValue);
}

//----------------------------------------------------------------------------
void vtkMrmlSceneOptionsNode::PrintSelf(ostream& os, vtkIndent indent)
{
  
  vtkMrmlNode::PrintSelf(os,indent);

  os << indent << "ViewUp: " << (this->ViewUp ? this->ViewUp : "(none)") << "\n";
  os << indent << "Position: " << (this->Position ? this->Position : "(none)") << "\n";
  os << indent << "FocalPoint: " << (this->FocalPoint ? this->FocalPoint : "(none)") << "\n";
  os << indent << "ClippingRange: " << (this->ClippingRange ? this->ClippingRange : "(none)") << "\n";
  os << indent << "ViewMode: " << (this->ViewMode ? this->ViewMode : "(none)") << "\n";
  os << indent << "ViewBgColor: " << (this->ViewBgColor ? this->ViewBgColor : "(none)") << "\n";
  os << indent << "DICOMStartDir: " << (this->DICOMStartDir ? this->DICOMStartDir : "(none)") << "\n";
  os << indent << "FileNameSortParam: " << (this->FileNameSortParam ? this->FileNameSortParam : "(none)") << "\n";
  os << indent << "DICOMDataDictFile: " << (this->DICOMDataDictFile ? this->DICOMDataDictFile : "(none)") << "\n";
  os << indent << "ShowAxes: " << this->ShowAxes << "\n";
  os << indent << "ShowBox: " << this->ShowBox << "\n";
  os << indent << "ShowAnnotations: " << this->ShowAnnotations << "\n";
  os << indent << "ShowSliceBounds: " << this->ShowSliceBounds << "\n";
  os << indent << "ShowLetters: " << this->ShowLetters << "\n";
  os << indent << "ShowCross: " << this->ShowCross << "\n";
  os << indent << "ShowHashes: " << this->ShowHashes << "\n";
  os << indent << "ShowMouse: " << this->ShowMouse << "\n";
  os << indent << "DICOMPreviewWidth: " << this->DICOMPreviewWidth << "\n";
  os << indent << "DICOMPreviewHeight: " << this->DICOMPreviewHeight << "\n";
  os << indent << "DICOMPreviewHighestValue: " << this->DICOMPreviewHighestValue << "\n";
}

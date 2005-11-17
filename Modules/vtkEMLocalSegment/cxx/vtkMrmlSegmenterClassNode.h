/*=auto=========================================================================

(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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
// .NAME vtkMrmlSegmenterClassNode - MRML node to represent transformation matrices.
// .SECTION Description
// The output of a rigid-body registration is a rotation and translation 
// expressed mathematically as a transformation matrix.  These transforms 
// can be inserted into MRML files as Segmenter nodes.  Each matrix 
// affects volumes and models that appear below it in the MRML file.  
// Multiple matrices can be concatenated together. 

#ifndef __vtkMrmlSegmenterClassNode_h
#define __vtkMrmlSegmenterClassNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkMrmlSegmenterGenericClassNode.h"
#include "vtkSlicer.h"
#include <vtkEMLocalSegmentConfigure.h>
#include "vtkMrmlSegmenterAtlasClassNode.h"

class VTK_EMLOCALSEGMENT_EXPORT vtkMrmlSegmenterClassNode : public vtkMrmlSegmenterGenericClassNode
{
public:
  static vtkMrmlSegmenterClassNode *New();
  vtkTypeMacro(vtkMrmlSegmenterClassNode,vtkMrmlNode);
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
  // Get/Set for SegmenterClass
  vtkSetStringMacro(PCAMeanName);
  vtkGetStringMacro(PCAMeanName);

  // Description:
  // Variance to maximum distance in the signed label map  
  vtkGetMacro(PCALogisticSlope,float);
  vtkSetMacro(PCALogisticSlope,float);

  vtkGetMacro(PCALogisticMin,float);
  vtkSetMacro(PCALogisticMin,float);

  vtkGetMacro(PCALogisticMax,float);
  vtkSetMacro(PCALogisticMax,float);

  vtkGetMacro(PCALogisticBoundary,float);
  vtkSetMacro(PCALogisticBoundary,float);
  
  vtkSetMacro(PrintPCA,int);
  vtkGetMacro(PrintPCA,int);

  int GetLabel() {return AtlasClassNode->GetLabel();}
  void SetLabel(int init) {AtlasClassNode->SetLabel(init);}

  char* GetLogMean()  {return AtlasClassNode->GetLogMean();}
  void SetLogMean(char* init) {AtlasClassNode->SetLogMean(init);}

  char* GetLogCovariance()  {return AtlasClassNode->GetLogCovariance();}
  void SetLogCovariance(char* init) {AtlasClassNode->SetLogCovariance(init);}

  char* GetReferenceStandardFileName()  {return AtlasClassNode->GetReferenceStandardFileName();}
  void SetReferenceStandardFileName(char* init) {AtlasClassNode->SetReferenceStandardFileName(init);}

  int GetPrintQuality()  {return AtlasClassNode->GetPrintQuality();}
  void SetPrintQuality(int init) {AtlasClassNode->SetPrintQuality(init);}

protected:
  vtkMrmlSegmenterClassNode();
  ~vtkMrmlSegmenterClassNode();
  vtkMrmlSegmenterClassNode(const vtkMrmlSegmenterClassNode&) {};
  void operator=(const vtkMrmlSegmenterClassNode&) {};

  vtkMrmlSegmenterAtlasClassNode *AtlasClassNode;

  char  *PCAMeanName;
  float PCALogisticSlope;
  float PCALogisticMin;
  float PCALogisticMax;
  float PCALogisticBoundary;
  int   PrintPCA;            // Print out PCA Parameters at each step 
};

#endif


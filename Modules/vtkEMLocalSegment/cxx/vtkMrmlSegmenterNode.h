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
// .NAME vtkMrmlSegmenterNode - MRML node to represent transformation matrices.
// .SECTION Description
// The output of a rigid-body registration is a rotation and translation 
// expressed mathematically as a transformation matrix.  These transforms 
// can be inserted into MRML files as Segmenter nodes.  Each matrix 
// affects volumes and models that appear below it in the MRML file.  
// Multiple matrices can be concatenated together. 

#ifndef __vtkMrmlSegmenterNode_h
#define __vtkMrmlSegmenterNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkMrmlNode.h"
#include "vtkSlicer.h"
#include <vtkEMLocalSegmentConfigure.h>

class VTK_EMLOCALSEGMENT_EXPORT vtkMrmlSegmenterNode : public vtkMrmlNode
{
public:
  static vtkMrmlSegmenterNode *New();
  vtkTypeMacro(vtkMrmlSegmenterNode,vtkMrmlNode);
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
  // Get/Set for Segmenter
  vtkSetMacro(AlreadyRead, int);
  vtkGetMacro(AlreadyRead, int);

  // Description:
  // Get/Set for Segmenter
  vtkSetMacro(MaxInputChannelDef, int);
  vtkGetMacro(MaxInputChannelDef, int);

  // Description:
  // Get/Set for Segmenter
  vtkSetMacro(NumClasses, int);
  vtkGetMacro(NumClasses, int);

  // Description:
  // Get/Set for Segmenter
  vtkSetMacro(EMShapeIter, int);
  vtkGetMacro(EMShapeIter, int);

  // Description:
  // Get/Set for Segmenter
  vtkSetMacro(EMiteration, int);
  vtkGetMacro(EMiteration, int);

  // Description:
  // Get/Set for Segmenter
  vtkSetMacro(MFAiteration, int);
  vtkGetMacro(MFAiteration, int);

  // Description:
  // Get/Set for Segmenter
  vtkSetMacro(Alpha, double);
  vtkGetMacro(Alpha, double);

  // Description:
  // Get/Set for Segmenter
  vtkSetMacro(SmWidth, int);
  vtkGetMacro(SmWidth, int);

  // Description:
  // Get/Set for Segmenter
  vtkSetMacro(SmSigma, int);
  vtkGetMacro(SmSigma, int);

  // Description:
  // Get/Set for Segmenter - not part of new version anymore 
  vtkSetMacro(StartSlice, int);
  vtkGetMacro(StartSlice, int);

  // Description:
  // Get/Set for Segmenter - not part of new version anymore 
  vtkSetMacro(EndSlice, int);
  vtkGetMacro(EndSlice, int);

  // Description:
  // Replacement for Start - EndSlice Bounding Box can be 3D
  void vtkSetSegmentationBoundaryMin(int init0, int init1, int init2) {
    int init[3]={init0,init1,init2};
    this->SetSegmentationBoundaryMin(init);
  }

  void SetSegmentationBoundaryMin(int init[3]) {
    for(int i = 0 ; i < 3; i++) {printf("%d ",init[i]); this->SegmentationBoundaryMin[i] = init[i];}
    this->StartSlice = this->SegmentationBoundaryMin[2];
  }

  void vtkSetSegmentationBoundaryMax(int init0, int init1, int init2) {
    int init[3]={init0,init1,init2};
    this->SetSegmentationBoundaryMax(init);
  }

  void SetSegmentationBoundaryMax(int init[3]) {
    for(int i = 0 ; i < 3; i++) this->SegmentationBoundaryMax[i] = init[i];
    this->EndSlice = this->SegmentationBoundaryMax[2];
  }

  vtkGetVector3Macro(SegmentationBoundaryMin,int);
  vtkGetVector3Macro(SegmentationBoundaryMax,int);

 
  // Description:
  // Get/Set for Segmenter
  vtkSetMacro(DisplayProb, int);
  vtkGetMacro(DisplayProb, int);

  // Description:
  // Get/Set for Segmenter
  vtkSetMacro(NumberOfTrainingSamples, int);
  vtkGetMacro(NumberOfTrainingSamples, int);

  // Description:
  // Get/Set for Segmenter
  vtkSetMacro(IntensityAvgClass, int);
  vtkGetMacro(IntensityAvgClass, int);

  // Description:
  // The work directory for this segmentation 
  // Necessarry for EM to spid out intermediate results 
  // it will generate the necessary subdirectories from here 
  // e.g. weights 
  vtkGetStringMacro(PrintDir);
  vtkSetStringMacro(PrintDir);

protected:
  vtkMrmlSegmenterNode();
  ~vtkMrmlSegmenterNode();
  vtkMrmlSegmenterNode(const vtkMrmlSegmenterNode&) {};
  void operator=(const vtkMrmlSegmenterNode&) {};

  int    AlreadyRead; 
  int    MaxInputChannelDef;
  int    NumClasses;
  int    EMShapeIter;
  int    EMiteration;
  int    MFAiteration;
  double Alpha;
  int    SmWidth;
  int    SmSigma;
  int    StartSlice;
  int    EndSlice;
  int    DisplayProb;
  int    NumberOfTrainingSamples;
  int    IntensityAvgClass;
  char*  PrintDir;
  int    SegmentationBoundaryMin[3];
  int    SegmentationBoundaryMax[3];
};

#endif


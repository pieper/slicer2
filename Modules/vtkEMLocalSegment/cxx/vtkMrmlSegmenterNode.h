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
  vtkSetMacro(EMShapeIter, int);
  vtkGetMacro(EMShapeIter, int);

  // Description:
  // Get/Set for Segmenter
  void SetEMiteration(int init) {
    // the public version still works that way  - later do it 
    //vtkWarningMacro(<<"You have an older XML Version for EMSegmenter - EMiteration is not defined anymore as part of vtMRMLSegmenterNode"<< endl 
    //                <<"We still read in values but update your XML File to new structure to erase this error message" );
    this->EMiteration  = init;
  }

  vtkGetMacro(EMiteration, int);

  // Description:
  // Get/Set for Segmenter
  void SetMFAiteration(int init) {
    // the public version still works that way  -later uncomment it 
    //vtkWarningMacro(<<"You have an older XML Version for EMSegmenter - MFAiteration is not defined anymore as part of vtMRMLSegmenterNode"<< endl 
    //                <<"We still read in values but update your XML File to new structure to erase this error message" );
    this->MFAiteration  = init;
  }
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
  // Replacement for Start - EndSlice Bounding Box can be 3D
  vtkSetVector3Macro(SegmentationBoundaryMin, int);
  vtkGetVector3Macro(SegmentationBoundaryMin,int);

  vtkSetVector3Macro(SegmentationBoundaryMax,int);
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


  // Description:
  // Define what kind of interpolation you want for the registration function - 
  // 1 = Linear Affine Registration 
  // 2 = Nearest Neighbour Affine Registration
   vtkSetMacro(RegistrationInterpolationType, int);
   vtkGetMacro(RegistrationInterpolationType, int);


  // Legacy Variables : 
  // The tree is a 1D list of nodes, it does not know anything about hireachies 
  //  => Never delete variables from vtkMrml..Node.h if for some XML files you use them 
  // and you cannot update them easily in LoadMRML

  // Description:
  // Get/Set for Segmenter
  void SetNumClasses(int init) {
    vtkWarningMacro(<<"You have an older XML Version for EMSegmenter - NumClasses is not defined anymore as part of vtMRMLSegmenterNode"<< endl 
                    <<"We still read in values but update your XML File to new structure to erase this error message" );
    this->NumClasses  = init;
  }
  vtkGetMacro(NumClasses, int);

protected:
  vtkMrmlSegmenterNode();
  ~vtkMrmlSegmenterNode();
  vtkMrmlSegmenterNode(const vtkMrmlSegmenterNode&) {};
  void operator=(const vtkMrmlSegmenterNode&) {};

  int    AlreadyRead; 
  int    MaxInputChannelDef;
  int    EMShapeIter;
  int    EMiteration;
  int    MFAiteration;
  double Alpha;
  int    SmWidth;
  int    SmSigma;
  int    DisplayProb;  // Should the probability displayed in the graph - left it in bc it is more work to take it out - should not be defined here but in GraphNode 
  int    NumberOfTrainingSamples;
  int    IntensityAvgClass;
  char*  PrintDir;
  int    SegmentationBoundaryMin[3];
  int    SegmentationBoundaryMax[3];

  int    RegistrationInterpolationType;
  // These are legacy definitions - we leave them in so we keep compatibility with older versions
  int    NumClasses; //  From July 04 the HeadClass will be defined seperatly from SegmenterNode so that there is no overlap anymore between SuperClassNode and SegmenterNode

};

#endif


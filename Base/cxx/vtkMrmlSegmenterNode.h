/*=auto=========================================================================

(c) Copyright 2001 Massachusetts Institute of Technology

Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for any purpose, 
provided that the above copyright notice and the following three paragraphs 
appear on all copies of this software.  Use of this software constitutes 
acceptance of these terms and conditions.

IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

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

class VTK_EXPORT vtkMrmlSegmenterNode : public vtkMrmlNode
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
  // Get/Set for Segmenter
  vtkSetMacro(PrintIntermediateResults, int);
  vtkGetMacro(PrintIntermediateResults, int);

  // Description:
  // Get/Set for Segmenter
  vtkSetMacro(PrintIntermediateSlice, int);
  vtkGetMacro(PrintIntermediateSlice, int);

  // Description:
  // Get/Set for Segmenter
  vtkSetMacro(PrintIntermediateFrequency, int);
  vtkGetMacro(PrintIntermediateFrequency, int);

  // Description:
  // Get/Set for Segmenter
  vtkSetMacro(StartSlice, int);
  vtkGetMacro(StartSlice, int);

  // Description:
  // Get/Set for Segmenter
  vtkSetMacro(EndSlice, int);
  vtkGetMacro(EndSlice, int);

  // Description:
  // Get/Set for Segmenter
  vtkSetMacro(DisplayProb, int);
  vtkGetMacro(DisplayProb, int);

  // Description:
  // Get/Set for Segmenter
  vtkSetMacro(NumberOfTrainingSamples, int);
  vtkGetMacro(NumberOfTrainingSamples, int);

protected:
  vtkMrmlSegmenterNode();
  ~vtkMrmlSegmenterNode();
  vtkMrmlSegmenterNode(const vtkMrmlSegmenterNode&) {};
  void operator=(const vtkMrmlSegmenterNode&) {};

  int    AlreadyRead; 
  int    MaxInputChannelDef;
  int    NumClasses;
  int    EMiteration;
  int    MFAiteration;
  double Alpha;
  int    SmWidth;
  int    SmSigma;
  int    PrintIntermediateResults;
  int    PrintIntermediateSlice;
  int    PrintIntermediateFrequency;
  int    StartSlice;
  int    EndSlice;
  int    DisplayProb;
  int    NumberOfTrainingSamples;
};

#endif


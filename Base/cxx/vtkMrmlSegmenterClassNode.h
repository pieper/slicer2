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
#include "vtkMrmlNode.h"

class VTK_EXPORT vtkMrmlSegmenterClassNode : public vtkMrmlNode
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
  vtkGetMacro(Label, int);
  vtkSetMacro(Label, int);

  // Description:
  // Get/Set for SegmenterClass
  vtkGetMacro(Prob, double);
  vtkSetMacro(Prob, double);

  // Description:
  // Get/Set for SegmenterClass
  vtkSetStringMacro(LocalPriorPrefix);
  vtkGetStringMacro(LocalPriorPrefix);

  // Description:
  // Get/Set for SegmenterClass
  vtkSetStringMacro(LocalPriorName);
  vtkGetStringMacro(LocalPriorName);

  // Description:
  // Get/Set for SegmenterClass
  vtkGetVector2Macro(LocalPriorRange, int);
  vtkSetVector2Macro(LocalPriorRange, int);

  // Description:
  // Get/Set for SegmenterClass
  vtkSetStringMacro(LogMean);
  vtkGetStringMacro(LogMean);

  // Description:
  // Get/Set for SegmenterClass
  vtkSetStringMacro(LogCovariance);
  vtkGetStringMacro(LogCovariance);

protected:
  vtkMrmlSegmenterClassNode();
  ~vtkMrmlSegmenterClassNode();
  vtkMrmlSegmenterClassNode(const vtkMrmlSegmenterClassNode&) {};
  void operator=(const vtkMrmlSegmenterClassNode&) {};

  // I do not know how to better Identify my Images
  int    Label;
  double Prob;
  char *LocalPriorPrefix; 
  char *LocalPriorName;
  int  LocalPriorRange[2];
  char *LogMean;
  char *LogCovariance;
};

#endif


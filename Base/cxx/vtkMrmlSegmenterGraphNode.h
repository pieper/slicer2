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
// .NAME vtkMrmlSegmenterGraphNode - MRML node to represent transformation matrices.
// .SECTION Description
// The output of a rigid-body registration is a rotation and translation 
// expressed mathematically as a transformation matrix.  These transforms 
// can be inserted into MRML files as Segmenter nodes.  Each matrix 
// affects volumes and models that appear below it in the MRML file.  
// Multiple matrices can be concatenated together. 

#ifndef __vtkMrmlSegmenterGraphNode_h
#define __vtkMrmlSegmenterGraphNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkMrmlNode.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlSegmenterGraphNode : public vtkMrmlNode
{
public:
  static vtkMrmlSegmenterGraphNode *New();
  vtkTypeMacro(vtkMrmlSegmenterGraphNode,vtkMrmlNode);
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
  // Get/Set for SegmenterGraph
  vtkSetMacro(Xmin, int);
  vtkGetMacro(Xmin, int);

  // Description:
  // Get/Set for SegmenterGraph
  vtkSetMacro(Xmax, int);
  vtkGetMacro(Xmax, int);

  // Description:
  // Get/Set for SegmenterGraph
  vtkSetMacro(Xsca, int);
  vtkGetMacro(Xsca, int);

protected:
  vtkMrmlSegmenterGraphNode();
  ~vtkMrmlSegmenterGraphNode();
  vtkMrmlSegmenterGraphNode(const vtkMrmlSegmenterGraphNode&) {};
  void operator=(const vtkMrmlSegmenterGraphNode&) {};

  int Xmin;
  int Xmax;
  int Xsca;
};

#endif


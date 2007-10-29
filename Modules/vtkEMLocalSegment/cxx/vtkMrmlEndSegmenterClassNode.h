/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlEndSegmenterClassNode.h,v $
  Date:      $Date: 2007/10/29 15:39:23 $
  Version:   $Revision: 1.2.8.1.2.1 $

=========================================================================auto=*/
// .NAME vtkMrmlEndSegmenterClassNode - represents the end of a vtkMrmlPathNode.
// .SECTION Description
// Just a place holder in a vtkMrmlTree

#ifndef __vtkMrmlEndSegmenterClassNode_h
#define __vtkMrmlEndSegmenterClassNode_h

#include "vtkMrmlNode.h"
#include "vtkSlicer.h"
#include <vtkEMLocalSegmentConfigure.h>

class VTK_EMLOCALSEGMENT_EXPORT vtkMrmlEndSegmenterClassNode : public vtkMrmlNode
{
public:
  static vtkMrmlEndSegmenterClassNode *New();
  vtkTypeMacro(vtkMrmlEndSegmenterClassNode,vtkMrmlNode);
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

protected:
  vtkMrmlEndSegmenterClassNode();
  ~vtkMrmlEndSegmenterClassNode();

private:
  vtkMrmlEndSegmenterClassNode(const vtkMrmlEndSegmenterClassNode&);
  void operator=(const vtkMrmlEndSegmenterClassNode&);
};

#endif


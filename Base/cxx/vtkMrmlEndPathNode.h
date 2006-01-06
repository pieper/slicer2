/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMrmlEndPathNode.h,v $
  Date:      $Date: 2006/01/06 17:56:46 $
  Version:   $Revision: 1.11 $

=========================================================================auto=*/
// .NAME vtkMrmlEndPathNode - represents the end of a vtkMrmlPathNode.
// .SECTION Description
// Just a place holder in a vtkMrmlTree

#ifndef __vtkMrmlEndPathNode_h
#define __vtkMrmlEndPathNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkMrmlNode.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlEndPathNode : public vtkMrmlNode
{
public:
  static vtkMrmlEndPathNode *New();
  vtkTypeMacro(vtkMrmlEndPathNode,vtkMrmlNode);
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
  vtkMrmlEndPathNode();
  ~vtkMrmlEndPathNode();
  vtkMrmlEndPathNode(const vtkMrmlEndPathNode&) {};
  void operator=(const vtkMrmlEndPathNode&) {};

};

#endif


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
// .NAME vtkMrmlFiducialsNode - MRML node for representing a
// constellation of vtkMrmlPoints.
// .SECTION Description
// A Fiducials is container for a constellation of vtkMrmlPoints

#ifndef __vtkMrmlFiducialsNode_h
#define __vtkMrmlFiducialsNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkMrmlNode.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"

class VTK_EXPORT vtkMrmlFiducialsNode : public vtkMrmlNode
{
public:
  static vtkMrmlFiducialsNode *New();
  vtkTypeMacro(vtkMrmlFiducialsNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);

  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlFiducialsNode *node);

protected:
  vtkMrmlFiducialsNode();
  ~vtkMrmlFiducialsNode();
  vtkMrmlFiducialsNode(const vtkMrmlFiducialsNode&) {};
  void operator=(const vtkMrmlFiducialsNode&) {};

};

#endif


/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for internal 
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
// .NAME vtkMrmlVolumeReadWriteStructuredPointsNode - 
// .SECTION Description
// This sub-node should contain information specific to each
// type of volume that needs to be read in.  This can be used
// to clean up the special cases in this file which handle
// volumes of various types, such as dicom, header, etc.  In
// future these things can be moved to the sub-node specific for that
// type of volume.  The sub-nodes here that describe specific volume
// types each correspond to an implementation of the reader/writer,
// which can be found in a vtkMrmlDataVolumeReadWrite subclass.

#ifndef __vtkMrmlVolumeReadWriteStructuredPointsNode_h
#define __vtkMrmlVolumeReadWriteStructuredPointsNode_h

#include "vtkMrmlVolumeReadWriteNode.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlVolumeReadWriteStructuredPointsNode : public vtkMrmlVolumeReadWriteNode
{
  public:
  static vtkMrmlVolumeReadWriteStructuredPointsNode *New();
  vtkTypeMacro(vtkMrmlVolumeReadWriteStructuredPointsNode,vtkMrmlVolumeReadWriteNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------
 
  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlNode *node);

  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);
  
  //--------------------------------------------------------------------------
  // Specifics for each type of volume data
  //--------------------------------------------------------------------------

  // Subclasses will add more here to handle their types of volume

protected:
  vtkMrmlVolumeReadWriteStructuredPointsNode();
  ~vtkMrmlVolumeReadWriteStructuredPointsNode();
  vtkMrmlVolumeReadWriteStructuredPointsNode(const vtkMrmlVolumeReadWriteStructuredPointsNode&) {};
  void operator=(const vtkMrmlVolumeReadWriteStructuredPointsNode&) {};

};

#endif

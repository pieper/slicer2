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
// .NAME vtkMrmlOptionsNode - MRML node for storing browser-specific data.
// .SECTION Description
// Option nodes allow browser-specific information to be stored in
// a MRML file.  For example, the 3D Slicer uses Option nodes to store
// the user's 3D viewpoint information since there currently is no
// View node in MRML2.0.

#ifndef __vtkMrmlOptionsNode_h
#define __vtkMrmlOptionsNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkMrmlNode.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlOptionsNode : public vtkMrmlNode
{
  public:
  static vtkMrmlOptionsNode *New();
  vtkTypeMacro(vtkMrmlOptionsNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Write the node's attributes
  void Write(ofstream& of, int indent);

  // Description:
  // Copy the nodes attributes to this object
  void Copy(vtkMrmlNode *node);

  //--------------------------------------------------------------------------
  // Get and Set Functions
  //--------------------------------------------------------------------------

  // Description:
  // Name of the program that should recognize these options.
  vtkGetStringMacro(Program);
  vtkSetStringMacro(Program);

  // Description:
  // Short description of the contents of this node. ie: 'presets'
  vtkGetStringMacro(Contents);
  vtkSetStringMacro(Contents);

  // Description:
  // List of options expressed in this form: key1='value1' key2='value2'
  vtkGetStringMacro(Options);
  vtkSetStringMacro(Options);

  protected:
  vtkMrmlOptionsNode();
  ~vtkMrmlOptionsNode();
  vtkMrmlOptionsNode(const vtkMrmlOptionsNode&) {};
  void operator=(const vtkMrmlOptionsNode&) {};

  // Description:
  // Contains information for use by specified program
  // Not needed here since superclass vtkMrmlNode has Options
  //char *Options;

  // Description:
  // Name of program that uses these options (i.e. Slicer)
  char *Program;
  // Description:
  // Type of Options stored in this node (i.e. user preferences)
  char *Contents;
};

#endif










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
// .NAME vtkMrmlWindowLevelNode - MRML node to represent the properties of 
// the window levels
// .SECTION Description

#ifndef __vtkMrmlWindowLevelNode_h
#define __vtkMrmlWindowLevelNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkMrmlNode.h"


class VTK_EXPORT vtkMrmlWindowLevelNode : public vtkMrmlNode
{
public:
  static vtkMrmlWindowLevelNode *New();
  vtkTypeMacro(vtkMrmlWindowLevelNode,vtkMrmlNode);
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

  vtkSetMacro(Window,int);
  vtkGetMacro(Window,int);
  
  vtkSetMacro(Level,int);
  vtkGetMacro(Level,int);

  // Description:
  // Lower threshold of windowing function
  vtkSetMacro(LowerThreshold,int);
  vtkGetMacro(LowerThreshold,int);

  // Description:
  // Upper threshold of windowing function
  vtkSetMacro(UpperThreshold,int);
  vtkGetMacro(UpperThreshold,int);
  
  // Description:
  // Does Slicer determine the window level automatically?
  vtkBooleanMacro(AutoWindowLevel,int);
  vtkSetMacro(AutoWindowLevel,int);
  vtkGetMacro(AutoWindowLevel,int);
  
  // Description:
  // Apply threshold?
  vtkBooleanMacro(ApplyThreshold,int);
  vtkSetMacro(ApplyThreshold,int);
  vtkGetMacro(ApplyThreshold,int);
  
  // Description:
  // Does Slicer determine the threshold automatically?
  vtkBooleanMacro(AutoThreshold,int);
  vtkSetMacro(AutoThreshold,int);
  vtkGetMacro(AutoThreshold,int);

  
 
protected:
  vtkMrmlWindowLevelNode();
  ~vtkMrmlWindowLevelNode();
  vtkMrmlWindowLevelNode(const vtkMrmlWindowLevelNode&) {};
  void operator=(const vtkMrmlWindowLevelNode&) {};

  // Numbers
  int Window;
  int Level;
  int LowerThreshold;
  int UpperThreshold;
  
  // Booleans
  int AutoWindowLevel;
  int ApplyThreshold;
  int AutoThreshold;
  
};

#endif

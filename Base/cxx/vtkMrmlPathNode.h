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
// .NAME vtkMrmlPathNode - MRML node to represent a path.
// .SECTION Description
//

#ifndef __vtkMrmlPathNode_h
#define __vtkMrmlPathNode_h

#include <iostream.h>
#include <fstream.h>
#include "vtkMrmlNode.h"

class VTK_EXPORT vtkMrmlPathNode : public vtkMrmlNode
{
public:
  static vtkMrmlPathNode *New();
  vtkTypeMacro(vtkMrmlPathNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);

  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlPathNode *node);


  // Description:
  // Name of the Camera Path (cPath) color, which is defined by a Color node in a MRML file
 //  vtkSetStringMacro(cPathColor);
 // vtkGetStringMacro(cPathColor);

  // Description:
  // Name of the Focal Point Path (fPath) color, which is defined by a Color node in a MRML file
  // vtkSetStringMacro(fPathColor);
  // vtkGetStringMacro(fPathColor);

  // Description:
  // Name of the Camera Landmarks (cLand) color, which is defined by a Color node in a MRML file
  // vtkSetStringMacro(cLandColor);
  // vtkGetStringMacro(cLandColor);

  // Description:
  // Name of the Focal Point Landmarks (fPath) color, which is defined by a Color node in a MRML file
  // vtkSetStringMacro(fLandColor);
  // vtkGetStringMacro(fLandColor);

protected:
  vtkMrmlPathNode();
  ~vtkMrmlPathNode();
  vtkMrmlPathNode(const vtkMrmlPathNode&) {};
  void operator=(const vtkMrmlPathNode&) {};

//  char *cPathColor;
//  char *fPathColor;
//  char *cLandColor;
//  char *fLandColor;

};

#endif


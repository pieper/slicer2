/*=auto=========================================================================
Copyright (c) 2001 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy (but NOT distribute) this
software and its documentation for any NON-COMMERCIAL purpose, provided
that existing copyright notices are retained verbatim in all copies.
The authors grant permission to modify this software and its documentation 
for any NON-COMMERCIAL purpose, provided that such modifications are not 
distributed without the explicit consent of the authors and that existing
copyright notices are retained in all copies. Some of the algorithms
implemented by this software are patented, observe all applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
=========================================================================auto=*/
// .NAME vtkMrmlPathNode - MRML node to represent transformation matrices.
// .SECTION Description
// The output of a rigid-body registration is a rotation and translation 
// expressed mathematically as a transformation matrix.  These transforms 
// can be inserted into MRML files as Path nodes.  Each matrix 
// affects volumes and models that appear below it in the MRML file.  
// Multiple matrices can be concatenated together. 

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
  const char *GetName() {return "";}; 

  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);

  // Description:
  // Name displayed on the user interface
  //vtkSetStringMacro(Name);
  //vtkGetStringMacro(Name);

  // Description:
  // Description displayed on the user interface
  vtkSetStringMacro(Description);
  vtkGetStringMacro(Description);

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


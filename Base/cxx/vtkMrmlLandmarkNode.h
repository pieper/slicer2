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
// .NAME vtkMrmlLandmarkNode - MRML node to represent transformation matrices.
// .SECTION Description
// The output of a rigid-body registration is a rotation and translation 
// expressed mathematically as a transformation matrix.  These transforms 
// can be inserted into MRML files as Landmark nodes.  Each matrix 
// affects volumes and models that appear below it in the MRML file.  
// Multiple matrices can be concatenated together. 

#ifndef __vtkMrmlLandmarkNode_h
#define __vtkMrmlLandmarkNode_h

#include <iostream.h>
#include <fstream.h>
#include "vtkMrmlNode.h"

class VTK_EXPORT vtkMrmlLandmarkNode : public vtkMrmlNode
{
public:
  static vtkMrmlLandmarkNode *New();
  vtkTypeMacro(vtkMrmlLandmarkNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);

  // Description:
  // Name displayed on the user interface
  vtkSetStringMacro(Name);
  vtkGetStringMacro(Name);

  // Description:
  // Description displayed on the user interface
  vtkSetStringMacro(Description);
  vtkGetStringMacro(Description);

  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlLandmarkNode *node);

  // Description:
  // Get/Set for Landmark
  vtkSetVector3Macro(XYZ,float);
  vtkGetVectorMacro(XYZ,float,3);

  // Description:
  // Get/Set for Landmark
  vtkSetVector3Macro(FXYZ,float);
  vtkGetVectorMacro(FXYZ,float,3);

  // Description:
  // Position of the landmark along the path
  vtkSetMacro(PathPosition, float);
  vtkGetMacro(PathPosition, float);

protected:
  vtkMrmlLandmarkNode();
  ~vtkMrmlLandmarkNode();
  vtkMrmlLandmarkNode(const vtkMrmlLandmarkNode&) {};
  void operator=(const vtkMrmlLandmarkNode&) {};

  char *Name;
  char *Description;
  float XYZ[3];
  float FXYZ[3];
  int PathPosition;
};

#endif


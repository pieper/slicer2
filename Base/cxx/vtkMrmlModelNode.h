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
// .NAME vtkMrmlModelNode - MRML node to represent a 3D surface model.
// .SECTION Description
// Model nodes describe polygonal data.  They indicate where the model is 
// stored on disk, and how to render it (color, opacity, etc).  Models 
// are assumed to have been constructed with the orientation and voxel 
// dimensions of the original segmented volume.

#ifndef __vtkMrmlModelNode_h
#define __vtkMrmlModelNode_h

#include <iostream.h>
#include <fstream.h>
#include "vtkMrmlNode.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"

class VTK_EXPORT vtkMrmlModelNode : public vtkMrmlNode
{
public:
  static vtkMrmlModelNode *New();
  vtkTypeMacro(vtkMrmlModelNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);

  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlModelNode *node);

  // Description:
  // Name displayed on the user interface
  vtkSetStringMacro(Name);
  vtkGetStringMacro(Name);

  // Description:
  // Path of the data file, relative to the MRML file
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  // Description:
  // Absolute Path of the data file
  vtkSetStringMacro(FullFileName);
  vtkGetStringMacro(FullFileName);

  // Description:
  // Name of the model's color, which is defined by a Color node in a MRML file
  vtkSetStringMacro(Color);
  vtkGetStringMacro(Color);

  // Description:
  // Opacity of the surface expressed as a number from 0 to 1
  vtkSetMacro(Opacity, float);
  vtkGetMacro(Opacity, float);

  // Description:
  // Indicates if the surface is visible
  vtkBooleanMacro(Visibility, int);
  vtkGetMacro(Visibility, int);
  vtkSetMacro(Visibility, int);

  // Description:
  // Specifies whether to clip the surface with the slice planes
  vtkBooleanMacro(Clipping, int);
  vtkGetMacro(Clipping, int);
  vtkSetMacro(Clipping, int);

  // Description:
  // Indicates whether to cull (not render) the backface of the surface
  vtkBooleanMacro(BackfaceCulling, int);
  vtkGetMacro(BackfaceCulling, int);
  vtkSetMacro(BackfaceCulling, int);

  // Description:
  // Indicates whether to render the scalar value associated with each polygon vertex
  vtkBooleanMacro(ScalarVisibility, int);
  vtkGetMacro(ScalarVisibility, int);
  vtkSetMacro(ScalarVisibility, int);

  // Description:
  // Range of scalar values to render rather than the single color designated by colorName
  vtkSetVector2Macro(ScalarRange, int);
  vtkGetVector2Macro(ScalarRange, int);

  // Description:
  // Perform registration by setting the matrix that transforms this model
  // from its RAS (right-anterior-superior) space to the WLD (world) space
  // of the 3D scene it is a part of.
  void SetRasToWld(vtkMatrix4x4 *reg);
  vtkGetObjectMacro(RasToWld, vtkMatrix4x4);

protected:
  vtkMrmlModelNode();
  ~vtkMrmlModelNode();
  vtkMrmlModelNode(const vtkMrmlModelNode&) {};
  void operator=(const vtkMrmlModelNode&) {};

  // Strings
  char *Name;
  char *FileName;
  char *FullFileName;
  char *Color;

  // Numbers
  float Opacity;

  // Booleans
  int Visibility;
  int Clipping;
  int BackfaceCulling;
  int ScalarVisibility;

  // Arrays
  int ScalarRange[2];

  vtkMatrix4x4 *RasToWld;
};

#endif

/*=auto=========================================================================

(c) Copyright 2002 Massachusetts Institute of Technology

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
// .NAME vtkMrmlMatrixNode - MRML node to represent transformation matrices.
// .SECTION Description
// The output of a rigid-body registration is a rotation and translation 
// expressed mathematically as a transformation matrix.  These transforms 
// can be inserted into MRML files as Matrix nodes.  Each matrix 
// affects volumes and models that appear below it in the MRML file.  
// Multiple matrices can be concatenated together. 

#ifndef __vtkMrmlMatrixNode_h
#define __vtkMrmlMatrixNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkMrmlNode.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"

class VTK_EXPORT vtkMrmlMatrixNode : public vtkMrmlNode
{
public:
  static vtkMrmlMatrixNode *New();
  vtkTypeMacro(vtkMrmlMatrixNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);

  // Description:
  // 16 numbers that form a 4x4 matrix. The matrix is multiplied by a 
  // point (M*P) to compute the transformed point
  void SetMatrix(char *str) {
    this->SetMatrixToString(this->Transform->GetMatrixPointer(), str);};
  char *GetMatrix() { 
    return this->GetMatrixToString(this->Transform->GetMatrixPointer());};

  // Description:
  // Rotate around each axis: x,y, and z in degrees
  void Scale(float x, float y, float z) {
    this->Transform->Scale(x, y, z);};

  // Description:
  // Rotate around each axis: x,y, and z in degrees
  void RotateX(float d) {
    this->Transform->RotateX(d);};
  void RotateY(float d) {
    this->Transform->RotateY(d);};
  void RotateZ(float d) {
    this->Transform->RotateZ(d);};

  // Description:
  // Rotate around each axis: x,y, and z in degrees
  void Translate(float x, float y, float z) {
    this->Transform->Translate(x, y, z);};

  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlNode *node);

  // Description:
  // Get/Set for Matrix
  vtkGetObjectMacro(Transform, vtkTransform);
  vtkSetObjectMacro(Transform, vtkTransform);

protected:
  vtkMrmlMatrixNode();
  ~vtkMrmlMatrixNode();
  vtkMrmlMatrixNode(const vtkMrmlMatrixNode&) {};
  void operator=(const vtkMrmlMatrixNode&) {};

  vtkTransform *Transform;
};

#endif


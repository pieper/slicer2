/*=auto=========================================================================
Copyright (c) 2000 Surgical Planning Lab, Brigham and Women's Hospital
 
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
// .NAME vtkMrmlMatrixNode - MRML node to represent transformation matrices.
// .SECTION Description
// The output of a rigid-body registration is a rotation and translation 
// expressed mathematically as a transformation matrix.  These transforms 
// can be inserted into MRML files as Matrix nodes.  Each matrix 
// affects volumes and models that appear below it in the MRML file.  
// Multiple matrices can be concatenated together. 

#ifndef __vtkMrmlMatrixNode_h
#define __vtkMrmlMatrixNode_h

#include <iostream.h>
#include <fstream.h>
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
  // Name displayed on the user interface
  vtkSetStringMacro(Name);
  vtkGetStringMacro(Name);

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
  void Copy(vtkMrmlMatrixNode *node);

  // Description:
  // Get/Set for Matrix
  vtkGetObjectMacro(Transform, vtkTransform);
  vtkSetObjectMacro(Transform, vtkTransform);

protected:
  vtkMrmlMatrixNode();
  ~vtkMrmlMatrixNode();
  vtkMrmlMatrixNode(const vtkMrmlMatrixNode&) {};
  void operator=(const vtkMrmlMatrixNode&) {};

  char *Name;
  vtkTransform *Transform;
};

#endif


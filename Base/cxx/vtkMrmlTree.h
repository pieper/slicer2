/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
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
// .NAME vtkMrmlTree - a list of actors
// .SECTION Description
// vtkMrmlTree represents and provides methods to manipulate a list of
// MRML objects. The list is core and duplicate
// entries are not prevented.

// .SECTION see also
// vtkMrmlNode vtkCollection 

#ifndef __vtkMrmlTree_h
#define __vtkMrmlTree_h

#include "vtkCollection.h"
#include "vtkMrmlNode.h"
#include "vtkMrmlVolumeNode.h"
#include "vtkMrmlModelNode.h"
#include "vtkMrmlTransformNode.h"
#include "vtkMrmlMatrixNode.h"
#include "vtkMrmlColorNode.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkMrmlTree : public vtkCollection
{
public:
  static vtkMrmlTree *New();
  vtkTypeMacro(vtkMrmlTree,vtkCollection);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  void Write(char *filename);

  // Description:
  // Add a path to the list.
  void AddItem(vtkMrmlNode *n) {
    this->vtkCollection::AddItem((vtkObject *)n);};

  // Description:
  // Remove a path from the list.
  void RemoveItem(vtkMrmlNode *n) {
    this->vtkCollection::RemoveItem((vtkObject *)n);};

  // Description:
  // Determine whether a particular node is present. Returns its position
  // in the list.
  int IsItemPresent(vtkMrmlNode *n) {
    return this->vtkCollection::IsItemPresent((vtkObject *)n);};

  // Description:
  // Get the next path in the list.
  vtkMrmlNode *GetNextItem() {
    return (vtkMrmlNode *)(this->GetNextItemAsObject());};

  vtkMrmlVolumeNode *GetNextVolume() {
    return (vtkMrmlVolumeNode*)this->GetNextItemByClass("vtkMrmlVolumeNode");};
  vtkMrmlModelNode *GetNextModel() {
    return (vtkMrmlModelNode*)this->GetNextItemByClass("vtkMrmlModelNode");};
  vtkMrmlTransformNode *GetNextTransform() {
    return (vtkMrmlTransformNode*)this->GetNextItemByClass("vtkMrmlTransformNode");};
  vtkMrmlMatrixNode *GetNextMatrix() {
    return (vtkMrmlMatrixNode*)this->GetNextItemByClass("vtkMrmlMatrixNode");};
  vtkMrmlColorNode *GetNextColor() {
    return (vtkMrmlColorNode*)this->GetNextItemByClass("vtkMrmlColorNode");};

  vtkMrmlVolumeNode *InitVolumeTraversal() {
    return (vtkMrmlVolumeNode*)this->InitTraversalByClass("vtkMrmlVolumeNode");};
  vtkMrmlModelNode *InitModelTraversal() {
    return (vtkMrmlModelNode*)this->InitTraversalByClass("vtkMrmlModelNode");};
  vtkMrmlTransformNode *InitTransformTraversal() {
    return (vtkMrmlTransformNode*)this->InitTraversalByClass("vtkMrmlTransformNode");};
  vtkMrmlMatrixNode *InitMatrixTraversal() {
    return (vtkMrmlMatrixNode*)this->InitTraversalByClass("vtkMrmlMatrixNode");};
  vtkMrmlColorNode *InitColorTraversal() {
    return (vtkMrmlColorNode*)this->InitTraversalByClass("vtkMrmlColorNode");};

  vtkMrmlNode* GetNthItem(int n);
  vtkMrmlVolumeNode *GetNthVolume(int n) {
    return (vtkMrmlVolumeNode*)this->GetNthItemByClass(n, "vtkMrmlVolumeNode");};
  vtkMrmlModelNode *GetNthModel(int n) {
    return (vtkMrmlModelNode*)this->GetNthItemByClass(n, "vtkMrmlModelNode");};
  vtkMrmlTransformNode *GetNthTransform(int n) {
    return (vtkMrmlTransformNode*)this->GetNthItemByClass(n, "vtkMrmlTransformNode");};
  vtkMrmlMatrixNode *GetNthMatrix(int n) {
    return (vtkMrmlMatrixNode*)this->GetNthItemByClass(n, "vtkMrmlMatrixNode");};
  vtkMrmlColorNode *GetNthColor(int n) {
    return (vtkMrmlColorNode*)this->GetNthItemByClass(n, "vtkMrmlColorNode");};

  int GetNumberOfVolumes() {
    return this->GetNumberOfItemsByClass("vtkMrmlVolumeNode");};
  int GetNumberOfModels() {
    return this->GetNumberOfItemsByClass("vtkMrmlModelNode");};
  int GetNumberOfTransforms() {
    return this->GetNumberOfItemsByClass("vtkMrmlTransformNode");};
  int GetNumberOfMatrices() {
    return this->GetNumberOfItemsByClass("vtkMrmlMatrixNode");};
  int GetNumberOfColors() {
    return this->GetNumberOfItemsByClass("vtkMrmlColorNode");};

  void ComputeTransforms();

  void InsertAfterItem( vtkMrmlNode *item, vtkMrmlNode *newItem);
  void InsertBeforeItem( vtkMrmlNode *item, vtkMrmlNode *newItem);

  void ComputeNodeTransform( vtkMrmlNode *node, vtkMatrix4x4 *xform );

protected:
  vtkMrmlTree() {};
  ~vtkMrmlTree() {};
  vtkMrmlTree(const vtkMrmlTree&) {};
  void operator=(const vtkMrmlTree&) {};

  vtkMrmlNode *InitTraversalByClass(char *className);
  int GetNumberOfItemsByClass(char *className);
  vtkMrmlNode* GetNthItemByClass(int n, char *className);
  vtkMrmlNode *GetNextItemByClass(char *className);
  
private:
  // hide the standard AddItem from the user and the compiler.
  void AddItem(vtkObject *o) { this->vtkCollection::AddItem(o); };
  void RemoveItem(vtkObject *o) { this->vtkCollection::RemoveItem(o); };
  void RemoveItem(int i) { this->vtkCollection::RemoveItem(i); };
  int  IsItemPresent(vtkObject *o) { return this->vtkCollection::IsItemPresent(o);};
};

#endif

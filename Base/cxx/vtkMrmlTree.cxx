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
#include "vtkMrmlTree.h"
#include "vtkObjectFactory.h"
#include "vtkErrorCode.h"


//------------------------------------------------------------------------------
vtkMrmlTree* vtkMrmlTree::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlTree");
  if(ret)
  {
    return (vtkMrmlTree*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlTree;
}

//------------------------------------------------------------------------------
void vtkMrmlTree::Write(char *filename)
{
  vtkCollectionElement *elem=this->Top;
  vtkMrmlNode *node;
  ofstream file;
  int indent=0, deltaIndent;

  // Open file
  file.open(filename);
  if (file.fail())
  {
    vtkErrorMacro("Write: Could not open file " << filename);
    cerr << "Write: Could not open file " << filename;
#if (VTK_MAJOR_VERSION <= 5)      
    this->SetErrorCode(2);
#else
    this->SetErrorCode(vtkErrorCode::GetErrorCodeFromString("CannotOpenFileError"));
#endif
    return;
  }
  
  file << "<?xml version=\"1.0\" standalone='no'?>\n";
  file << "<!DOCTYPE MRML SYSTEM \"mrml20.dtd\">\n";
  file << "<MRML>\n";
   
  // Write each node
  while (elem != NULL)
  {
    node = (vtkMrmlNode*)elem->Item;

    deltaIndent = node->GetIndent();
    if ( deltaIndent < 0 )
    {
      indent -=2;
    }

    node->Write(file, indent);

    if ( deltaIndent > 0 )
    {
      indent += 2;
    }

    elem = elem->Next;
  }

  file << "</MRML>\n";

  // Close file
  file.close();
#if (VTK_MAJOR_VERSION <= 5)      
    this->SetErrorCode(0);
#else
    this->SetErrorCode(vtkErrorCode::GetErrorCodeFromString("NoError"));
#endif
}

//------------------------------------------------------------------------------
int vtkMrmlTree::GetNumberOfItemsByClass(char *className)
{
  vtkCollectionElement *elem=this->Top;
  int num=0;

  while (elem != NULL)
  {
    if (!strcmp(elem->Item->GetClassName(), className))
    {
      num++;
    }
    elem = elem->Next;
  }
  return num;
}

//------------------------------------------------------------------------------
vtkMrmlNode *vtkMrmlTree::GetNextItemByClass(char *className)
{
  vtkCollectionElement *elem=this->Current;

  if (elem != NULL)
  {
    elem = elem->Next;
  }

  while (elem != NULL && strcmp(elem->Item->GetClassName(), className))
  {
    elem = elem->Next;
  }
  
  if (elem != NULL)
  {
    this->Current = elem;
    return (vtkMrmlNode*)(elem->Item);
  }
  else
  {
    return NULL;
  }
}

//------------------------------------------------------------------------------
vtkMrmlNode* vtkMrmlTree::InitTraversalByClass(char *className)
{
  vtkCollectionElement *elem=this->Top;

  while (elem != NULL && strcmp(elem->Item->GetClassName(), className))
  {
    elem = elem->Next;
  }
  
  if (elem != NULL)
  {
    this->Current = elem;
    return (vtkMrmlNode*)(elem->Item);
  }
  else
  {
    return NULL;
  }
}

//------------------------------------------------------------------------------
vtkMrmlNode* vtkMrmlTree::GetNthItem(int n)
{
  vtkCollectionElement *elem;

  if(n < 0 || n >= this->NumberOfItems)
  {
    return NULL;
  }
  
  elem = this->Top;
  for (int j = 0; j < n; j++, elem = elem->Next) 
  {}

  return (vtkMrmlNode*)(elem->Item);
}

//------------------------------------------------------------------------------
vtkMrmlNode* vtkMrmlTree::GetNthItemByClass(int n, char *className)
{
  vtkCollectionElement *elem;
  int j=0;

  elem = this->Top;
  while (elem != NULL)
  {
    if (strcmp(elem->Item->GetClassName(), className) == 0)
    {
      if (j == n)
      {
        return (vtkMrmlNode*)(elem->Item);
      }
      j++;
    }
    elem = elem->Next;
  }

  return NULL;
}

//------------------------------------------------------------------------------
void vtkMrmlTree::ComputeTransforms()
{
  vtkMrmlNode *n;
  vtkTransform *tran = vtkTransform::New();
  vtkMatrix4x4 *mat = vtkMatrix4x4::New();
  vtkMrmlMatrixNode *t;
  vtkCollectionElement *elem;

    // Set the vtkTransform to PreMultiply so a concatenated matrix, C,
    // is applied before the existing matrix, M: M*C (not C*M)
  tran->PreMultiply();

  elem = this->Top;
  while (elem != NULL)
  {
    n = (vtkMrmlNode*)(elem->Item);
    
    // Transform
    if (!strcmp("vtkMrmlTransformNode", n->GetClassName()))
    {
      tran->Push();
    }

    // EndTransform
    else if (!strcmp("vtkMrmlEndTransformNode", n->GetClassName()))
    {
      tran->Pop();
    }

    // Matrix
    else if (!strcmp("vtkMrmlMatrixNode", n->GetClassName()))
    {
      t = (vtkMrmlMatrixNode*)n;
      tran->Concatenate(t->GetTransform()->GetMatrix());
    }

    // Volume
    else if (!strcmp("vtkMrmlVolumeNode", n->GetClassName()))
    {
      tran->GetMatrix(mat);
      ((vtkMrmlVolumeNode*)n)->SetRasToWld(mat);
    }

    // Model
    else if (!strcmp("vtkMrmlModelNode", n->GetClassName()))
    {
      tran->GetMatrix(mat);
      ((vtkMrmlModelNode*)n)->SetRasToWld(mat);
    }

    elem = elem->Next;
  }
  tran->Delete();
  mat->Delete();
}
//------------------------------------------------------------------------------
void vtkMrmlTree::ComputeNodeTransform( vtkMrmlNode *node, vtkTransform *tran )
{
  vtkMrmlNode *n;
  vtkMrmlMatrixNode *t;
  vtkCollectionElement *elem;

    // Set the vtkTransform to PreMultiply so a concatenated matrix, C,
    // is applied before the existing matrix, M: M*C (not C*M)
  tran->PreMultiply();

  elem = this->Top;
  while ((vtkMrmlNode*)(elem->Item) != node)
  {
    n = (vtkMrmlNode*)(elem->Item);
    
    // Transform
    if (!strcmp("vtkMrmlTransformNode", n->GetClassName()))
    {
      tran->Push();
    }

    // EndTransform
    else if (!strcmp("vtkMrmlEndTransformNode", n->GetClassName()))
    {
      tran->Pop();
    }

    // Matrix
    else if (!strcmp("vtkMrmlMatrixNode", n->GetClassName()))
    {
      t = (vtkMrmlMatrixNode*)n;
      // used to be Concatenate t->GetTransform()->GetMatrix()
      // changed by samson (Dec 2,2003) so that transforms could
      // be tracked
      tran->Concatenate(t->GetTransform());
    }
    elem = elem->Next;
  }
}

//------------------------------------------------------------------------------
void vtkMrmlTree::ComputeNodeTransform( vtkMrmlNode *node, vtkMatrix4x4 *xform )
{
  vtkTransform *tran = vtkTransform::New();
  this->ComputeNodeTransform(node,tran);
  vtkMatrix4x4 *mat = vtkMatrix4x4::New();
  tran->GetMatrix(mat);
  xform->DeepCopy( mat );
  tran->Delete();
  mat->Delete();
}

//------------------------------------------------------------------------------
void vtkMrmlTree::InsertAfterItem(vtkMrmlNode *item, vtkMrmlNode *n)
{
  int i;
  vtkCollectionElement *elem, *newElem;
  
  // Empty list
  if (!this->Top)
  {
    return;
  }

  newElem = new vtkCollectionElement;
  n->Register(this);
  newElem->Item = n;

  elem = this->Top;
  for (i = 0; i < this->NumberOfItems; i++)
  {
    if (elem->Item == item)
    {
      newElem->Next = elem->Next;
      elem->Next = newElem;

      if (this->Bottom == elem)
      {
        this->Bottom = newElem;
      }
      this->NumberOfItems++;
      return;
    }
    else
    {
      elem = elem->Next;
    }
  }
}

//------------------------------------------------------------------------------
void vtkMrmlTree::InsertBeforeItem(vtkMrmlNode *item, vtkMrmlNode *n)
{
  int i;
  vtkCollectionElement *elem, *newElem, *prev;
  
  // Empty list
  if (!this->Top)
  {
    return;
  }

  newElem = new vtkCollectionElement;
  n->Register(this);
  newElem->Item = n;


  elem = this->Top;

  // if insert before 1st elem
  if (elem->Item == item)
  {
    newElem->Next = this->Top;
    this->Top = newElem;
    this->NumberOfItems++;
    return;
  }

  for (i = 1; i < this->NumberOfItems; i++)
  {
    prev = elem;
    elem = elem->Next;

    if (elem->Item == item)
    {
      newElem->Next = prev->Next;
      prev->Next = newElem;
      this->NumberOfItems++;
      return;
    }
  }
}

void vtkMrmlTree::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkCollection::PrintSelf(os,indent);

  os << indent << "Number Of Volumes: " << this->GetNumberOfVolumes() << "\n";
  os << indent << "Number Of Models: " << this->GetNumberOfModels() << "\n";
  os << indent << "Number Of Transforms: " << this->GetNumberOfTransforms() << "\n";
  os << indent << "Number Of Matrices: " << this->GetNumberOfMatrices() << "\n";
  os << indent << "Number Of Colors: " << this->GetNumberOfColors() << "\n";
}

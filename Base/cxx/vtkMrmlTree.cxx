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
#include "vtkMrmlTree.h"
#include "vtkObjectFactory.h"

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

    // Set the vtkTransform to PostMultiply so a concatenated matrix, C,
    // is multiplied by the existing matrix, M: C*M (not M*C)
  tran->PostMultiply();

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
      tran->Concatenate(t->GetTransform()->GetMatrixPointer());
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
void vtkMrmlTree::ComputeNodeTransform( vtkMrmlNode *node, vtkMatrix4x4 *xform )
{
  vtkMrmlNode *n;
  vtkTransform *tran = vtkTransform::New();
  vtkMatrix4x4 *mat = vtkMatrix4x4::New();
  vtkMrmlMatrixNode *t;
  vtkCollectionElement *elem;

    // Set the vtkTransform to PostMultiply so a concatenated matrix, C,
    // is multiplied by the existing matrix, M: C*M (not M*C)
  tran->PostMultiply();

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
      tran->Concatenate(t->GetTransform()->GetMatrixPointer());
    }

    elem = elem->Next;
  }
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

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
  vtkMrmlTransformNode *t;
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
      t = (vtkMrmlTransformNode*)n;
      tran->Concatenate(t->GetTransform()->GetMatrixPointer());
    }

    // EndTransform
    else if (!strcmp("vtkMrmlEndTransformNode", n->GetClassName()))
    {
      tran->Pop();
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


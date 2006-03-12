/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLNode.cxx,v $
Date:      $Date: 2006/03/12 16:34:34 $
Version:   $Revision: 1.8 $

=========================================================================auto=*/
#include "vtkMRMLNode.h"
#include "vtkMRMLScene.h"

#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkMRMLNode* vtkMRMLNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLNode");
  if(ret)
    {
      return (vtkMRMLNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return NULL;
}

//------------------------------------------------------------------------------
vtkMRMLNode::vtkMRMLNode()
{
  this->ID = NULL;
  
  // By default nodes have no effect on indentation
  this->Indent = 0;

  // Strings
  this->Description = NULL;

  // By default all MRML nodes have a blank name
  // Must set name to NULL first so that the SetName
  // macro will not free memory.
  this->Name = NULL;
  this->SetName("");

  this->SceneRootDir = NULL;

  this->ReferenceNode = NULL;
  this->ReferencingNode = NULL;
  this->Scene = NULL;
}

//----------------------------------------------------------------------------
vtkMRMLNode::~vtkMRMLNode()
{
  this->SetDescription(NULL);
  this->SetName(NULL);
  this->SetID(NULL);

  if (this->ReferenceNode) {
    this->ReferenceNode->Delete();
  }
  if (this->ReferencingNode) {
    this->ReferencingNode->Delete();
  }
}

//----------------------------------------------------------------------------
void vtkMRMLNode::Copy(vtkMRMLNode *node)
{
  this->SetDescription(node->GetDescription());
  this->SetName(node->GetName());
  this->SetID( node->GetID() );

  this->SetScene(node->GetScene());
}

//----------------------------------------------------------------------------
void vtkMRMLNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os,indent);

  os << indent << "ID:          " << this->ID << "\n";

  os << indent << "Indent:      " << this->Indent << "\n";

  os << indent << "Name: " <<
    (this->Name ? this->Name : "(none)") << "\n";

  os << indent << "Description: " <<
    (this->Description ? this->Description : "(none)") << "\n";

}

//----------------------------------------------------------------------------
void vtkMRMLNode::WriteXML(ostream& of, int nIndent)
{
  vtkErrorMacro("NOT IMPLEMENTED YET");
}

//----------------------------------------------------------------------------
void vtkMRMLNode::ReadXMLAttributes(const char** atts)
{
  const char* attName;
  const char* attValue;
  while (*atts != NULL) {
    attName = *(atts++);
    attValue = *(atts++);
    if (!strcmp(attName, "ID")) {
      this->SetID(attValue);
    }
    else if (!strcmp(attName, "Name")) {
      this->SetName(attValue);
    }
    else if (!strcmp(attName, "Description")) {
      this->SetDescription(attValue);
    }
  } 
  return;
}

//----------------------------------------------------------------------------

vtkMRMLNode* vtkMRMLNode::GetLastReferencedNode()
{
  if (this->ReferenceNode != NULL) {
    return this->ReferenceNode->GetLastReferencedNode();
  }
  else {
    return this;
  }
}
//----------------------------------------------------------------------------

vtkMRMLNode* vtkMRMLNode::GetFirstReferencingNode()
{
  if (this->ReferencingNode != NULL) {
    return this->ReferencingNode->GetFirstReferencingNode();
  }
  else {
    return this;
  }
}

//----------------------------------------------------------------------------
void vtkMRMLNode::DeleteReference()
{
  if (this->ReferenceNode != NULL) {
    this->ReferenceNode->DeleteReference();
  }
  ReferenceNode=NULL;
}

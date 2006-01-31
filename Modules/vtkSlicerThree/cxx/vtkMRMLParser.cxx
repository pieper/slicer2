/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLParser.cxx,v $
Date:      $Date: 2006/01/31 12:34:01 $
Version:   $Revision: 1.1 $

=========================================================================auto=*/
#include "vtkObjectFactory.h"
#include "vtkMRMLParser.h"


//------------------------------------------------------------------------------
vtkMRMLParser* vtkMRMLParser::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLParser");
  if(ret) {
    return (vtkMRMLParser*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLParser;
}


void vtkMRMLParser::StartElement(const char* name, const char** atts)
{
  vtkMRMLNode* node = vtkMRMLScene::CreateNodeByClass( name );

  node->ReadXMLAttributes(atts);

  this->MRMLScene->AddNode(node);

  node->Delete();
}

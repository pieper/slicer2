/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLSceneManager.cxx,v $
Date:      $Date: 2006/03/10 21:23:45 $
Version:   $Revision: 1.1 $

=========================================================================auto=*/
#include "vtkMRMLSceneManager.h"

//------------------------------------------------------------------------------
vtkMRMLSceneManager::vtkMRMLSceneManager() 
{
  this->CurrentScene =  vtkMRMLScene::New();
  this->UndoStackSize = 100;
}

//------------------------------------------------------------------------------
vtkMRMLSceneManager::~vtkMRMLSceneManager() 
{
  this->CurrentScene->Delete();
}

//------------------------------------------------------------------------------
vtkMRMLSceneManager* vtkMRMLSceneManager::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLSceneManager");
  if(ret) {
    return (vtkMRMLSceneManager*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLSceneManager;
}

//------------------------------------------------------------------------------
void vtkMRMLSceneManager::CreateReferenceScene()
{
  if (this->GetCurrentScene() == NULL) {
    return;
  }

  vtkMRMLScene* newScene = vtkMRMLScene::New();

  vtkMRMLScene* currentScene = this->GetCurrentScene();
  
  int nnodes = currentScene->GetNumberOfItems();

  for (int n=0; n<nnodes; n++) {
    vtkMRMLNode *node  = dynamic_cast < vtkMRMLNode *>(currentScene->GetItemAsObject(n));
    if (node) {
      vtkMRMLNode *newNode = node->CreateNodeInstance();
      newNode->SetScene(newScene);
      newNode->SetReferenceNode(node);
      newScene->AddNode(newNode);
    }
  }

  //TODO check max stack size
  this->UndoStack.push_back(currentScene);
  this->CurrentScene = newScene;
  
}

//------------------------------------------------------------------------------
void vtkMRMLSceneManager::Undo()
{
  this->CurrentScene->RemoveAllItems();
  this->CurrentScene->Delete();
  this->CurrentScene = NULL;

  this->CurrentScene = dynamic_cast < vtkMRMLScene *>( this->UndoStack.back() );
  UndoStack.pop_back();
}


//------------------------------------------------------------------------------
void vtkMRMLSceneManager::PrintSelf(ostream& os, vtkIndent indent)
{

}


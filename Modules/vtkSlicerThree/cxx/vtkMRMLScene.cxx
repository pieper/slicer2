/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLScene.cxx,v $
Date:      $Date: 2006/03/12 16:34:34 $
Version:   $Revision: 1.14 $

=========================================================================auto=*/
#include <sstream>

#include "vtkMRMLScene.h"
#include "vtkMRMLParser.h"
#include "vtkObjectFactory.h"
#include "vtkErrorCode.h"

//------------------------------------------------------------------------------
vtkMRMLScene::vtkMRMLScene() 
{
  this->URL = NULL;
  this->ClassNameList = NULL;
  this->RegisteredNodeClasses.clear();
  this->UniqueIDByClass.clear();

  this->CurrentScene =  vtkCollection::New();
  this->UndoStackSize = 100;
  this->UndoFlag = true;
}

//------------------------------------------------------------------------------
vtkMRMLScene::~vtkMRMLScene() 
{
  if (this->URL) {
    delete this->URL;
  }
  if (this->ClassNameList) {
    delete this->ClassNameList;
  }
  this->CurrentScene->Delete();
}

//------------------------------------------------------------------------------
vtkMRMLScene* vtkMRMLScene::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLScene");
  if(ret) {
    return (vtkMRMLScene*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLScene;
}

//------------------------------------------------------------------------------
vtkMRMLNode* vtkMRMLScene::CreateNodeByClass(const char* className) 
{
  vtkMRMLNode* node = NULL;
  for (unsigned int i=0; i<RegisteredNodeClasses.size(); i++) {
    if (!strcmp(RegisteredNodeClasses[i]->GetClassName(), className)) {
      node = RegisteredNodeClasses[i]->CreateNodeInstance();
      break;
    }
  }
  // non-registered nodes can have a registered factory
  if (node == NULL) {
    vtkObject* ret = vtkObjectFactory::CreateInstance(className); 
    if(ret) {
      node = static_cast<vtkMRMLNode *>(ret);
    }
  }
  return node;
}

//------------------------------------------------------------------------------
void vtkMRMLScene::RegisterNodeClass(vtkMRMLNode* node) 
{
  this->RegisteredNodeClasses.push_back(node);
  this->RegisteredNodeTags.push_back(std::string(node->GetNodeTagName()));
}

//------------------------------------------------------------------------------
const char* vtkMRMLScene::GetClassNameByTag(const char *tagName)
{
  for (unsigned int i=0; i<RegisteredNodeTags.size(); i++) {
    if (!strcmp(RegisteredNodeTags[i].c_str(), tagName)) {
      return (RegisteredNodeClasses[i])->GetClassName();
    }
  }
  return NULL;
}

//------------------------------------------------------------------------------
int vtkMRMLScene::Connect()
{
  if (this->URL == NULL) {
    vtkErrorMacro("Need URL specified");
    return 0;
  }
  bool undoFlag = this->GetUndoFlag();
  this->SetUndoOff();

  this->CurrentScene->RemoveAllItems();
  vtkMRMLParser* parser = vtkMRMLParser::New();
  parser->SetMRMLScene(this);
  parser->SetFileName(URL);
  parser->Parse();
  parser->Delete();

  // create node references
  int nnodes = this->CurrentScene->GetNumberOfItems();
  vtkMRMLNode *node = NULL;
  for (int n=0; n<nnodes; n++) {
    node = (vtkMRMLNode *)this->CurrentScene->GetItemAsObject(n);
    node->UpdateScene(this);
  }

  this->SetUndoFlag(undoFlag);

  return 1;
}

//------------------------------------------------------------------------------
int vtkMRMLScene::Commit(const char* url)
{
  if (url == NULL) {
    url = URL;
  }

  vtkMRMLNode *node;
  ofstream file;
  int indent=0, deltaIndent;
  
  // Open file
  file.open(url);
  if (file.fail()) {
    vtkErrorMacro("Write: Could not open file " << url);
    cerr << "Write: Could not open file " << url;
#if (VTK_MAJOR_VERSION <= 5)      
    this->SetErrorCode(2);
#else
    this->SetErrorCode(vtkErrorCode::GetErrorCodeFromString("CannotOpenFileError"));
#endif
    return 1;
  }
  
  file << "<?xml version=\"1.0\" standalone='no'?>\n";
  file << "<!DOCTYPE MRML SYSTEM \"mrml20.dtd\">\n";
  file << "<MRML>\n";
   
  // Write each node
  for (int n=0; n < this->CurrentScene->GetNumberOfItems(); n++) {
    node = (vtkMRMLNode*)this->CurrentScene->GetItemAsObject(n);
    
    deltaIndent = node->GetIndent();
    if ( deltaIndent < 0 ) {
      indent -=2;
    }
    
    node->WriteXML(file, indent);
    
    if ( deltaIndent > 0 ) {
      indent += 2;
    }    
  }
  
  file << "</MRML>\n";
  
  // Close file
  file.close();
#if (VTK_MAJOR_VERSION <= 5)      
  this->SetErrorCode(0);
#else
  this->SetErrorCode(vtkErrorCode::GetErrorCodeFromString("NoError"));
#endif
  return 1;
}

//------------------------------------------------------------------------------
int vtkMRMLScene::GetNumberOfNodesByClass(const char *className)
{
  int num=0;
  vtkMRMLNode *node;
  for (int n=0; n < this->CurrentScene->GetNumberOfItems(); n++) {
    node = (vtkMRMLNode*)this->CurrentScene->GetItemAsObject(n);
    if (!strcmp(node->GetClassName(), className)) {
      num++;
    }
  }
  return num;
}

//------------------------------------------------------------------------------
std::list< std::string > vtkMRMLScene::GetNodeClassesList()
{
  std::list< std::string > classes;

  vtkMRMLNode *node;
  for (int n=0; n < this->CurrentScene->GetNumberOfItems(); n++) {
    node = (vtkMRMLNode*)this->CurrentScene->GetItemAsObject(n);
    classes.push_back(node->GetClassName());
  }
  classes.unique();
  return classes;
  
}

//------------------------------------------------------------------------------
const char* vtkMRMLScene::GetNodeClasses()
{
  std::list< std::string > classes = this->GetNodeClassesList();
  std::string classList;

  std::list< std::string >::const_iterator iter;
  // Iterate through list and output each element.
  for (iter = classes.begin(); iter != classes.end(); iter++) {
    if (!(iter == classes.begin())) {
      classList += std::string(" ");
    }
    classList.append(*iter);
  }
  SetClassNameList(classList.c_str());
  return this->ClassNameList;
}



//------------------------------------------------------------------------------
vtkMRMLNode *vtkMRMLScene::GetNextNodeByClass(const char *className)
{
  vtkMRMLNode *node = (vtkMRMLNode*)this->CurrentScene->GetNextItemAsObject();

  while (node != NULL && strcmp(node->GetClassName(), className)) {
    node = (vtkMRMLNode*)this->CurrentScene->GetNextItemAsObject();
  }
  return node;
}


//------------------------------------------------------------------------------
vtkMRMLNode* vtkMRMLScene::GetNthNode(int n)
{

  if(n < 0 || n >= this->CurrentScene->GetNumberOfItems()) {
    return NULL;
  }
  else {
    return (vtkMRMLNode*)this->CurrentScene->GetItemAsObject(n);
  }
}

//------------------------------------------------------------------------------
vtkMRMLNode* vtkMRMLScene::GetNthNodeByClass(int n, const char *className)
{
  int num=0;
  vtkMRMLNode *node;
  for (int nn=0; nn < this->CurrentScene->GetNumberOfItems(); nn++) {
    node = (vtkMRMLNode*)this->CurrentScene->GetItemAsObject(nn);
    if (!strcmp(node->GetClassName(), className)) {
      if (num == n) {
        return node;
      }
      num++;
    }
  }
  return NULL;
}

//------------------------------------------------------------------------------
vtkCollection* vtkMRMLScene::GetNodesByName(const char* name)
{

  vtkCollection* nodes = vtkCollection::New();

  vtkMRMLNode *node;
  for (int n=0; n < this->CurrentScene->GetNumberOfItems(); n++) {
    node = (vtkMRMLNode*)this->CurrentScene->GetItemAsObject(n);
    if (!strcmp(node->GetName(), name)) {
      nodes->AddItem(node);
    }
  }
  
  return nodes;
}


//------------------------------------------------------------------------------
vtkCollection* vtkMRMLScene::GetNodesByID(const char* id)
{

  vtkCollection* nodes = vtkCollection::New();

  vtkMRMLNode *node;
  for (int n=0; n < this->CurrentScene->GetNumberOfItems(); n++) {
    node = (vtkMRMLNode*)this->CurrentScene->GetItemAsObject(n);
    if (node->GetID() && !strcmp(node->GetID(), id)) {
      nodes->AddItem(node);
    }
  }
  
  return nodes;
}

//------------------------------------------------------------------------------
vtkCollection* vtkMRMLScene::GetNodesByClassByID(const char* className, const char* id)
{
  vtkCollection* nodes = vtkCollection::New();
  
  vtkMRMLNode *node;
  for (int n=0; n < this->CurrentScene->GetNumberOfItems(); n++) {
    node = (vtkMRMLNode*)this->CurrentScene->GetItemAsObject(n);
    if (node->GetID() && !strcmp(node->GetID(), id) && strcmp(node->GetClassName(), className) == 0) {
      nodes->AddItem(node);
    }
  }

  return nodes;
}

//------------------------------------------------------------------------------
vtkCollection* vtkMRMLScene::GetNodesByClassByName(const char* className, const char* name)
{
  vtkCollection* nodes = vtkCollection::New();
  
  vtkMRMLNode *node;
  for (int n=0; n < this->CurrentScene->GetNumberOfItems(); n++) {
    node = (vtkMRMLNode*)this->CurrentScene->GetItemAsObject(n);
    if (!strcmp(node->GetName(), name) && strcmp(node->GetClassName(), className) == 0) {
      nodes->AddItem(node);
    }
  }

  return nodes;
}

//------------------------------------------------------------------------------
int  vtkMRMLScene::GetTransformBetweenNodes(vtkMRMLNode *node1,
                                            vtkMRMLNode *node2, 
                                            vtkTransform *xform)
{
  vtkErrorMacro("NOT IMPLEMENTEED YET");
  return 1;
}

//------------------------------------------------------------------------------
int vtkMRMLScene::GetTransformBetweenSpaces( const char *space1, 
                                             const char *space2, 
                                             vtkTransform *xform )
{
  vtkErrorMacro("NOT IMPLEMENTEED YET");
  return 1;
}


//------------------------------------------------------------------------------
void vtkMRMLScene::InsertAfterNode(vtkMRMLNode *item, vtkMRMLNode *n)
{
  vtkErrorMacro("NOT IMPLEMENTEED YET");
}

//------------------------------------------------------------------------------
void vtkMRMLScene::InsertBeforeNode(vtkMRMLNode *item, vtkMRMLNode *n)
{
  vtkErrorMacro("NOT IMPLEMENTEED YET");
}

//------------------------------------------------------------------------------
void vtkMRMLScene::PrintSelf(ostream& os, vtkIndent indent)
{
  this->CurrentScene->vtkCollection::PrintSelf(os,indent);
  std::list<std::string> classes = this->GetNodeClassesList();

  std::list< std::string >::const_iterator iter;
  // Iterate through list and output each element.
  for (iter = classes.begin(); iter != classes.end(); iter++) {
    std::string className = (*iter);
    os << indent << "Number Of Nodes for class " << className.c_str() << " : " << this->GetNumberOfNodesByClass(className.c_str()) << "\n";
  }
}

//------------------------------------------------------------------------------
const char* vtkMRMLScene::GetUniqueIDByClass(const char* className)
{
  std::string sname(className);
  if (UniqueIDByClass.find(sname) == UniqueIDByClass.end() ) {
    UniqueIDByClass[className] = 1;
  }
  int id = UniqueIDByClass[sname];

  std::string name;

  while (true) {
    std::stringstream ss;
    ss << className;
    ss << id;
    name = ss.str();
    bool nameExists = false;
    for (int i=0; i< UniqueIDs.size(); i++) {
      if (UniqueIDs[i] == name) {
        nameExists = true;
        break;
      }
    }
    if (nameExists) {
      id++;
      continue;
    }
    else {
      break;
    }
  }
  UniqueIDByClass[className] = id + 1;
  UniqueIDs.push_back(name);
  return UniqueIDs[UniqueIDs.size()-1].c_str();
}

//------------------------------------------------------------------------------
void vtkMRMLScene::CreateReferenceScene()
{
  if (this->CurrentScene == NULL) {
    return;
  }

  vtkCollection* newScene = vtkCollection::New();

  vtkCollection* currentScene = this->CurrentScene;
  
  int nnodes = currentScene->GetNumberOfItems();

  for (int n=0; n<nnodes; n++) {
    vtkMRMLNode *node  = dynamic_cast < vtkMRMLNode *>(currentScene->GetItemAsObject(n));
    if (node) {
      vtkMRMLNode *newNode = node->CreateNodeInstance();
      newNode->SetScene(this);
      newNode->SetReferenceNode(node);
      node->SetReferencingNode(newNode);
      newScene->AddItem(newNode);
    }
  }

  //TODO check max stack size
  this->UndoStack.push_back(currentScene);
  this->CurrentScene = newScene;
  
}

//------------------------------------------------------------------------------
void vtkMRMLScene::Undo()
{
  if (this->UndoStack.size() == 0) {
    return;
  }

  vtkCollection *currentScene = dynamic_cast < vtkCollection *>( this->UndoStack.back() );

  int nnodes = currentScene->GetNumberOfItems();
  for (int n=0; n<nnodes; n++) {
    vtkMRMLNode *node  = dynamic_cast < vtkMRMLNode *>(currentScene->GetItemAsObject(n));
    if (node) {
      if (node->GetReferencingNode() != NULL) {
        node->GetReferencingNode()->Delete();
        node->SetReferencingNode(NULL);
      }
    }
  }

  this->CurrentScene->RemoveAllItems();
  this->CurrentScene->Delete();
  this->CurrentScene = currentScene;

  UndoStack.pop_back();
}


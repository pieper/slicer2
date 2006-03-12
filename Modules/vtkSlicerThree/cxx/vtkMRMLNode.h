/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLNode.h,v $
  Date:      $Date: 2006/03/12 16:34:34 $
  Version:   $Revision: 1.14 $

=========================================================================auto=*/
// .NAME vtkMRMLNode - Abstract Superclass for all specific types of MRML nodes.
// .SECTION Description
// This node encapsulates the functionality common to all types of MRML nodes.
// This includes member variables for ID, Description, and Options,
// as well as member functions to Copy() and Write().

#ifndef __vtkMRMLNode_h
#define __vtkMRMLNode_h

#include "vtkObject.h"

#include "vtkMRML.h"


#define MRMLUndoableSetMacro(name,type,className) \
void className::UndoableSet##name (type _arg) \
{ \
  className *node = this; \
  if (GetScene()->GetUndoFlag() == true) { \
    this->GetScene()->CreateReferenceScene(); \
    node = dynamic_cast < className *> (this->GetFirstReferencingNode()); \
    className *refNode = dynamic_cast < className *> (this->GetLastReferencedNode()); \
    if (refNode != NULL && node != NULL) { \
      node->Copy(refNode); \
    } \
    node->ReferenceNode=NULL; \
  } \
  if (node->name != _arg) { \
    node->name = _arg; \
    node->Modified(); \
  } \
} 

#define MRMLGetMacro(name,type,className) \
type className::Get##name () { \
    className *node = dynamic_cast < className *> (this->GetFirstReferencingNode()); \
    node = dynamic_cast < className *> (node->GetLastReferencedNode()); \
    return node->##name; \
} 

#define MRMLUndoableSetStringMacro(name,className) \
void className::UndoableSet##name (const char* _arg) \
{ \
  className *node = this; \
  if (GetScene()->GetUndoFlag() == true) { \
    this->GetScene()->CreateReferenceScene(); \
    node = dynamic_cast < className *> (this->GetFirstReferencingNode()); \
    className *refNode = dynamic_cast < className *> (this->GetLastReferencedNode()); \
    if (refNode != NULL && node != NULL) { \
      node->Copy(refNode); \
    } \
    node->ReferenceNode=NULL; \
  } \
  if ( node->name == NULL && _arg == NULL) { return;} \
  if ( node->name && _arg && (!strcmp(node->name,_arg))) { return;} \
  if (node->name) { delete [] node->name; } \
  if (_arg) \
    { \
    node->name = new char[strlen(_arg)+1]; \
    strcpy(node->name,_arg); \
    } \
   else \
    { \
    node->name = NULL; \
    } \
  node->Modified(); \
  } 

#define MRMLGetStringMacro(name, className) \
char* className::Get##name () { \
    className *node = dynamic_cast < className *> (this->GetFirstReferencingNode()); \
    node = dynamic_cast < className *> (node->GetLastReferencedNode()); \
    return node->##name; \
} 

class vtkMRMLScene;

class VTK_EXPORT vtkMRMLNode : public vtkObject
{
public:
  static vtkMRMLNode *New();
  vtkTypeMacro(vtkMRMLNode,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Create instance of the default node. Like New only virtual
  // NOTE: Subclasses should implement this method
  virtual vtkMRMLNode* CreateNodeInstance() = 0;

  // Description:
  // Set node attributes
  // NOTE: Subclasses should implement this method
  // NOTE: Call this method in the subclass impementation
  virtual void ReadXMLAttributes(const char** atts);

  // Description:
  // Set dependencies between this node and the parent node
  // when parsing XML file
  virtual void ProcessParentNode(vtkMRMLNode *parentNode){};

  // Description:
  // Set dependencies between this node and a child node
  // when parsing XML file
  virtual void ProcessChildNode(vtkMRMLNode *childNode){};
  
  // Description:
  // Updates other nodes in the scene depending on this node
  // This method is called automatically by XML parser after all nodes are created
  virtual void UpdateScene(vtkMRMLScene *scene) {};

  // Description:
  // Write this node's information to a MRML file in XML format.
  // NOTE: Subclasses should implement this method
  // NOTE: Call this method in the subclass impementation
  virtual void WriteXML(ostream& of, int indent);
  
  // Description:
  // Copy everything from another node of the same type.
  // NOTE: Subclasses should implement this method
  // NOTE: Call this method in the subclass impementation
  virtual void Copy(vtkMRMLNode *node);

  // Description:
  // Get node XML tag name (like Volume, Model)
  // NOTE: Subclasses should implement this method
  virtual const char* GetNodeTagName() = 0;
  
  // Description:
  // Text description of this node, to be set by the user
  vtkSetStringMacro(Description);
  vtkGetStringMacro(Description);

  // Description:
  // Root directory of MRML scene
  vtkSetStringMacro(SceneRootDir);
  vtkGetStringMacro(SceneRootDir);
  
  // Description:
  // Name of this node, to be set by the user
  vtkSetStringMacro(Name);
  vtkGetStringMacro(Name);
  
  
  // Description:
  // Node's effect on indentation when displaying the
  // contents of a MRML file. (0, +1, -1)
  vtkGetMacro(Indent, int);
  
  // Description:
  // ID use by other nodes to reference this node in XML
  vtkSetStringMacro(ID);
  vtkGetStringMacro(ID);


  vtkGetObjectMacro(ReferenceNode, vtkMRMLNode);
  vtkSetObjectMacro(ReferenceNode, vtkMRMLNode);

  vtkGetObjectMacro(ReferencingNode, vtkMRMLNode);
  vtkSetObjectMacro(ReferencingNode, vtkMRMLNode);


  vtkMRMLNode* GetLastReferencedNode();
  vtkMRMLNode* GetFirstReferencingNode();

  void DeleteReference();
  
  vtkMRMLScene* GetScene() {return this->Scene;};
  void SetScene(vtkMRMLScene* scene) {this->Scene = scene;};


protected:
  
  vtkMRMLNode();
  // critical to have a virtual destructor!
  virtual ~vtkMRMLNode();
  vtkMRMLNode(const vtkMRMLNode&);
  void operator=(const vtkMRMLNode&);
  
  vtkSetMacro(Indent, int);
  
  char *Description;
  char *SceneRootDir;
  char *Name;
  char *ID;
  int Indent;

  vtkMRMLNode *ReferenceNode;
  vtkMRMLNode *ReferencingNode;
  vtkMRMLScene *Scene;
};

#endif




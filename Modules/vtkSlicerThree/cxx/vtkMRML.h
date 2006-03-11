/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   MRML
  Module:    $RCSfile: vtkMRML.h,v $
  Date:      $Date: 2006/03/11 19:51:14 $
  Version:   $Revision: 1.2 $

=========================================================================auto=*/

/*
 * This is needed for loading mrml code as module.
 */


//
// use an ifdef on MRML_VTK5 to flag code that won't
// compile on vtk4.4 and before
//
#if ( (VTK_MAJOR_VERSION >= 5) || ( VTK_MAJOR_VERSION == 4 && VTK_MINOR_VERSION >= 5 ) )
#define MRML_VTK5
#endif

#if defined(WIN32) && !defined(VTKMRML_STATIC)
#if defined(MRML_EXPORTS)
#define VTK_MRML_EXPORT __declspec( dllexport ) 
#else
#define VTK_MRML_EXPORT __declspec( dllimport ) 
#endif
#else
#define VTK_MRML_EXPORT
#endif

#include "vtkSetGet.h"

#define MRMLNodeSetMacro(name,type) \
virtual void Set##name (type _arg) \
{ \
  if (GetScene()->GetUndoFlag() == true) { \
    this->GetScene()->CreateReferenceScene(); \
    this->Copy(this->GetReferenceNode()); \
    this->DeleteReferenceNode(); \
  } \
  if (this->name != _arg) { \
    this->name = _arg; \
    this->Modified(); \
  } \
} 

#define MRMLNodeGetMacro(name,type,className) \
virtual type Get##name () { \
  if (this->ReferenceNode != NULL) { \
    className *node = dynamic_cast < className *> (this->ReferenceNode); \
    return node->Get##name (); \
  } \
  else { \
    return this->name; \
  } \
} 

#define MRMLNodeSetStringMacro(name) \
virtual void Set##name (const char* _arg) \
{ \
  if (GetScene()->GetUndoFlag() == true) { \
    this->GetScene()->CreateReferenceScene(); \
    this->Copy(this->GetReferenceNode()); \
    this->DeleteReferenceNode(); \
  } \
  if ( this->name == NULL && _arg == NULL) { return;} \
  if ( this->name && _arg && (!strcmp(this->name,_arg))) { return;} \
  if (this->name) { delete [] this->name; } \
  if (_arg) \
    { \
    this->name = new char[strlen(_arg)+1]; \
    strcpy(this->name,_arg); \
    } \
   else \
    { \
    this->name = NULL; \
    } \
  this->Modified(); \
  } 

#define MRMLNodeGetStringMacro(name, className) \
virtual char* Get##name () { \
  if (this->ReferenceNode != NULL) { \
    className *node = dynamic_cast < className *> (this->ReferenceNode); \
    return node->Get##name (); \
  } \
  else { \
    return this->name; \
  } \
} 

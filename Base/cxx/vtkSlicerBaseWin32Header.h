/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSlicerBaseWin32Header.h,v $
  Language:  C++
  Date:      $Date: 2002/05/08 18:59:29 $
  Version:   $Revision: 1.1 $

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSlicerBaseWin32Header - manage Windows system differences
// .SECTION Description
// The vtkSlicerBaseWin32Header captures some system differences between Unix
// and Windows operating systems. 

#ifndef __vtkSlicerBaseWin32Header_h
#define __vtkSlicerBaseWin32Header_h

#include <vtkSlicerConfigure.h>

#if defined(WIN32) && !defined(VTKSLICER_STATIC)
#if defined(vtkSlicerBase_EXPORTS)
#define VTK_SLICER_BASE_EXPORT __declspec( dllexport ) 
#else
#define VTK_SLICER_BASE_EXPORT __declspec( dllimport ) 
#endif
#else
#define VTK_SLICER_BASE_EXPORT
#endif

#endif

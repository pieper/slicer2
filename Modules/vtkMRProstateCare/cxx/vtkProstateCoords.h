/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkProstateCoords.h,v $
  Date:      $Date: 2006/07/20 18:31:06 $
  Version:   $Revision: 1.1.2.1 $

=========================================================================auto=*/



#ifndef __vtkProstateCoords_h
#define __vtkProstateCoords_h

#include "vtkObject.h"
#include <vtkMRProstateCareConfigure.h>


class VTK_MRPROSTATECARE_EXPORT vtkProstateCoords : public vtkObject 
{
public:
    static vtkProstateCoords *New();
    vtkTypeMacro(vtkProstateCoords, vtkObject);

    vtkProstateCoords();
    ~vtkProstateCoords();

    int SetFileName(int in, const char *name);
    int Run();

private:
    char *FileNameIn;
    char *FileNameOut;
};

#endif


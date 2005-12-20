/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkDataIntervalDrop.h,v $
  Date:      $Date: 2005/12/20 22:55:56 $
  Version:   $Revision: 1.3.2.1 $

=========================================================================auto=*/
#ifndef __vtkDataIntervalDrop_h
#define __vtkDataIntervalDrop_h

#include "vtkIntervalDrop.h"
#include "vtkDataObject.h"

class VTK_EXPORT vtkDataIntervalDrop : public vtkIntervalDrop {
 public:
    static vtkDataIntervalDrop *New ();
    vtkTypeRevisionMacro(vtkDataIntervalDrop, vtkIntervalDrop);
    void PrintSelf(ostream& os, vtkIndent indent);

    vtkDataObject *dropData;

 protected:
    vtkDataIntervalDrop ();
    vtkDataIntervalDrop ( vtkTransform& tr);
    ~vtkDataIntervalDrop ();

 private:
    vtkDataIntervalDrop(const vtkDataIntervalDrop&); //Not implemented
    void operator=(const vtkDataIntervalDrop&); //Not implemented
};
#endif

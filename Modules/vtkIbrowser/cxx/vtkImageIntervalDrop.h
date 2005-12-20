/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageIntervalDrop.h,v $
  Date:      $Date: 2005/12/20 22:55:56 $
  Version:   $Revision: 1.3.2.1 $

=========================================================================auto=*/
#ifndef __vtkImageIntervalDrop_h
#define __vtkImageIntervalDrop_h

#include "vtkIntervalDrop.h"
#include "vtkImageData.h"


class VTK_EXPORT vtkImageIntervalDrop : public vtkIntervalDrop {

 public:
    static vtkImageIntervalDrop *New();        
    void PrintSelf (ostream& os, vtkIndent indent );
    vtkTypeRevisionMacro (vtkImageIntervalDrop, vtkIntervalDrop );

    // image data.
    vtkImageData *dropData;
    
 protected:
    vtkImageIntervalDrop ( );
    vtkImageIntervalDrop ( vtkTransform& t);
    vtkImageIntervalDrop ( vtkImageInterval *ref );
    ~vtkImageIntervalDrop ( );
        
 private:
};

#endif


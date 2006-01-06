/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageFrameSource.h,v $
  Date:      $Date: 2006/01/06 17:56:41 $
  Version:   $Revision: 1.15 $

=========================================================================auto=*/
// .NAME vtkImageFrameSource - Pulls data from a RenderWindow and outputs it.
// .SECTION Description
// Used to make a matching window in the Twin slicer module.
//
#ifndef __vtkImageFrameSource_h
#define __vtkImageFrameSource_h

#include "vtkImageData.h"
#include "vtkImageSource.h"
#include "vtkRenderWindow.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkImageFrameSource : public vtkImageSource 
{
public:
    static vtkImageFrameSource *New();
  vtkTypeMacro(vtkImageFrameSource,vtkImageSource);
     void PrintSelf(ostream& os, vtkIndent indent);

    void SetExtent(int xMin, int xMax, int yMin, int yMax);

    void ExecuteInformation();

    vtkSetObjectMacro(RenderWindow, vtkRenderWindow);
    vtkGetObjectMacro(RenderWindow, vtkRenderWindow);

protected:
    vtkImageFrameSource();
    ~vtkImageFrameSource();
  vtkImageFrameSource(const vtkImageFrameSource&) {};
  void operator=(const vtkImageFrameSource&) {};

    int WholeExtent[6];
    vtkRenderWindow *RenderWindow;

    void Execute(vtkImageData *data);
};


#endif

  

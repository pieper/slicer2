/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkPruneStreamline.h,v $
  Date:      $Date: 2005/12/20 22:55:07 $
  Version:   $Revision: 1.4.2.1 $

=========================================================================auto=*/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkPruneStreamline.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPruneStreamline - transform points and associated normals and vectors for polygonal dataset
// .SECTION Description
// vtkPruneStreamline is a filter to transform point
// coordinates and associated point and cell normals and
// vectors. Other point and cell data is passed through the filter
// unchanged. This filter is specialized for polygonal data. See
// vtkTransformFilter for more general data.
//
// An alternative method of transformation is to use vtkActor's methods
// to scale, rotate, and translate objects. The difference between the
// two methods is that vtkActor's transformation simply effects where
// objects are rendered (via the graphics pipeline), whereas
// vtkPruneStreamline actually modifies point coordinates in the 
// visualization pipeline. This is necessary for some objects 
// (e.g., vtkProbeFilter) that require point coordinates as input.

// .SECTION See Also
// vtkTransform vtkTransformFilter vtkActor

#ifndef __vtkPruneStreamline_h
#define __vtkPruneStreamline_h

#include "vtkDTMRIConfigure.h"

#include "vtkPolyDataToPolyDataFilter.h"
#include "vtkShortArray.h"
#include "vtkIntArray.h"


class VTK_DTMRI_EXPORT vtkPruneStreamline : public vtkPolyDataToPolyDataFilter
{
public:
  static vtkPruneStreamline *New();
  vtkTypeRevisionMacro(vtkPruneStreamline,vtkPolyDataToPolyDataFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Return the MTime also considering the ROI values.
  unsigned long GetMTime();

  // Description:
  // Specify an array with the ROI signatures.
  vtkSetObjectMacro(ANDROIValues,vtkShortArray);
  vtkGetObjectMacro(ANDROIValues,vtkShortArray);
  
  vtkSetObjectMacro(NOTROIValues,vtkShortArray);
  vtkGetObjectMacro(NOTROIValues,vtkShortArray);
  
  // Description:
  // List of streamlines Ids that pass the test
  vtkGetObjectMacro(StreamlineIdPassTest,vtkIntArray);

  //Description:
  // Number of positives that we have to get before declaring that a
  // streamline passes through a given ROI.
  vtkSetMacro(Threshold,int);
  vtkGetMacro(Threshold,int);
  
protected:
  vtkPruneStreamline();
  ~vtkPruneStreamline();

  void Execute();
  
  vtkShortArray *ANDROIValues;
  vtkShortArray *NOTROIValues;
  vtkIntArray *StreamlineIdPassTest;
  int Threshold;
  
  int TestForStreamline(int *streamlineANDTest,int nptsAND, int *streamlineNOTTest, int nptsNOT);
  
private:
  vtkPruneStreamline(const vtkPruneStreamline&);  // Not implemented.
  void operator=(const vtkPruneStreamline&);  // Not implemented.
};

#endif



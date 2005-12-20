/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkTimecourseExtractor.h,v $
  Date:      $Date: 2005/12/20 22:55:34 $
  Version:   $Revision: 1.1.2.1 $

=========================================================================auto=*/
/*==============================================================================
(c) Copyright 2004 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
==============================================================================*/

#ifndef __vtkTimecourseExtractor_h
#define __vtkTimecourseExtractor_h


#include <vtkFSLReaderConfigure.h>
#include "vtkFloatArray.h"
#include "vtkSimpleImageToImageFilter.h"


class  VTK_FSLREADER_EXPORT vtkTimecourseExtractor : public vtkSimpleImageToImageFilter
{
public:
    static vtkTimecourseExtractor *New();
    vtkTypeMacro(vtkTimecourseExtractor, vtkSimpleImageToImageFilter);

    // Description:
    // Returns the time course of a specified voxel (i, j, k).
    vtkFloatArray *GetTimeCourse(int i, int j, int k);

    // Description:
    // Adds an input to the input list. Expands the list memory if necessary.
    void AddInput(vtkImageData *input);

    // Description:
    // Get one input whose index is "indx" on the input list.
    vtkImageData *GetInput(int indx);

protected:
    vtkTimecourseExtractor();
    ~vtkTimecourseExtractor();

    void SimpleExecute(vtkImageData* input,vtkImageData* output);
};


#endif

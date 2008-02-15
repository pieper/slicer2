/*=auto=========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: ICPRegistration.h,v $
  Date:      $Date: 2008/02/15 23:19:47 $
  Version:   $Revision: 1.1.2.1 $

=========================================================================auto=*/

/**
 *  class ICPRegistration from Slicer Image-Guided Navigator (The SIGN) 
 *
 *  Iterative Closest Point (ICP) registration
 *
 *  This class uses ITK methods to perform ICP registration.
 *
 *  Author Arne Hans
 *
 */


#ifndef __ICPRegistration_H
#define __ICPRegistration_H

#include "vtkSlicerBaseWin32Header.h"


class vtkMatrix4x4;
class vtkPoints;


class VTK_SLICER_BASE_EXPORT ICPRegistration
{
public:
    ICPRegistration();
    ~ICPRegistration();

    /// Set implementation to use, default is VTK
    void UseVTK();
    /// Set implementation to use, default is VTK
    void UseITK();
    /// Compute rigid registraion matrix between two point clouds
    vtkMatrix4x4* RegisterWithICP(vtkPoints* fixedPoints, vtkPoints* movingPoints);

private:
    vtkMatrix4x4* RegisterWithITKICP(vtkPoints* fixedPoints, vtkPoints* movingPoints);
    vtkMatrix4x4* RegisterWithVTKICP(vtkPoints* fixedPoints, vtkPoints* movingPoints);

    bool useVTK;
};

#endif

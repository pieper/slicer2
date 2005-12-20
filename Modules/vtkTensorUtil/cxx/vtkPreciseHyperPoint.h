/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkPreciseHyperPoint.h,v $
  Date:      $Date: 2005/12/20 22:56:25 $
  Version:   $Revision: 1.4.8.1 $

=========================================================================auto=*/
#ifndef __vtkPreciseHyperPoint_h
#define __vtkPreciseHyperPoint_h

//BTX

class vtkPreciseHyperPoint { //;prevent man page generation
public:
    vtkPreciseHyperPoint(); // method sets up storage
    vtkPreciseHyperPoint &operator=(const vtkPreciseHyperPoint& hp); //for resizing
    
    vtkFloatingPointType   X[3];    // position 
    vtkIdType CellId;  // cell
    int     SubId; // cell sub id
    vtkFloatingPointType   P[3];    // parametric coords in cell 
    vtkFloatingPointType   W[3];    // eigenvalues (sorted in decreasing value)
    vtkFloatingPointType   *V[3];   // pointers to eigenvectors (also sorted)
    vtkFloatingPointType   V0[3];   // storage for eigenvectors
    vtkFloatingPointType   V1[3];
    vtkFloatingPointType   V2[3];
    vtkFloatingPointType   S;       // scalar value 
    vtkFloatingPointType   D;       // distance travelled so far 
};
//ETX

#endif

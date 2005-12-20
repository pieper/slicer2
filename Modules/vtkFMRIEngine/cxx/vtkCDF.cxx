/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkCDF.cxx,v $
  Date:      $Date: 2005/12/20 22:55:28 $
  Version:   $Revision: 1.6.2.1 $

=========================================================================auto=*/

#include "vtkObjectFactory.h"
#include "vtkCDF.h"
#include "itkTDistribution.h"


vtkStandardNewMacro(vtkCDF);


double vtkCDF::p2t(double p, long dof)
{
    // p value passed in is double sided probability
    return fabs(itk::Statistics::TDistribution::InverseCDF((p / 2), dof));
}


double vtkCDF::t2p(double t, long dof)
{
    double p = itk::Statistics::TDistribution::CDF(t, dof);

    // double sided tail probability for t-distribution
    p *= 2;

    return p; 
}


/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: Point.h,v $
  Date:      $Date: 2007/07/02 19:12:25 $
  Version:   $Revision: 1.1.2.1 $

=========================================================================auto=*/
#ifndef _Point_h
#define _Point_h
#include <vtkNeuroendoscopyConfigure.h>


class  Point {

public:

    void setX(double value);
    void setY(double value);
    void setZ(double value);

    double getX(void);
    double getY(void);
    double getZ(void);

private:

    double X;
    double Y;
    double Z;

};

#endif

/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: Point.cxx,v $
  Date:      $Date: 2007/07/02 19:12:25 $
  Version:   $Revision: 1.1.2.1 $

=========================================================================auto=*/
#ifndef _Point_cxx
#define _Point_cxx

#include "Point.h"


void Point::setX(double value) {
    X = value;
}
void Point::setY(double value) {
    Y = value;
}
void Point::setZ(double value) {
    Z = value;
}
double Point::getX(void) {
    return X;
}
double Point::getY(void) {
    return Y;
}
double Point::getZ(void) {
    return Z;
}

#endif


/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkNeuroendoscopy.h,v $
  Date:      $Date: 2007/10/10 20:06:15 $
  Version:   $Revision: 1.2.2.4 $

=========================================================================auto=*/
#ifndef __vtkNeuroendoscopy_h
#define __vtkNeuroendoscopy_h

//#include "vtkObjectFactory.h"
#include <vtkObject.h>
#include <vtkNeuroendoscopyConfigure.h>
//  #include <stl>
#include <vtkMath.h>

//#include "Point.h"
//#include "vtkCapture.h"

#include <pthread.h>
#include  "xc.h"

#include "vtkCapture.h"

#include <vector>
#include <vtkPointSet.h>
#include <vtkRenderer.h>
#include <vtkDataSet.h>
//#include "eigenvalues/feigen.h"
//#include "eigenvalues/basis.h"


class VTK_NEUROENDOSCOPY_EXPORT vtkNeuroendoscopy  : public vtkObject {
public:
    static vtkNeuroendoscopy *New();
    //void PrintSelf(ostream& os, vtkIndent indent);
    
   // char *GetEndoscopyNEWVersion();
    vtkTypeMacro(vtkNeuroendoscopy, vtkObject);

    // p - p value
    // dof - degrees of freedom 
    // The function returns t statistic.
    double p2t(double p, long dof);

    // Description:
    // Converts t statistic to p value
    // t - t statistic 
    // dof - degrees of freedom 
    // The function returns p value.
    double t2p(double t, long dof);

    //Calculates the required angle from the cone to the projection frustum silength
    double coneAngle2FrustumLength(double coneAngle);
    double closestDistanceCalculation(double p1[3], double p2[3]);
    XImage * getCurrentPic(void);
    void *  getCurrentPic2(void);
    void openXWindow(void);
    void GetZBuffer(vtkRenderer * renderer, int X, int Y);
    void GetZBuffer2(vtkRenderer * renderer, int X, int Y);
    double GetDistance(vtkRenderer * renderer, vtkDataSet * poly);
    
// void getTransformationMatrix(void);
//     
//      void addTargetPoint(double X, double Y, double Z);
//      void addControlPoint(double X, double Y, double Z);
//      void calcCovarianceMatrix(double **covMatrix);
//      void quaternionMatrix(double **q);
// //BTX
//      void meanPointCalculation(std::vector<Point> *p, Point *result);
// //ETX
// 
// private:
//     
// //BTX
//     //calculates the mean value of a given point.
//     //n is the number of valid points in the model
//     //k is the current point in the model
//     //co is the coordinate x,y or z
//   //  std::string test;
// 
//       std::vector<Point> controlPoints;
//       std::vector<Point> targetPoints;
// // 
// //ETX
//       Point meanTargetPoint;
//       Point meanControlPoint;
protected:
  XImage * curPic;

};

#endif

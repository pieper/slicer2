/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkNeuroendoscopy.cxx,v $
  Date:      $Date: 2007/07/02 19:34:18 $
  Version:   $Revision: 1.1.2.4 $

=========================================================================auto=*/
#ifndef _vtkNeuroendoscopy_cxx
#define _vtkNeuroendoscopy_cxx

#include "vtkObjectFactory.h"
#include "vtkNeuroendoscopy.h"

#include "math.h"


vtkStandardNewMacro(vtkNeuroendoscopy);



//char * vtkEndoscopyNEWVersion::GetEndoscopyNEWVersion()
//{
 //   return "TEST";
//}





double vtkNeuroendoscopy::p2t(double p, long dof)
{
    // p value passed in is double sided probability
    return (2 / 2);
}


double vtkNeuroendoscopy::t2p(double t, long dof)
{
    double p = 2;

    // double sided tail probability for t-distribution
    p *= 2;

    return p; 
}


//transfers the cone angle to the frustumlength
//calculated the side of the rectangle of the pyramid based on the distance 50 and the angle
double vtkNeuroendoscopy::coneAngle2FrustumLength(double coneAngle) {
    //angle between the rotation axis and the cone endge
    double radianAngle = ((2*M_PI)/360)*coneAngle;
    

    return (tan (radianAngle)*100);


}





//calculates the distance between two points
//euklidian distance
// double vtkNeuroendoscopy::closestDistanceCalculation(Point p1, Point p2) {
// 
//   return sqrt(pow((p1.getX() - p2.getX()),2) + pow((p1.getY() - p2.getY()),2) + pow((p1.getZ()-p2.getZ()),2));
// 
// }
// 
// void vtkNeuroendoscopy::addTargetPoint(double X, double Y, double Z) {
//   
//    Point p;
//    p.setX(X);
//    p.setY(Y);
//    p.setZ(Z);
//    targetPoints.push_back(p);
// 
// 
// }
// 
// void vtkNeuroendoscopy::addControlPoint(double X, double Y, double Z) {
//   
//    Point p;
//    p.setX(X);
//    p.setY(Y);
//    p.setZ(Z);
//    controlPoints.push_back(p);
// 
//  
// }
// 
// 
// 
// void vtkNeuroendoscopy::quaternionMatrix(void) {
//   double *covMatrix[2][2];
//   calcCovarianceMatrix(&covMatrix);
// 
//   //A matrix 
//   double a[2][2];
//     a[0][0] = *covMatrix[0][0] - *covMatrix[0][0];
//     a[1][0] = *covMatrix[1][0] - *covMatrix[0][1];
//     a[2][0] = *covMatrix[2][0] - *covMatrix[0][2];
//       
//     a[0][1] = *covMatrix[0][1] - *covMatrix[1][0];
//     a[1][1] = *covMatrix[1][1] - *covMatrix[1][1];
//     a[2][1] = *covMatrix[2][1] - *covMatrix[1][2];
// 
//     a[0][2] = *covMatrix[0][2] - *covMatrix[2][0];
//     a[1][2] = *covMatrix[1][2] - *covMatrix[2][1];
//     a[2][2] = *covMatrix[2][2] - *covMatrix[2][2];
// 
//    double d[2];
//    d[0] = a[1][2];
//    d[1] = a[2][0];
//    d[2] = a[0][1];
//    
//    double s;
//    s = *covMatrix[0][0] + *covMatrix[1][1] + *covMatrix[2][2];
// 
//    double t[2][2];
//     t[0][0] = *covMatrix[0][0] + *covMatrix[0][0] - s;
//     t[1][0] = *covMatrix[1][0] + *covMatrix[0][1];
//     t[2][0] = *covMatrix[2][0] + *covMatrix[0][2];
//       
//     t[0][1] = *covMatrix[0][1] + *covMatrix[1][0];
//     t[1][1] = *covMatrix[1][1] + *covMatrix[1][1] - s;
//     t[2][1] = *covMatrix[2][1] + *covMatrix[1][2];
// 
//     t[0][2] = *covMatrix[0][2] + *covMatrix[2][0];
//     t[1][2] = *covMatrix[1][2] + *covMatrix[2][1];
//     t[2][2] = *covMatrix[2][2] + *covMatrix[2][2] - s;
// 
//    double q[3][3];
//    q[0][0] = s;
//    q[1][0] = a[1][2];
//    q[2][0] = a[2][0];
//    q[3][0] = a[0][1];
//    
//    q[0][1] = a[1][2];
//    q[1][1] = t[0][0];
//    q[2][1] = t[1][0];
//    q[3][1] = t[2][0];
// 
//    q[0][2] = a[2][0];
//    q[1][2] = t[0][1];
//    q[2][2] = t[1][1];
//    q[3][2] = t[2][1];
// 
//    q[0][3] = a[0][1];
//    q[1][3] = t[0][2];
//    q[2][3] = t[1][2];
//    q[3][3] = t[2][2];
// }
// 
// void vtkNeuroendoscopy::calcCovarianceMatrix(double *covMatrix[2][2]) {
// 
//   //con
//   
//   if (targetPoints.size() == controlPoints.size()) {
//     Point *meanTargetPoint;
//     Point *meanControlPoint;
//    // double covMatrix[2][2];
//     
//     meanPointCalculation(&targetPoints , meanTargetPoint);
//     meanPointCalculation(&controlPoints , meanControlPoint);
// 
//     double c[2][2];
// 
//     for (int i = 0;i<targetPoints.size(); i++) {
//       c[0][0] = c[0][0] + (controlPoints.at(i).getX() * targetPoints.at(i).getX() - meanControlPoint->getX() * meanTargetPoint->getX());
//       c[1][0] = c[1][0] + (controlPoints.at(i).getY() * targetPoints.at(i).getX() - meanControlPoint->getY() * meanTargetPoint->getX());
//       c[2][0] = c[2][0] + (controlPoints.at(i).getZ() * targetPoints.at(i).getX() - meanControlPoint->getZ() * meanTargetPoint->getX());
//       
//       c[0][1] = c[0][1] + (controlPoints.at(i).getX() * targetPoints.at(i).getY() - meanControlPoint->getX() * meanTargetPoint->getY());
//       c[1][1] = c[1][1] + (controlPoints.at(i).getY() * targetPoints.at(i).getY() - meanControlPoint->getY() * meanTargetPoint->getY());
//       c[2][1] = c[2][1] + (controlPoints.at(i).getZ() * targetPoints.at(i).getY() - meanControlPoint->getZ() * meanTargetPoint->getY());
// 
//       c[0][2] = c[0][2] + (controlPoints.at(i).getX() * targetPoints.at(i).getZ() - meanControlPoint->getX() * meanTargetPoint->getZ());
//       c[1][2] = c[1][2] + (controlPoints.at(i).getY() * targetPoints.at(i).getZ() - meanControlPoint->getY() * meanTargetPoint->getZ());
//       c[2][2] = c[2][2] + (controlPoints.at(i).getZ() * targetPoints.at(i).getZ() - meanControlPoint->getZ() * meanTargetPoint->getZ());
//     }
//     
// 
//    *covMatrix[0][0] = c[0][0];
//    *covMatrix[1][0] = c[1][0];
//    *covMatrix[2][0] = c[2][0];
//    *covMatrix[0][1] = c[0][1];
//    *covMatrix[1][1] = c[1][1];
//    *covMatrix[2][1] = c[2][1];
//    *covMatrix[0][2] = c[0][2];
//    *covMatrix[1][2] = c[1][2];
//    *covMatrix[2][2] = c[2][2];
// 
// 
//   }
// }
// 
// 
// 
// //BTX
// void vtkNeuroendoscopy::meanPointCalculation(std::vector<Point> *p, Point *result) {
//   if (p->size() >= 3) {
//    double meanX = 0;
//    double meanY = 0;
//    double meanZ = 0;
//    double n = p->size();
// 
//    for (int i = 0;i<p->size(); i++) {
//     meanX = meanX + (p->at(i).getX() / n);
//         meanY = meanY + (p->at(i).getY() / n);
//     meanZ = meanZ + (p->at(i).getZ() / n);
//    }
//     result->setX(meanX);
//     result->setY(meanY);
//     result->setZ(meanZ);
// //     return true;
//   } else {
//     
// 
// //    return false;
//   }
// 
// 
// }
// //ETX



//this function calclulates coordinates of the 
//double vtkNeuroendoscopy::calcProjectorFocusPoint(double x, double y, double z) {


//}



#endif

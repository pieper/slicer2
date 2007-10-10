/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkNeuroendoscopy.cxx,v $
  Date:      $Date: 2007/10/10 20:05:35 $
  Version:   $Revision: 1.1.2.5 $

=========================================================================auto=*/
#ifndef _vtkNeuroendoscopy_cxx
#define _vtkNeuroendoscopy_cxx

#include "vtkObjectFactory.h"
#include "vtkNeuroendoscopy.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "math.h"
#include "vtkCamera.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkCellLocator.h"



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
double vtkNeuroendoscopy::closestDistanceCalculation(double p1[3], double p2[3]) {

return sqrt(pow((p1[0] - p2[0]),2) + pow((p1[1] - p2[1]),2) + pow((p1[2]-p2[2]),2));

}


XImage * vtkNeuroendoscopy::getCurrentPic(void) {
XImage * TmpPic = curPic;
  return curPic;
}
void *  vtkNeuroendoscopy::getCurrentPic2(void) {
//XImage * TmpPic = curPic;

  return vtkCapture::startCapture();
}
void vtkNeuroendoscopy::openXWindow(void) {

 //  xc test;
    //printf ("vor start\n");

int threadnr = 1;
int zahl;
pthread_t threads[threadnr];
int rc, t;
//mutex creation
pthread_mutex_t lock;
pthread_mutex_init(&lock,NULL);

pthread_attr_t attrCapture;
pthread_t tidCapture;

pthread_attr_init(&attrCapture);

pthread_create(&tidCapture,&attrCapture,xc::startwindow2,curPic);



//test.startwindow();

 // rc = pthread_create(&threads[0], NULL, test.startwindow(), (void *)t);

//pthread_exit(NULL);


   
}



void vtkNeuroendoscopy::GetZBuffer2 (vtkRenderer * renderer, int X, int Y) {

double zPtr;
zPtr = renderer->GetZ(X,Y);

double *aspects;
aspects =  renderer->GetAspect();



vtkTransform *transformDisp = vtkTransform::New();
transformDisp->SetMatrix(renderer->GetActiveCamera()->GetCompositePerspectiveTransformMatrix((aspects[0]/aspects[1]), 0 , 1));
transformDisp->Inverse();
 
vtkMatrix4x4 *matrixDisp = vtkMatrix4x4::New();
matrixDisp->DeepCopy( transformDisp->GetMatrix());

double wcoords[4];
double realcoord[4];
realcoord[0] = X;
realcoord[1] = Y;
realcoord[2] = zPtr;
realcoord[3] = 0;
matrixDisp->MultiplyPoint( realcoord,wcoords);
printf( "SetViewPoint => %f %f %f %f\n", wcoords[0], wcoords[1], wcoords[2],wcoords[3] );
}

double vtkNeuroendoscopy::GetDistance(vtkRenderer * renderer, vtkDataSet * poly) {

  double camPos[3];
  renderer->GetActiveCamera()->GetPosition(camPos);
  double camPointDirection[3];
  renderer->GetActiveCamera()->GetDirectionOfProjection(camPointDirection);
  vtkCellLocator *cellLocator = vtkCellLocator::New();
  cellLocator->SetDataSet(poly);
  cellLocator->BuildLocator();

  double t, ptline[3], pcoords[3];
  int subId;
  int result = cellLocator->IntersectWithLine(camPos, camPointDirection, 1, t, ptline, pcoords, subId);

  printf( "IntersectionPoint => %f %f %f\n", ptline[0], ptline[1],ptline[2] );
printf( "camPos => %f %f %f\n", camPos[0], camPos[1],camPos[2] );
printf( "PointDirection => %f %f %f\n", camPointDirection[0], camPointDirection[1],camPointDirection[2] );
cellLocator->Delete();
return closestDistanceCalculation(ptline,camPos);


}
void vtkNeuroendoscopy::GetZBuffer (vtkRenderer * renderer, int X, int Y) {

printf("=======================================================\n");
double x = X;
double y = Y;
double displayPt[3];
renderer->LocalDisplayToDisplay( x, y );
double *centerPt;
centerPt = renderer->GetCenter();
printf( "LocalDisplayToDisplay => %d %d => %f %f\n", X, Y, x, y );
renderer->SetDisplayPoint( centerPt[0], centerPt[1], 0 ); // z = 0 because I'm in the XY vue
renderer->DisplayToView( );
renderer->GetDisplayPoint(displayPt);
renderer->Modified();
double viewPt[3];
renderer->GetViewPoint(viewPt);
printf( "GetViewPoint => %f %f %f\n", viewPt[0], viewPt[1],viewPt[2] );
// displayPt[0] = 0.5;
// displayPt[1] = 0.5;
// displayPt[2] = renderer->GetZ(X,Y);
printf( "SetDisplayPoint => %f %f %f\n", displayPt[0], displayPt[1],displayPt[2] );
renderer->SetViewPoint( displayPt );
renderer->ViewToWorld( );
double worldPt[4];
renderer->GetWorldPoint( worldPt );
double camPos[3];
renderer->GetActiveCamera()->GetPosition(camPos);


printf( "SetViewPoint => %f %f %f %f\n", worldPt[0], worldPt[1], worldPt[2],worldPt[3] );
printf( "Camera Position => %f %f %f\n", camPos[0], camPos[1], camPos[2]);

float* bufferData;
bufferData = renderer->GetRenderWindow()->GetZbufferData(X-1,Y-1,X,Y);
printf( "Bufferdata => %f %f\n", bufferData[0], bufferData[1]);
printf("=======================================================\n");
//   vtkPointSet * output;
//   vtkRenderWindow * renderwindow = renderer->GetRenderWindow();
//   int * dims;
//   dims = renderwindow->GetSize();
//   float* zBuffer = renderwindow->GetZbufferData(0, 0, dims[0]-1, dims[1]-1);



//   float* zPtr = zBuffer;
  
//   float *rangeBuffer = new float[ dims[0] * dims[1] ];
//   float *rangeBuffer2 = new float[ dims[0] * dims[1] ];
// 
// 
//   // Some are copied from Lisa's vtkOpenGLProjectedPolydataRayBounder.cxx
//   rayCaster = this->Renderer->GetRayCaster();
// 
//   // Create some objects that we will need later
//   transformDisp       = vtkTransform::New();
//   matrixDisp          = vtkMatrix4x4::New();  
//   transformView       = vtkTransform::New();
//   matrixView          = vtkMatrix4x4::New();  
// 
//   // Get the aspect ratio of the RenderWindow
//   this->Renderer->GetAspect( renAspect );
//   aspect = renAspect[0]/renAspect[1];
//     
//   // Create the perspective matrix for the camera.  This will be used
//   // to decode z values, so we will need to invert it
//   transformDisp->SetMatrix(this->Renderer->GetActiveCamera()
//                 ->GetPerspectiveTransformMatrix(aspect,-1,1));
//   transformDisp->Inverse();
//     
//   // To speed things up, we pull the matrix out of the transform. 
//   // This way, we can decode z values faster since we know which elements
//   // of the matrix are important, and which are zero.
//   transformDisp->GetMatrix( matrixDisp );
//     
//   // These are the important elements of the matrix.  We will decode
//   // z values by taking the znum1 and dividing by the zbuffer z value times
//   // zdenom1 plus zdenom2.
//   zNumerator   = matrixDisp->Element[2][3];
//   zDenomMult   = matrixDisp->Element[3][2];
//   zDenomAdd    = matrixDisp->Element[3][3];
// 
//   rangePtr = rangeBuffer;
//   rayBuffer = rayCaster->GetPerspectiveViewRays();
//   rayPtr = rayBuffer;
//   rayPtr += 2;
// 
//   // Convert  the ZBuffer to range values
//   // j is row, i is column
//   for ( j = 0; j < dims[1]; j++ )
//     {
//     for ( i = 0; i < dims[0]; i++ )
//       {
//       *(rangePtr) = (-zNumerator / ( ((*(zPtr++))*2.0 -1.0) *
//            zDenomMult + zDenomAdd )) / (-(*rayPtr));
// 
//       rangePtr++;
//       rayPtr += 3;
//       }
//     }
// 
// 
//   // Create the view matrix.  This will be used to transform camera
//   // coordinates to world coordinates, so we will need to invert it
//   transformView
//     ->SetMatrix(this->Renderer->GetActiveCamera()->GetViewTransformMatrix());
//   transformView->Inverse();
//     
//   // To speed things up, we pull the matrix out of the transform.
//   transformView->GetMatrix( matrixView );
// 
//   // Use ray trace value to calculate camera coordinates
//   rayPtr = rayBuffer;
//   rangePtr = rangeBuffer;
// 
//   zPtr   = zBuffer;
//   zbuf_tolerance = 1.0e-5;
// 
//   int offset;
// 
//   point[3] = 1.0;
//   vtkDebugMacro( << "Outputting range/scalars" );
//   for ( j = 0; j < dims[1]; j++ )
//     {
//     for ( i = 0; i < dims[0]; i++ )
//       {
//       point[0] = (*rangePtr) * (*(rayPtr++));
//       point[1] = (*rangePtr) * (*(rayPtr++));
//       point[2] = (*rangePtr) * (*(rayPtr++));
// 
//       matrixView->MultiplyPoint(point, point1);
//       
//       offset = j*dims[0] + i;
//       point1[0] /= point1[3];
//       point1[1] /= point1[3];
//       point1[2] /= point1[3];
//       point1[3] = 1.0;
//       points->SetPoint(offset, point1);
// 
//       // Set range to -1 if the range falls outside the depth of field
//       if (*rangePtr <= this->MinDepthOfField
//           || *rangePtr >= this->MaxDepthOfField)
//         {
//       *rangePtr = -1;
//         }
// 
//       // Check for extreme bounds of the Z buffer
//       if (*zPtr < zbuf_tolerance || (1 - *zPtr) < zbuf_tolerance)
//     {
//     *rangePtr = -1;
//     }
// 
//       scalars->SetScalar(offset, *rangePtr);
//       rangePtr++;
//       zPtr++;
//       }
//     }



//   output->SetPoints(points);
//   output->GetPointData()->SetScalars(scalars);
//   return output;
}

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
// void vtkNeuroendoscopy::getTransformationMatrix(void) {
// REAL **eivec;
// REAL  *valre;
// REAL  *valim;
// int *cnt;
// double **qMatrix;
// quaternionMatrix(qMatrix);
// //ElmHes(qMatrix);
// int test = feigen::eigen(1,0,0,4,qMatrix,eivec,valre,valim,cnt);
// 
// }
// 
// 
// 
// void vtkNeuroendoscopy::quaternionMatrix(double **q) {
//   double **covMatrix;
//   calcCovarianceMatrix(covMatrix);
// 
//   //A matrix 
//   double a[2][2];
//     a[0][0] = covMatrix[0][0] - covMatrix[0][0];
//     a[1][0] = covMatrix[1][0] - covMatrix[0][1];
//     a[2][0] = covMatrix[2][0] - covMatrix[0][2];
//       
//     a[0][1] = covMatrix[0][1] - covMatrix[1][0];
//     a[1][1] = covMatrix[1][1] - covMatrix[1][1];
//     a[2][1] = covMatrix[2][1] - covMatrix[1][2];
// 
//     a[0][2] = covMatrix[0][2] - covMatrix[2][0];
//     a[1][2] = covMatrix[1][2] - covMatrix[2][1];
//     a[2][2] = covMatrix[2][2] - covMatrix[2][2];
// 
//    double d[2];
//    d[0] = a[1][2];
//    d[1] = a[2][0];
//    d[2] = a[0][1];
//    
//    double s;
//    s = covMatrix[0][0] + covMatrix[1][1] + covMatrix[2][2];
// 
//    double t[2][2];
//     t[0][0] = covMatrix[0][0] + covMatrix[0][0] - s;
//     t[1][0] = covMatrix[1][0] + covMatrix[0][1];
//     t[2][0] = covMatrix[2][0] + covMatrix[0][2];
//       
//     t[0][1] = covMatrix[0][1] + covMatrix[1][0];
//     t[1][1] = covMatrix[1][1] + covMatrix[1][1] - s;
//     t[2][1] = covMatrix[2][1] + covMatrix[1][2];
// 
//     t[0][2] = covMatrix[0][2] + covMatrix[2][0];
//     t[1][2] = covMatrix[1][2] + covMatrix[2][1];
//     t[2][2] = covMatrix[2][2] + covMatrix[2][2] - s;
// 
// 
//   //matrix first element is row and second column
//    //double q[3][3];
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
// void vtkNeuroendoscopy::calcCovarianceMatrix(double **covMatrix) {
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
//    covMatrix[0][0] = c[0][0];
//    covMatrix[1][0] = c[1][0];
//    covMatrix[2][0] = c[2][0];
//    covMatrix[0][1] = c[0][1];
//    covMatrix[1][1] = c[1][1];
//    covMatrix[2][1] = c[2][1];
//    covMatrix[0][2] = c[0][2];
//    covMatrix[1][2] = c[1][2];
//    covMatrix[2][2] = c[2][2];
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

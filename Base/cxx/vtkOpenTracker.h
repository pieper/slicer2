/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkOpenTracker.h,v $
  Date:      $Date: 2006/11/15 16:48:42 $
  Version:   $Revision: 1.1.2.1 $

=========================================================================auto=*/

#ifndef __vtkOpenTracker_h
#define __vtkOpenTracker_h

#include "vtkPoints.h"
#include "vtkMatrix4x4.h"
#include "vtkSlicer.h"
#include "OpenTracker.h"
#include "common/CallbackModule.h"


using namespace ot;


class VTK_SLICER_BASE_EXPORT vtkOpenTracker  : public vtkObject
{
public:
    static vtkOpenTracker *New();
    vtkTypeMacro(vtkOpenTracker,vtkObject);
    void PrintSelf(ostream& os, vtkIndent indent) {};
    vtkGetObjectMacro(LocatorMatrix,vtkMatrix4x4);
    vtkGetObjectMacro(LandmarkTransformMatrix,vtkMatrix4x4);

    vtkSetMacro(UseRegistration,int);
    vtkGetMacro(UseRegistration,int);

    vtkGetMacro(NumberOfPoints,int);

    void Init(char *configfile);
    void CloseConnection();
    void PollRealtime();
    static void callbackF(const Node&, const Event &event, void *data);

    // t1, t2, t3: target landmarks 
    // s1, s2, s3: source landmarks 
    void AddPoint(int id, float t1, float t2, float t3, float s1, float s2, float s3);
    int DoRegistration();

    void SetNumberOfPoints(int no);


protected:
    vtkOpenTracker();
    ~vtkOpenTracker();
    Context *context;

    int NumberOfPoints;
    int UseRegistration;
    vtkPoints *SourceLandmarks;
    vtkPoints *TargetLandmarks;
    vtkMatrix4x4 *LandmarkTransformMatrix;

    void quaternion2xyz(float* orientation, float *normal, float *transnormal); 
    vtkMatrix4x4 *LocatorMatrix;

    void ApplyTransform(float *position, float *norm, float *transnorm);
};

#endif


  

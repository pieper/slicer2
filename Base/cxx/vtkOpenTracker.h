/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkOpenTracker.h,v $
  Date:      $Date: 2007/11/20 14:37:17 $
  Version:   $Revision: 1.1.2.7 $

=========================================================================auto=*/

#ifndef __vtkOpenTracker_h
#define __vtkOpenTracker_h

#include "vtkPoints.h"
#include "vtkMatrix4x4.h"
#include "vtkSlicer.h"
#include "OpenTracker/OpenTracker.h"
#include "OpenTracker/common/CallbackModule.h"
#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkIterativeClosestPointTransform.h"


using namespace ot;


class VTK_SLICER_BASE_EXPORT vtkOpenTracker  : public vtkObject
{
public:
    static vtkOpenTracker *New();
    vtkTypeMacro(vtkOpenTracker,vtkObject);
    void PrintSelf(ostream& os, vtkIndent indent) {};

    vtkGetObjectMacro(LocatorMatrix,vtkMatrix4x4);
    vtkGetObjectMacro(LandmarkTransformMatrix,vtkMatrix4x4);
    vtkPolyData * GetSourceModel(void);
    vtkPolyData * GetLandmarkSourceModel(void);
    


    vtkSetMacro(UseRegistration,int);
    vtkGetMacro(UseRegistration,int);

    vtkGetMacro(Button,short);

    vtkGetMacro(NumberOfPoints,int);

    vtkSetMacro(MultiRate,float);

    vtkSetMacro(SensorNO,int);

    void Init(char *configfile);
    void CloseConnection();
    void PollRealtime();

    // We currently support 4 sensors at the same time
    // using Aurora tracking device, for example.
    // NO 4 is used as reference.
    static void CallbackSensor1(const Node&, const Event &event, void *data);
    static void CallbackSensor2(const Node&, const Event &event, void *data);
    static void CallbackSensor3(const Node&, const Event &event, void *data);
    static void CallbackSensor4(const Node&, const Event &event, void *data);

    // t1, t2, t3: target landmarks 
    // s1, s2, s3: source landmarks 
    void AddPoint(int id, float t1, float t2, float t3, float s1, float s2, float s3);
    void AddTargetICPPoint(float t1, float t2, float t3);
    void AddSourceICPPoint(float s1, float s2, float s3);

    /*! Starts the Landmark Registration
      \note Number of points SetNumberOfPoints must be set before starting the Registration. At least 2 Source and Targetpoints must be set by AddPoint
      \return returns a integer. 0 if the registration is done. if it fails returning 1
     */
    int DoRegistration();
    /*! Starts the ICP Registration
      \note Parameters Must be set in SetICPParams and a target model must be given by SetTargetModel to start the registration.
      \return returns a integer. 0 if the registration is done. if it fails returning 1
     */
    int DoRegistrationICP(void);
    void SetNumberOfPoints(int no);

    /*! Set the target Model where the registration is depending on
     */
    void SetTargetModel(vtkDataSet *model);
    /*! Set the target Model where the registration is depending on
      \param sets the mean distance to rms
      \param sets 1 if mean distance will checked 0 if not
      \param sets the maximum mean distance
      \param sets the maximum iterations
     */
    void SetICPParams(int rms, int chkMean, double maxMean, int iter);

    void SetLocatorMatrix();



protected:
    vtkOpenTracker();
    ~vtkOpenTracker();
    Context *context;

    short Button;
    int NumberOfPoints;
    int UseRegistration;
    float MultiRate;
    vtkPoints *SourceLandmarks;
    vtkPoints *TargetLandmarks;
    vtkMatrix4x4 *LandmarkTransformMatrix;
    vtkMatrix4x4 *LocatorMatrix;
    
    vtkDataSet *targetDataSet;
    vtkPolyData *SourceModel;
    vtkPolyData *LandmarkSourceModel;
    vtkPoints *SourceICPPoints;
    vtkPoints *SourceICPLandmarkPoints;
    vtkPoints *TargetICPPoints;
    int UseICPRegistration;
    vtkCellArray *strips;
    vtkIterativeClosestPointTransform *ICPTransformation;
    vtkLandmarkTransform *LandmarkGlobalTransformation;

    int SensorNO;
    float Position[4][3];
    float Orientation[4][4];
    float ReferencePosition[3];


    // This is the list of special points. Each point holds these values:
    // x: the x value difference between the referece and one point from the skull or patient
    // y: the y value difference 
    // z: the z value difference
    vtkPoints *ReferenceDiff;
 

    // Internal member functions
    void Quaternion2xyz(float* orientation, float *normal, float *transnormal); 
    void ApplyTransform(float *position, float *norm, float *transnorm);
    void BuildSourceModel(void);
    void BuildLandmarkSourceModel(void);
    void UpdateRegistration();

};

#endif


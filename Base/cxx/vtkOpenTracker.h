/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkOpenTracker.h,v $
  Date:      $Date: 2007/12/02 05:53:12 $
  Version:   $Revision: 1.1.2.9 $

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
#include "PivotCalibration.h"


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

    vtkSetMacro(MultiRate,double);

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
    void AddPoint(int id, double t1, double t2, double t3, double s1, double s2, double s3);
    void AddTargetICPPoint(double t1, double t2, double t3);
    void AddSourceICPPoint(double s1, double s2, double s3);

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
    void DeleteRegistration();

    // Gets the position of the pivot point
    void GetPivotPosition(double pos[3]);
    // Gets the translation from the tracked marker to the pivot point
    void GetTranslation(double trans[3]);
    // Gets the root mean square error (RMSE)
    double GetRMSE();

    void CollectDataForPivotCalibration(int yes);
    void ComputePivotCalibration();
    void ApplyPivotCalibration(int yes);

protected:
    vtkOpenTracker();
    ~vtkOpenTracker();
    Context *context;

    short Button;
    int NumberOfPoints;
    int UseRegistration;
    double MultiRate;
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

    double PivotPosition[3];
    double Translation[3];
    double RMSE;
    bool CollectPCData;
    bool UsePivotCalibration;
    PivotCalibration *PVCalibration;

    int SensorNO;
    double Position[4][3];
    double Orientation[4][4];
    double ReferencePosition[3];


    // This is the list of special points. Each point holds these values:
    // x: the x value difference between the referece and one point from the skull or patient
    // y: the y value difference 
    // z: the z value difference
    vtkPoints *ReferenceDiff;
 

    // Internal member functions
    void Quaternion2xyz(double* orientation, double *normal, double *transnormal); 
    void ApplyTransform(double *position, double *norm, double *transnorm);
    void BuildSourceModel(void);
    void BuildLandmarkSourceModel(void);
    void UpdateRegistration();

};

#endif


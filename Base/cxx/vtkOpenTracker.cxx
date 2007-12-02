/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkOpenTracker.cxx,v $
  Date:      $Date: 2007/12/02 05:53:12 $
  Version:   $Revision: 1.1.2.9 $

  add Author: Christoph Ruetz
=========================================================================auto=*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memcpy
//#include <iostream.h>
#include <string.h>
#include <fcntl.h>
#ifndef _WIN32
#include <strings.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#include "vtkOpenTracker.h"
#include "vtkObjectFactory.h"
#include "vtkLandmarkTransform.h"
#include "vtkTransform.h"


#ifdef _WIN32
#pragma warning ( disable : 4786 )
#endif

using namespace ot;


vtkOpenTracker::vtkOpenTracker()
{
    this->LocatorMatrix = vtkMatrix4x4::New(); // Identity
    this->LandmarkTransformMatrix = vtkMatrix4x4::New(); // Identity

    this->SourceLandmarks = NULL; 
    this->TargetLandmarks = NULL; 

    this->UseICPRegistration = 0;
    this->NumberOfPoints = 0;
    this->MultiRate = 1.0;

    this->SourceICPPoints = NULL;
    this->SourceICPLandmarkPoints = NULL;
    this->TargetICPPoints = NULL;

    this->SensorNO = 1;
    this->ReferencePosition[0] = 0.0;
    this->ReferencePosition[1] = 0.0;
    this->ReferencePosition[2] = 0.0;

    this->ReferenceDiff = NULL;

    this->CollectPCData = 0;
    this->PVCalibration = new PivotCalibration;

    this->UseRegistration = 0;
    this->UsePivotCalibration = 0;

}



vtkOpenTracker* vtkOpenTracker::New()
{
    // First try to create the object from the vtkObjectFactory
    vtkObject* ret = vtkObjectFactory::CreateInstance("vtkOpenTracker");
    if(ret)
    {
        return (vtkOpenTracker*)ret;
    }
    // If the factory was unable to create the object, then create it here.
    return new vtkOpenTracker;
}



vtkOpenTracker::~vtkOpenTracker()
{
    this->LocatorMatrix->Delete();
    this->SourceLandmarks->Delete();
    this->TargetLandmarks->Delete();
    this->SourceICPPoints->Delete();
    this->SourceICPLandmarkPoints->Delete();
    this->TargetICPPoints->Delete();

    this->ReferenceDiff->Delete();
    if (this->PVCalibration)
    {
        delete this->PVCalibration;
    }

}



void vtkOpenTracker::SetNumberOfPoints(int no)
{
    if (this->TargetLandmarks)
    {
        this->TargetLandmarks->Delete();
    }
    this->TargetLandmarks = vtkPoints::New();
    this->TargetLandmarks->SetDataTypeToDouble();
    this->TargetLandmarks->SetNumberOfPoints(no);


    if (this->SourceLandmarks)
    {
        this->SourceLandmarks->Delete();
    }
    this->SourceLandmarks = vtkPoints::New();
    this->SourceLandmarks->SetDataTypeToDouble();
    this->SourceLandmarks->SetNumberOfPoints(no);


    if (this->ReferenceDiff)
    {
        this->ReferenceDiff->Delete();
    }
    this->ReferenceDiff = vtkPoints::New();
    this->ReferenceDiff->SetDataTypeToDouble();
    this->ReferenceDiff->SetNumberOfPoints(no);


    this->NumberOfPoints = no;
}



void vtkOpenTracker::Init(char *configfile)
{
    fprintf(stderr,"config file: %s\n",configfile);
    this->context=new Context(1); 
    // get callback module from the context
    CallbackModule * callbackMod = (CallbackModule *)context->getModule("CallbackConfig");

    context->parseConfiguration(configfile);  // parse the configuration file

    // sets the callback function
    callbackMod->setCallback( "cb1", (OTCallbackFunction*)&CallbackSensor1, this); 
    callbackMod->setCallback( "cb2", (OTCallbackFunction*)&CallbackSensor2, this);
    callbackMod->setCallback( "cb3", (OTCallbackFunction*)&CallbackSensor3, this);
    callbackMod->setCallback( "cb4", (OTCallbackFunction*)&CallbackSensor4, this);
 
    context->start();

}



void vtkOpenTracker::CloseConnection()
{
    context->close();
}



void vtkOpenTracker::Quaternion2xyz(double* orientation, double *normal, double *transnormal) 
{
    double q0, qx, qy, qz;

    q0 = orientation[3];
    qx = orientation[0];
    qy = orientation[1];
    qz = orientation[2]; 

    transnormal[0] = 1-2*qy*qy-2*qz*qz;
    transnormal[1] = 2*qx*qy+2*qz*q0;
    transnormal[2] = 2*qx*qz-2*qy*q0;

    normal[0] = 2*qx*qz+2*qy*q0;
    normal[1] = 2*qy*qz-2*qx*q0;
    normal[2] = 1-2*qx*qx-2*qy*qy;
}



void vtkOpenTracker::PollRealtime()
{
    context->pushEvents();       // push event and
    context->pullEvents();       // pull event 
    context->stop();
}



void vtkOpenTracker::SetLocatorMatrix()
{
    double norm[3];
    double transnorm[3];
    int j;
    int i = this->SensorNO - 1;


    // collect data for pivot calibration
    if (this->CollectPCData)
    {
        this->PVCalibration->AddSample(this->Orientation[i], this->Position[i]);
    }

    Quaternion2xyz(this->Orientation[i], norm, transnorm);


    // apply pivot calibration
    if (this->UsePivotCalibration)
    {
        for (int ii = 0; ii < 3; ii++)
        {
            this->Position[i][ii] += this->Translation[ii];
        }
    }


    // Apply the transform matrix 
    // to the postion, norm and transnorm
    if (this->UseRegistration || this->UseICPRegistration)
    {
        ApplyTransform(this->Position[i], norm, transnorm);
    }

    for (j = 0; j < 3; j++) 
    {
        this->LocatorMatrix->SetElement(j,0,this->Position[i][j]);
        this->LocatorMatrix->SetElement(j,1,norm[j]);
        this->LocatorMatrix->SetElement(j,2,transnorm[j]);
        this->LocatorMatrix->SetElement(j,3,0);
        this->LocatorMatrix->SetElement(3,j,0);
    }

    this->LocatorMatrix->SetElement(3,3,1);
}



void vtkOpenTracker::CallbackSensor1(const Node&, const Event &event, void *data)
{
    vtkOpenTracker *VOT=(vtkOpenTracker *)data;

    // the original values are in the unit of meters
    VOT->Position[0][0]=(double)(event.getPosition())[0] * VOT->MultiRate; 
    VOT->Position[0][1]=(double)(event.getPosition())[1] * VOT->MultiRate;
    VOT->Position[0][2]=(double)(event.getPosition())[2] * VOT->MultiRate;

    VOT->Orientation[0][0]=(double)(event.getOrientation())[0];
    VOT->Orientation[0][1]=(double)(event.getOrientation())[1];
    VOT->Orientation[0][2]=(double)(event.getOrientation())[2];
    VOT->Orientation[0][3]=(double)(event.getOrientation())[3];

    VOT->Button = (short)(event.getButton());
    VOT->SetLocatorMatrix();
}



void vtkOpenTracker::CallbackSensor2(const Node&, const Event &event, void *data)
{
    vtkOpenTracker *VOT=(vtkOpenTracker *)data;

    // the original values are in the unit of meters
    VOT->Position[1][0]=(double)(event.getPosition())[0] * VOT->MultiRate; 
    VOT->Position[1][1]=(double)(event.getPosition())[1] * VOT->MultiRate;
    VOT->Position[1][2]=(double)(event.getPosition())[2] * VOT->MultiRate;

    VOT->Orientation[1][0]=(double)(event.getOrientation())[0];
    VOT->Orientation[1][1]=(double)(event.getOrientation())[1];
    VOT->Orientation[1][2]=(double)(event.getOrientation())[2];
    VOT->Orientation[1][3]=(double)(event.getOrientation())[3];

    VOT->Button = (short)(event.getButton());
    VOT->SetLocatorMatrix();
}



void vtkOpenTracker::CallbackSensor3(const Node&, const Event &event, void *data)
{
    vtkOpenTracker *VOT=(vtkOpenTracker *)data;

    // the original values are in the unit of meters
    VOT->Position[2][0]=(double)(event.getPosition())[0] * VOT->MultiRate; 
    VOT->Position[2][1]=(double)(event.getPosition())[1] * VOT->MultiRate;
    VOT->Position[2][2]=(double)(event.getPosition())[2] * VOT->MultiRate;

    VOT->Orientation[2][0]=(double)(event.getOrientation())[0];
    VOT->Orientation[2][1]=(double)(event.getOrientation())[1];
    VOT->Orientation[2][2]=(double)(event.getOrientation())[2];
    VOT->Orientation[2][3]=(double)(event.getOrientation())[3];

    VOT->Button = (short)(event.getButton());
    VOT->SetLocatorMatrix();
}



void vtkOpenTracker::CallbackSensor4(const Node&, const Event &event, void *data)
{
    vtkOpenTracker *VOT=(vtkOpenTracker *)data;


    double pos[3];
    pos[0] = (double)(event.getPosition())[0] * VOT->MultiRate; 
    pos[1] = (double)(event.getPosition())[1] * VOT->MultiRate;
    pos[2] = (double)(event.getPosition())[2] * VOT->MultiRate;

    double d1 = fabs(VOT->ReferencePosition[0] - pos[0]);
    double d2 = fabs(VOT->ReferencePosition[1] - pos[1]);
    double d3 = fabs(VOT->ReferencePosition[2] - pos[2]);
    
    // the original values are in the unit of meters
    VOT->Position[3][0]= pos[0];
    VOT->Position[3][1]= pos[1];
    VOT->Position[3][2]= pos[2];

    // Sensor 4 is the reference probe
    VOT->ReferencePosition[0] = pos[0];
    VOT->ReferencePosition[1] = pos[1];
    VOT->ReferencePosition[2] = pos[2];

    VOT->Orientation[3][0] = (double)(event.getOrientation())[0];
    VOT->Orientation[3][1] = (double)(event.getOrientation())[1];
    VOT->Orientation[3][2] = (double)(event.getOrientation())[2];
    VOT->Orientation[3][3] = (double)(event.getOrientation())[3];

    // Need redo registration due to reference change
    if (VOT->SourceLandmarks != NULL && 
        (d1 >= 1 || d2 >= 1 || d3 >= 1))
    { 
        VOT->UpdateRegistration();
    }

    VOT->Button = (short)(event.getButton());
    VOT->SetLocatorMatrix();
}



void vtkOpenTracker::DeleteRegistration()
{
    if (this->TargetLandmarks)
    {
        this->TargetLandmarks->Delete();
    }

    if (this->SourceLandmarks)
    {
        this->SourceLandmarks->Delete();
    }

    if (this->ReferenceDiff)
    {
        this->ReferenceDiff->Delete();
    }

    this->NumberOfPoints = 0;
    this->UseRegistration = 0;

}



void vtkOpenTracker::UpdateRegistration()
{
    // Update the source points in patient space
    int nps = this->SourceLandmarks->GetNumberOfPoints();
    this->SourceLandmarks->Delete();

    for (int i = 0; i < nps; i++)
    {
        double s[3];
        this->ReferenceDiff->GetPoint(i, s);
        double x = s[0] + this->ReferencePosition[0];
        double y = s[1] + this->ReferencePosition[1];
        double z = s[2] + this->ReferencePosition[2];
        
        this->SourceLandmarks->InsertPoint(i, x, y, z);
        this->SourceLandmarks->Modified();
    }

    // Call DoRegistration
    DoRegistration();
}



void vtkOpenTracker::ApplyTransform(double *position, double *norm, double *transnorm)
{
    // Transform position, norm and transnorm
    // ---------------------------------------------------------
    double p[4];
    double n[4];
    double tn[4];

    for (int i = 0; i < 3; i++)
    {
        p[i] = position[i];
        n[i] = norm[i];
        tn[i] = transnorm[i];
    }
    p[3] = 1;     // translation affects a poistion
    n[3] = 0;     // translation doesn't affect an orientation
    tn[3] = 0;    // translation doesn't affect an orientation

    this->LandmarkTransformMatrix->MultiplyPoint(p, p);    // transform a position
    this->LandmarkTransformMatrix->MultiplyPoint(n, n);    // transform an orientation
    this->LandmarkTransformMatrix->MultiplyPoint(tn, tn);  // transform an orientation

    for (int i = 0; i < 3; i++)
    {
        position[i] = p[i];
        norm[i] = n[i];
        transnorm[i] = tn[i];
    }
}



void vtkOpenTracker::AddPoint(int id, double t1, double t2, double t3, double s1, double s2, double s3)
{
    // slicer space
    this->TargetLandmarks->InsertPoint(id, t1, t2, t3);
    this->TargetLandmarks->Modified();

    // patient space
    this->SourceLandmarks->InsertPoint(id, s1, s2, s3);
    this->SourceLandmarks->Modified();

    // save the difference for later registration 
    double dx = s1 - this->ReferencePosition[0];
    double dy = s2 - this->ReferencePosition[1];
    double dz = s3 - this->ReferencePosition[2];
    this->ReferenceDiff->InsertPoint(id, dx, dy, dz);
    this->ReferenceDiff->Modified();
}



int vtkOpenTracker::DoRegistration()
{

    if (this->TargetLandmarks == NULL || this->SourceLandmarks == NULL)
    {
        vtkErrorMacro(<< "vtkOpenTracker::DoRegistration(): Got NULL pointer.");
        return 1;
    }

    int tnp = this->TargetLandmarks->GetNumberOfPoints();
    int snp = this->SourceLandmarks->GetNumberOfPoints();
    if (tnp < 2 || snp < 2)
    {
        vtkErrorMacro(<< "vtkOpenTracker::DoRegistration(): Number of points is less than 2 in either TargetLandmarks or SourceLandmarks.");
        return 1;
    }

    if (tnp != snp)
    {
        vtkErrorMacro(<< "vtkOpenTracker::DoRegistration(): TargetLandmarks != SourceLandmarks in terms of number of points.");
        return 1;
    }

    vtkLandmarkTransform *landmark = vtkLandmarkTransform::New();
    this->LandmarkGlobalTransformation = vtkLandmarkTransform::New();
    landmark->SetTargetLandmarks(this->TargetLandmarks);
    landmark->SetSourceLandmarks(this->SourceLandmarks);
    landmark->SetModeToRigidBody();
    landmark->Update();
    this->LandmarkTransformMatrix->DeepCopy(landmark->GetMatrix());
    this->LandmarkGlobalTransformation->DeepCopy(landmark);
    landmark->Delete();

    this->UseRegistration = 1;

    return 0; 
}

//VTK needs a Models to perform the registration, 
//thus building a source model from the received points
void vtkOpenTracker::BuildSourceModel(void)
{
   int noCells = this->SourceICPPoints->GetNumberOfPoints();
   this->strips = vtkCellArray::New();
   this->SourceModel = vtkPolyData::New();

   this->strips->InsertNextCell(noCells);
   
   for (int i = 0; i < noCells; i++)
   {
       this->strips->InsertCellPoint(i);
   }

   this->SourceModel->SetPoints(this->SourceICPPoints);
   this->SourceModel->SetLines(this->strips);
}


//VTK needs a Models to perform the registration, 
//thus building a source model from the received points
void vtkOpenTracker::BuildLandmarkSourceModel(void)
{
   int noCells = this->SourceICPLandmarkPoints->GetNumberOfPoints();
   this->strips = vtkCellArray::New();
   this->LandmarkSourceModel = vtkPolyData::New();

   this->strips->InsertNextCell(noCells);
   
   for (int i = 0; i < noCells; i++)
   {
       this->strips->InsertCellPoint(i);
   }

   this->LandmarkSourceModel->SetPoints(this->SourceICPLandmarkPoints);
   this->LandmarkSourceModel->SetLines(this->strips);
}

//Add a Source ICP Registration point
void vtkOpenTracker::AddSourceICPPoint(double s1, double s2, double s3)
{

   
   this->SourceICPPoints->InsertNextPoint(s1, s2, s3);
   this->SourceICPPoints->Modified();
   this->SourceICPLandmarkPoints->InsertNextPoint(this->LandmarkGlobalTransformation->TransformDoublePoint(s1, s2, s3));
   this->SourceICPLandmarkPoints->Modified();
fprintf(stderr,"Insert Point(%d) %f, %f, %f\n",this->SourceICPPoints->GetNumberOfPoints(),s1,s2,s3);
 
}

//Add a Target ICP Registration point
void vtkOpenTracker::AddTargetICPPoint(double t1, double t2, double t3)
{
    this->TargetICPPoints->InsertNextPoint(t1, t2, t3);
    this->TargetICPPoints->Modified();

}
//Target where the registration depends on, It must be a model
void vtkOpenTracker::SetTargetModel(vtkDataSet *model)
{
    this->targetDataSet = model;
}

//returns the source model
vtkPolyData * vtkOpenTracker::GetSourceModel(void)
{
BuildSourceModel();

return this->SourceModel;
}

//returns the source model
vtkPolyData * vtkOpenTracker::GetLandmarkSourceModel(void)
{
BuildLandmarkSourceModel();

return this->LandmarkSourceModel;
}

//Build the initial point set
void vtkOpenTracker::SetICPParams(int rms, int chkMean, double maxMean, int iter)
{

    this->ICPTransformation = vtkIterativeClosestPointTransform::New();
    
    if (this->TargetICPPoints)
    {
        this->TargetICPPoints->Delete();
    }
    this->TargetICPPoints = vtkPoints::New();
    this->TargetICPPoints->SetDataTypeToDouble();
    
   
    if (this->SourceICPPoints)
    {
        this->SourceICPPoints->Delete();
    }
    
    this->SourceICPPoints = vtkPoints::New();
    this->SourceICPPoints->SetDataTypeToDouble();
    if (this->SourceICPLandmarkPoints)
    {
        this->SourceICPLandmarkPoints->Delete();
    }
    
    this->SourceICPLandmarkPoints = vtkPoints::New();
    this->SourceICPLandmarkPoints->SetDataTypeToDouble();

    if (rms == 1)
       ICPTransformation->SetMeanDistanceModeToRMS();

    ICPTransformation->SetCheckMeanDistance(chkMean);
    ICPTransformation->SetMaximumMeanDistance(maxMean);
    ICPTransformation->SetMaximumNumberOfIterations(iter);

}



//Registration of ICP
int vtkOpenTracker::DoRegistrationICP(void)
{
  fprintf(stderr,"start ICP \n");
  BuildSourceModel();
  BuildLandmarkSourceModel();
  fprintf(stderr,"built model \n");
  if (this->SourceModel == NULL || this->targetDataSet == NULL)
  {
       vtkErrorMacro(<< "vtkOpenTracker::DoRegistrationICP(): Got NULL pointer. No Models are created");
       return 1;
  }
  
  fprintf(stderr,"Set source \n");
  this->ICPTransformation->SetSource(this->LandmarkSourceModel);
  fprintf(stderr,"set Target \n");
  this->ICPTransformation->SetTarget(this->targetDataSet);
  //this->ICPTransformation->StartByMatchingCentroidsOn();

  fprintf(stderr,"set rigid body \n");
  this->ICPTransformation->GetLandmarkTransform()->SetModeToRigidBody();
  //this->ICPTransformation->GetLandmarkTransform()->SetTargetLandmarks(this->TargetLandmarks);
  //this->ICPTransformation->GetLandmarkTransform()->SetSourceLandmarks(this->SourceLandmarks);
  //this->ICPTransformation->GetLandmarkTransform()->Update();
  fprintf(stderr,"set max number of landmarks: %i\n",this->SourceICPPoints->GetNumberOfPoints());
  this->ICPTransformation->SetMaximumNumberOfLandmarks(this->SourceICPPoints->GetNumberOfPoints());
  fprintf(stderr,"update \n");
  this->ICPTransformation->Update();
  fprintf(stderr,"matrix deep copy \n");
  //fill the Tranformation matrix of this class with context. 
  //Using this Matrix in slicer will perform the necessary transformation
 
  //temptransform->RotateZ(90);
  //temptransform->RotateY(90);
  vtkTransform *transform = vtkTransform::New();
 
  //transform->DeepCopy(this->ICPTransformation);
  //vtkTransform *transform2 = vtkTransform::New();
  //transform2->DeepCopy(this->ICPTransformation);
  transform->Concatenate(this->ICPTransformation);
  transform->Concatenate(this->LandmarkGlobalTransformation);
  
  transform->Modified();
  transform->Update();
//   vtkMatrix4x4 *b = vtkMatrix4x4::New();
//   vtkMatrix4x4 *c = vtkMatrix4x4::New();
//   b->Multiply4x4(this->ICPTransformation->GetMatrix(),this->LandmarkGlobalTransformation->GetMatrix(), c);
  this->LandmarkTransformMatrix->DeepCopy(transform->GetMatrix());
//   this->LandmarkTransformMatrix->DeepCopy(c);
  //this->LandmarkTransformMatrix->Invert();
  ICPTransformation->Delete();
 
  this->UseICPRegistration = 1;
  fprintf(stderr,"ICP done\n");

  return 0; 
}



void vtkOpenTracker::GetPivotPosition(double pos[3])
{
    pos[0] = this->PivotPosition[0];
    pos[1] = this->PivotPosition[1];
    pos[2] = this->PivotPosition[2];
}



void vtkOpenTracker::GetTranslation(double trans[3])
{
    trans[0] = this->Translation[0];
    trans[1] = this->Translation[1];
    trans[2] = this->Translation[2];
}



double vtkOpenTracker::GetRMSE()
{
    return this->RMSE;
}



void vtkOpenTracker::CollectDataForPivotCalibration(int yes)
{
    this->CollectPCData = yes;
    if (yes) // start
    {
        if (this->PVCalibration)
        {
            delete this->PVCalibration;
            this->PVCalibration = new PivotCalibration;
        }
    }
}



void vtkOpenTracker::ComputePivotCalibration()
{
    this->PVCalibration->CalculateCalibration();
    this->PVCalibration->GetPivotPosition(this->PivotPosition);
    this->PVCalibration->GetTranslation(this->Translation);
    this->RMSE = this->PVCalibration->GetRMSE();
}



void vtkOpenTracker::ApplyPivotCalibration(int yes)
{
    this->UsePivotCalibration = yes;

}



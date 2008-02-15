/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkOpenTracker.cxx,v $
  Date:      $Date: 2008/02/15 23:16:55 $
  Version:   $Revision: 1.1.2.14 $

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
#include "vtkMath.h"


#ifdef _WIN32
#pragma warning ( disable : 4786 )
#endif

using namespace ot;


vtkOpenTracker::vtkOpenTracker()
{
    this->LocatorMatrix = vtkMatrix4x4::New(); // Identity
    this->LandmarkTransformMatrix = vtkMatrix4x4::New(); // Identity
    this->ICPTransformMatrix = vtkMatrix4x4::New(); // Identity
    this->ReferenceMatrix1 = vtkMatrix4x4::New(); // Identity
    this->ReferenceMatrix2 = vtkMatrix4x4::New(); // Identity
 

    this->SourceLandmarks = NULL; 
    this->TargetLandmarks = NULL; 

    this->UseICPRegistration = 0;
 
    this->NumberOfPoints = 0;
    this->MultiRate = 1.0;

    this->SourceICPPoints = NULL;
    this->SourceICPLandmarkPoints = NULL;
    this->TargetICPPoints = NULL;

    this->RegistrationDone = 0;
    this->ICPRegistrationDone = 0;
    this->ICPPoints = NULL;
    this->ICPTargetModelPoints = NULL;
    this->CollectICPData = 0;
    this->ICPReg = NULL; 
    this->ICPPointID = 0;
 

    this->SensorNO = 1;
    this->CollectPCData = 0;
    //this->PVCalibration = NULL; 
 

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
    this->LandmarkTransformMatrix->Delete();
    this->ICPTransformMatrix->Delete();
 
    this->SourceLandmarks->Delete();
    this->TargetLandmarks->Delete();
    this->SourceICPPoints->Delete();
    this->SourceICPLandmarkPoints->Delete();
    this->TargetICPPoints->Delete();

    this->ReferenceMatrix1->Delete();
    this->ReferenceMatrix2->Delete();

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



void vtkOpenTracker::SetLocatorMatrix(int sensorNO)
{
    double norm[3];
    double transnorm[3];
    double pos[3];
    int j;

    if (sensorNO != this->SensorNO)
    {
        return;
    }


    int i = this->SensorNO - 1;
    Quaternion2xyz(this->Orientation[i], norm, transnorm);


    // apply pivot calibration
    for (int ii = 0; ii < 3; ii++)
    {
        pos[ii] = this->Position[i][ii] + this->Translation[ii];
    }

    // If not reference sensor
    if (sensorNO != 4)
    {
        // collect data for pivot calibration
        if (this->CollectPCData)
        {
            /*
            cout << "Ori: " << this->Orientation[i][0] <<  "   " << 
                               this->Orientation[i][1] <<  "   " <<  
                               this->Orientation[i][2] <<  "   " << 
                               this->Orientation[i][3] << endl; 

            cout << "Pos: " << this->Position[i][0] <<  "   "  << 
                               this->Position[i][1] <<  "   "  << 
                               this->Position[i][2] << endl; 
                               */

            this->PVCalibration.AddSample(this->Orientation[i], this->Position[i]);
        }

        // collect data for icp registration 
        if (this->CollectICPData)
        {
            this->ICPPoints->InsertPoint(this->ICPPointID, pos);
            this->ICPPointID++;
        }


        // Apply the transform matrix 
        // to the postion, norm and transnorm
        if (this->UseRegistration || this->UseICPRegistration)
        {
            ApplyTransform(pos, norm, transnorm);
        }
    }


    for (j = 0; j < 3; j++) 
    {
        this->LocatorMatrix->SetElement(j,0,pos[j]);
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
    VOT->SetLocatorMatrix(1);

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
    VOT->SetLocatorMatrix(2);

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
    VOT->SetLocatorMatrix(3);
}



void vtkOpenTracker::CallbackSensor4(const Node&, const Event &event, void *data)
{
    vtkOpenTracker *VOT=(vtkOpenTracker *)data;


    // Position 
    double pos[3];

    for (int i = 0; i < 3; i++)
    { 
        pos[i] = (double)(event.getPosition())[i] * VOT->MultiRate; 
        VOT->Position[3][i]= pos[i];
    }

    // Orientation
    for (int i = 0; i < 4; i++)
    {
        VOT->Orientation[3][i] = (double)(event.getOrientation())[i];
    }

    // Sensor 4 is the reference probe
    VOT->UpdateReferenceMatrices();

    /*
    cout << "matrix 2" << endl;
    VOT->ReferenceMatrix2->PrintSelf(std::cout, 5);
    cout << "matrix 1" << endl;
    VOT->ReferenceMatrix1->PrintSelf(std::cout, 5);
    */
 
    VOT->Button = (short)(event.getButton());
    VOT->SetLocatorMatrix(4);
}



void vtkOpenTracker::UpdateReferenceMatrices()
{
    double normal[3];
    double tnormal[3];
    double cross[3];

    Quaternion2xyz(this->Orientation[3], normal, tnormal); 
    vtkMath::Cross(normal, tnormal, cross);
    vtkMath::Normalize(normal);
    vtkMath::Normalize(tnormal);
    vtkMath::Normalize(cross);

    if (!RegistrationDone)
    {
        for (int j = 0; j < 3; j++) 
        {
            this->ReferenceMatrix1->SetElement(j,0,normal[j]);
            this->ReferenceMatrix1->SetElement(j,1,tnormal[j]);
            this->ReferenceMatrix1->SetElement(j,2,cross[j]);
            this->ReferenceMatrix1->SetElement(j,3,this->Position[3][j]);
        }
    }
    else
    {
        for (int j = 0; j < 3; j++) 
        {
            this->ReferenceMatrix2->SetElement(j,0,normal[j]);
            this->ReferenceMatrix2->SetElement(j,1,tnormal[j]);
            this->ReferenceMatrix2->SetElement(j,2,cross[j]);
            this->ReferenceMatrix2->SetElement(j,3,this->Position[3][j]);
        }

    }
}



void vtkOpenTracker::DeleteICPRegistration()
{
    // this->ICPPoints->Delete();
    this->UseICPRegistration = 0;
    this->ICPRegistrationDone = 0;
}



void vtkOpenTracker::DeleteRegistration()
{
    if (this->TargetLandmarks)
    {
        this->TargetLandmarks->Delete();
    }
    this->TargetLandmarks = NULL;


    if (this->SourceLandmarks)
    {
        this->SourceLandmarks->Delete();
    }
    this->SourceLandmarks = NULL;


    this->NumberOfPoints = 0;
    this->UseRegistration = 0;
    this->RegistrationDone = 0;

}



void vtkOpenTracker::ApplyTransform(double *position, double *norm, double *transnorm)
{
    // Transform position, norm and transnorm
    // ---------------------------------------------------------
    double p[4];
    double n[4];
    double tn[4];
    vtkMatrix4x4 *invert = vtkMatrix4x4::New(); // Identity
 
    for (int i = 0; i < 3; i++)
    {
        p[i] = position[i];
        n[i] = norm[i];
        tn[i] = transnorm[i];
    }
    p[3] = 1;     // translation affects a poistion
    n[3] = 0;     // translation doesn't affect an orientation
    tn[3] = 0;    // translation doesn't affect an orientation


    // transform from the current location of the reference sensor
    // to the global origin (of the field generator in terms of 
    // Aurora tracking system)
    vtkMatrix4x4::Invert(this->ReferenceMatrix2, invert);
    invert->MultiplyPoint(p, p);    // transform a position
    invert->MultiplyPoint(n, n);    // transform an orientation
    invert->MultiplyPoint(tn, tn);  // transform an orientation
    invert->Delete();

    // transform from the global origin to the original location 
    // of the reference sensor 
    this->ReferenceMatrix1->MultiplyPoint(p, p);    // transform a position
    this->ReferenceMatrix1->MultiplyPoint(n, n);    // transform an orientation
    this->ReferenceMatrix1->MultiplyPoint(tn, tn);  // transform an orientation

    // Additional transform due to patient to slicer registration
    if (this->UseRegistration)
    { 
        this->LandmarkTransformMatrix->MultiplyPoint(p, p);    // transform a position
        this->LandmarkTransformMatrix->MultiplyPoint(n, n);    // transform an orientation
        this->LandmarkTransformMatrix->MultiplyPoint(tn, tn);  // transform an orientation
    }

    // ICP registration 
    if (this->UseICPRegistration)
    { 
        this->ICPTransformMatrix->MultiplyPoint(p, p);    // transform a position
        this->ICPTransformMatrix->MultiplyPoint(n, n);    // transform an orientation
        this->ICPTransformMatrix->MultiplyPoint(tn, tn);  // transform an orientation
    }


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
    this->RegistrationDone = 1;


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



int vtkOpenTracker::DoICPRegistration()
{
 
    if (this->ICPPoints == NULL || this->ICPTargetModelPoints == NULL)
    {
        vtkErrorMacro(<< "vtkOpenTracker::DoICPRegistration(): Got NULL pointer.");
        return 1;
    }
    if (this->ICPPoints->GetNumberOfPoints() == 0)
    {
        vtkErrorMacro(<< "vtkOpenTracker::DoICPRegistration(): Got empty source pointer.");
        return 1;
    }
    if (this->ICPTargetModelPoints->GetNumberOfPoints() == 0)
    {
        vtkErrorMacro(<< "vtkOpenTracker::DoICPRegistration(): Got empty target model points.");
        return 1;
    }


    if (this->ICPReg)
    {
        delete this->ICPReg;
        this->ICPReg = NULL;
    }

    this->ICPReg = new ICPRegistration;
    this->ICPReg->UseVTK();



    // this->ICPTargetModelPoints holds all points from a loaded model
    // in Slicer. A human head skull has 660,000 points. It takes a long
    // time to compute ICP matrix. The lines of code below are trying
    // to reduce the size of this->ICPTargetModelPoints.
    vtkPoints *reducedModelPoints = vtkPoints::New();
    int id = 0;
    double coord[3];
    for (int i = 0; i < this->ICPTargetModelPoints->GetNumberOfPoints(); i += 100)
    {
        this->ICPTargetModelPoints->GetPoint(i,coord);
        reducedModelPoints->InsertPoint(id, coord);
        id++;
    }

    // ICP computing
    this->ICPTransformMatrix->DeepCopy(
        this->ICPReg->RegisterWithICP(reducedModelPoints, this->ICPPoints));

    this->UseICPRegistration = 1;
    this->ICPRegistrationDone = 1;
    reducedModelPoints->Delete();

    return 0; 

}


 
void vtkOpenTracker::CollectDataForICP(int yes)
{
    this->CollectICPData = yes;

    if (!yes)
    {
        this->ICPPointID = 0;
    }
    else
    {
        if (this->ICPPoints)
        {
            this->ICPPoints->Delete();
        }
        this->ICPPoints = vtkPoints::New();
    }
}



void vtkOpenTracker::CollectDataForPivotCalibration(int yes)
{
    this->CollectPCData = yes;
    if (yes) // start
    {
        this->PVCalibration.Clear();
    }
}



void vtkOpenTracker::ComputePivotCalibration()
{
    this->PVCalibration.CalculateCalibration();
    this->PVCalibration.GetPivotPosition(this->PivotPosition);
    this->PVCalibration.GetTranslation(this->Translation);
    this->RMSE = this->PVCalibration.GetRMSE();
}



void vtkOpenTracker::ApplyPivotCalibration(int yes)
{
    this->UsePivotCalibration = yes;

}



/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkOpenTracker.cxx,v $
  Date:      $Date: 2007/05/08 18:29:44 $
  Version:   $Revision: 1.1.2.4 $

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

    this->UseRegistration = 0;
    this->NumberOfPoints = 0;
    this->MultiRate = 1.0;
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
}



void vtkOpenTracker::SetNumberOfPoints(int no)
{
    if (this->TargetLandmarks)
    {
        this->TargetLandmarks->Delete();
    }
    this->TargetLandmarks = vtkPoints::New();
    this->TargetLandmarks->SetDataTypeToFloat();
    this->TargetLandmarks->SetNumberOfPoints(no);


    if (this->SourceLandmarks)
    {
        this->SourceLandmarks->Delete();
    }
    this->SourceLandmarks = vtkPoints::New();
    this->SourceLandmarks->SetDataTypeToFloat();
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

    callbackMod->setCallback( "cb1", (OTCallbackFunction*)&callbackF ,this);    // sets the callback function


    context->start();

}



void vtkOpenTracker::CloseConnection()
{
    context->close();
}



void vtkOpenTracker::quaternion2xyz(float* orientation, float *normal, float *transnormal) 
{
    float q0, qx, qy, qz;

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



void vtkOpenTracker::callbackF(const Node&, const Event &event, void *data)
{
    float position[3];
    float orientation[4];
    float norm[3];
    float transnorm[3];
    int j;

    vtkOpenTracker *VOT=(vtkOpenTracker *)data;

    // the original values are in the unit of meters
    position[0]=(float)(event.getPosition())[0] * VOT->MultiRate; 
    position[1]=(float)(event.getPosition())[1] * VOT->MultiRate;
    position[2]=(float)(event.getPosition())[2] * VOT->MultiRate;

    orientation[0]=(float)(event.getOrientation())[0];
    orientation[1]=(float)(event.getOrientation())[1];
    orientation[2]=(float)(event.getOrientation())[2];
    orientation[3]=(float)(event.getOrientation())[3];

    VOT->Button = (short)(event.getButton());

    VOT->quaternion2xyz(orientation, norm, transnorm);


    // Apply the transform matrix 
    // to the postion, norm and transnorm
    if (VOT->UseRegistration)
        VOT->ApplyTransform(position, norm, transnorm);

    for (j=0; j<3; j++) {
        VOT->LocatorMatrix->SetElement(j,0,position[j]);
    }


    for (j=0; j<3; j++) {
        VOT->LocatorMatrix->SetElement(j,1,norm[j]);
    }

    for (j=0; j<3; j++) {
        VOT->LocatorMatrix->SetElement(j,2,transnorm[j]);
    }

    for (j=0; j<3; j++) {
        VOT->LocatorMatrix->SetElement(j,3,0);
    }

    for (j=0; j<3; j++) {
        VOT->LocatorMatrix->SetElement(3,j,0);
    }

    VOT->LocatorMatrix->SetElement(3,3,1);
}



void vtkOpenTracker::ApplyTransform(float *position, float *norm, float *transnorm)
{
    // Transform position, norm and transnorm
    // ---------------------------------------------------------
    float p[4];
    float n[4];
    float tn[4];

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



void vtkOpenTracker::AddPoint(int id, float t1, float t2, float t3, float s1, float s2, float s3)
{
    this->TargetLandmarks->InsertPoint(id, t1, t2, t3);
    this->TargetLandmarks->Modified();

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
    landmark->SetTargetLandmarks(this->TargetLandmarks);
    landmark->SetSourceLandmarks(this->SourceLandmarks);
    landmark->SetModeToRigidBody();
    landmark->Update();
    this->LandmarkTransformMatrix->DeepCopy(landmark->GetMatrix());

    landmark->Delete();

    this->UseRegistration = 1;

    return 0; 
}

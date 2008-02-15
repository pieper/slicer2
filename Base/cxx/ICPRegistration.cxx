/*=auto=========================================================================

  Portions (c) Copyright 2007 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: ICPRegistration.cxx,v $
Date:      $Date: 2008/02/15 23:19:51 $
Version:   $Revision: 1.1.2.1 $
=========================================================================auto=*/

#include <ICPRegistration.h>

#include <vtkMatrix4x4.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkLandmarkTransform.h>
#include <vtkPolyData.h>
#include <vtkMaskPoints.h>
#include <itkEuler3DTransform.h>
#include <itkEuclideanDistancePointMetric.h>
#include <itkLevenbergMarquardtOptimizer.h>
#include <itkPointSet.h>
#include <itkPointSetToPointSetRegistrationMethod.h>

#include <vtkPoints.h>

ICPRegistration::ICPRegistration() : useVTK(false)
{
}


ICPRegistration::~ICPRegistration()
{
}


void ICPRegistration::UseVTK()
{
    this->useVTK=true;
}


void ICPRegistration::UseITK()
{
    this->useVTK=false;
}


vtkMatrix4x4* ICPRegistration::RegisterWithICP(vtkPoints *fixedPoints, vtkPoints *movingPoints)
{
    if (this->useVTK)
        return this->RegisterWithVTKICP(fixedPoints,movingPoints);
    else
        return this->RegisterWithITKICP(fixedPoints,movingPoints);
}


vtkMatrix4x4* ICPRegistration::RegisterWithITKICP(vtkPoints *fixedPoints, vtkPoints *movingPoints)
    // Does not work with only a few fiducials, not tried with many fiducials
{
    const unsigned int Dimension = 3;

    typedef itk::PointSet< float, Dimension >   PointSetType;

    PointSetType::Pointer fixedPointSet  = PointSetType::New();
    PointSetType::Pointer movingPointSet = PointSetType::New();

    typedef PointSetType::PointType     PointType;

    typedef PointSetType::PointsContainer  PointsContainer;

    PointsContainer::Pointer fixedPointContainer  = PointsContainer::New();
    PointsContainer::Pointer movingPointContainer = PointsContainer::New();

    PointType fixedPoint;
    PointType movingPoint;


    unsigned int pointId = 0;
    for (int i=0;i<fixedPoints->GetNumberOfPoints();i++)
    {
        double coord[3];
        fixedPoints->GetPoint(i, coord);
        fixedPoint[0]=coord[0];
        fixedPoint[1]=coord[1];
        fixedPoint[2]=coord[2];
        fixedPointContainer->InsertElement( pointId, fixedPoint );
        pointId++;
    }
    fixedPointSet->SetPoints( fixedPointContainer );
    std::cout <<     "Number of fixed Points = " << fixedPointSet->GetNumberOfPoints() << std::endl;


    pointId = 0;
    for (int i=0;i<movingPoints->GetNumberOfPoints();i++)
    {
        double coord[3];
        movingPoints->GetPoint(i,coord);
        movingPoint[0]=coord[0];
        movingPoint[1]=coord[1];
        movingPoint[2]=coord[2];
        movingPointContainer->InsertElement( pointId, movingPoint );
        pointId++;
    }
    movingPointSet->SetPoints( movingPointContainer );
    std::cout << "Number of moving Points = " << movingPointSet->GetNumberOfPoints() << std::endl;


    //-----------------------------------------------------------
    // Set up  the Metric
    //-----------------------------------------------------------
    typedef itk::EuclideanDistancePointMetric<
        PointSetType,
        PointSetType>
            MetricType;

    typedef MetricType::TransformType                 TransformBaseType;
    typedef TransformBaseType::ParametersType         ParametersType;
    typedef TransformBaseType::JacobianType           JacobianType;

    MetricType::Pointer  metric = MetricType::New();


    //-----------------------------------------------------------
    // Set up a Transform
    //-----------------------------------------------------------

    typedef itk::Euler3DTransform< double >      TransformType;

    TransformType::Pointer transform = TransformType::New();


    // Optimizer Type
    typedef itk::LevenbergMarquardtOptimizer OptimizerType;

    OptimizerType::Pointer      optimizer     = OptimizerType::New();
    optimizer->SetUseCostFunctionGradient(false);

    // Registration Method
    typedef itk::PointSetToPointSetRegistrationMethod<
        PointSetType,
        PointSetType >
            RegistrationType;


    RegistrationType::Pointer   registration  = RegistrationType::New();

    // Scale the translation components of the Transform in the Optimizer
    OptimizerType::ScalesType scales( transform->GetNumberOfParameters() );

    const double translationScale = 1000.0;   // dynamic range of translations
    const double rotationScale    =    1.0;   // dynamic range of rotations

    scales[0] = 1.0 / rotationScale;
    scales[1] = 1.0 / rotationScale;
    scales[2] = 1.0 / rotationScale;
    scales[3] = 1.0 / translationScale;
    scales[4] = 1.0 / translationScale;
    scales[5] = 1.0 / translationScale;

    unsigned long   numberOfIterations =  2000;
    double          gradientTolerance  =  1e-4;   // convergence criterion
    double          valueTolerance     =  1e-4;   // convergence criterion
    double          epsilonFunction    =  1e-5;   // convergence criterion


    optimizer->SetScales( scales );
    optimizer->SetNumberOfIterations( numberOfIterations );
    optimizer->SetValueTolerance( valueTolerance );
    optimizer->SetGradientTolerance( gradientTolerance );
    optimizer->SetEpsilonFunction( epsilonFunction );

    // Start from an Identity transform (in a normal case, the user
    // can probably provide a better guess than the identity...
    transform->SetIdentity();
    registration->SetInitialTransformParameters( transform->GetParameters() );

    //------------------------------------------------------
    // Connect all the components required for Registration
    //------------------------------------------------------
    registration->SetMetric(metric);
    registration->SetOptimizer(optimizer);
    registration->SetTransform(transform);
    registration->SetFixedPointSet(fixedPointSet);
    registration->SetMovingPointSet(movingPointSet);

    try
    {
        registration->StartRegistration();
    }
    catch( itk::ExceptionObject & e )
    {
        std::cout << e << std::endl;
        return NULL;
    }

    std::cout << "Solution = " << transform->GetParameters() << std::endl;

    TransformType::MatrixType rotMatrix;
    rotMatrix=transform->GetMatrix().GetTranspose();

    vtkMatrix4x4* vtkMatrix=vtkMatrix4x4::New();
    vtkMatrix->Element[0][0]=rotMatrix[0][0];
    vtkMatrix->Element[0][1]=rotMatrix[0][1];
    vtkMatrix->Element[0][2]=rotMatrix[0][2];
    vtkMatrix->Element[1][0]=rotMatrix[1][0];
    vtkMatrix->Element[1][1]=rotMatrix[1][1];
    vtkMatrix->Element[1][2]=rotMatrix[1][2];
    vtkMatrix->Element[2][0]=rotMatrix[2][0];
    vtkMatrix->Element[2][1]=rotMatrix[2][1];
    vtkMatrix->Element[2][2]=rotMatrix[2][2];

    TransformType::OutputVectorType transVector;
    transVector=transform->GetTranslation();

    vtkMatrix->Element[0][3]=transVector[0];
    vtkMatrix->Element[1][3]=transVector[1];
    vtkMatrix->Element[2][3]=transVector[2];

    return vtkMatrix;
}


vtkMatrix4x4* ICPRegistration::RegisterWithVTKICP(vtkPoints *fixedPoints, vtkPoints *movingPoints)
{
    vtkIterativeClosestPointTransform* icp=vtkIterativeClosestPointTransform::New();

    vtkPolyData* fixedPointSet = vtkPolyData::New();
    fixedPointSet->SetPoints(fixedPoints);
    fixedPointSet->Update();

    vtkMaskPoints* points1 = vtkMaskPoints::New();
    points1->SetInput(fixedPointSet);
    points1->SetMaximumNumberOfPoints(10000); // 500
    points1->GenerateVerticesOn();
    points1->Update();
    cout <<"Number of ICP fixed points = " << fixedPoints->GetNumberOfPoints() << endl;


    vtkPolyData* movingPointSet = vtkPolyData::New();
    movingPointSet->SetPoints(movingPoints);
    movingPointSet->Update();

    vtkMaskPoints* points2 = vtkMaskPoints::New();
    points2->SetInput(movingPointSet);
    points2->SetMaximumNumberOfPoints(1000); // 500
    points2->GenerateVerticesOn();
    points2->Update();
    cout << "Number of ICP moving points = " << movingPoints->GetNumberOfPoints() << endl;

    icp->GetLandmarkTransform()->SetModeToRigidBody();
    icp->StartByMatchingCentroidsOn();
    icp->SetCheckMeanDistance(1);
    icp->SetMaximumMeanDistance(0.001);
    icp->SetMaximumNumberOfIterations(30);
    icp->SetMaximumNumberOfLandmarks(50);
    icp->SetSource(points2->GetOutput());
    icp->SetTarget(points1->GetOutput());
    icp->Update();

    vtkMatrix4x4* transform = vtkMatrix4x4::New();
    transform->DeepCopy(icp->GetMatrix());

    fixedPointSet->Delete();
    points1->Delete();
    points2->Delete();
    movingPointSet->Delete();
    icp->Delete();

    return transform;
}

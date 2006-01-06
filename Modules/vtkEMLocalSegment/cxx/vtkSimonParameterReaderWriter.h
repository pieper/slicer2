/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkSimonParameterReaderWriter.h,v $
  Date:      $Date: 2006/01/06 17:57:34 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
// .NAME vtkSimonParameterReaderWriter - performs common math operations
// .SECTION Description
// ---------------------------------------------------------------------------------------------
/* Copyright (c) 1997 Simon Warfield simonw@bwh.harvard.edu */
// This is from Simon's Transform.h 
// It generates a a transform matrix from reading in a parameter file (e.g. final-param-file)
/* Convert from parameter estimates to transform matrix */
/* Convert from the estimated parameters to the transform matrix
     theta[0] = translate y dim
     theta[1] = translate x dim
     theta[2] = translate z dim
     theta[3] = rotate around y axis in degrees
     theta[4] = rotate around x axis in degrees
     theta[5] = rotate around z axis in degrees
     theta[6] = scale y axis
     theta[7] = scale x axis
     theta[8] = scale z axis
*/
// End of Simon's Copyright
// ---------------------------------------------------------------------------------------------



#ifndef __vtkSimonParameterReaderWriter_h
#define __vtkSimonParameterReaderWriter_h

#include "vtkObject.h"
#include "vtkSlicer.h"
#include <vtkEMLocalSegmentConfigure.h>
#include "vtkSetGet.h"

class  VTK_EMLOCALSEGMENT_EXPORT vtkSimonParameterReaderWriter : public vtkObject
{
public:
  static vtkSimonParameterReaderWriter *New();
  vtkTypeRevisionMacro(vtkSimonParameterReaderWriter,vtkObject);
 
  // Need vtkImageSimonResample
  // *inA, *inB, and *out can all be the same 
  static void matmult_3x3(float *inA, float *inB, float *out);
  static void matmult_3x4(float *inARotation, float *inATranslation, float *inBRotation,  float *inBTranslation, float *outRotation,  float *outTranslation);

  static int fast_invert_3x3_matrix(float *in, float *out);

  static void convertParmsToTransform(double *theta, float *transform, int numparms, int compute2Dregistration);
  // Description: 
  // paraType = 1 old Simon parameterisation 
  // paraType = 2 Kilian parameterisation 
  static int TurnParameteresIntoInverseRotationTranslation( double Xtranslate, double Ytranslate , double Ztranslate,
                                double Xrotate, double Yrotate, double Zrotate, double Xscale, double Yscale, double Zscale, 
                                float *invRotation, float *invTranslation, int paraType);
  // Description: 
  // paraType = 1 old Simon parameterisation 
  // paraType = 2 Kilian parameterisation 
  // 2DFlag   = 2D Registration  
  static int TurnParameteresIntoInverseRotationTranslation(const float *parameters, float *invRotation, float *invTranslation, int paraType, int TwoDFlag, int RigidFlag);
  static int TurnParameteresIntoInverseRotationTranslation(const double *parameters, float *invRotation, float *invTranslation, int paraType, int TwoDFlag, int RigidFlag);

  static int TurnParameteresIntoInverseRotationTranslation(double *Translation, double *Rotation, double *Scale, float *invRotation, float *invTranslation, int paraType, int  TwoDFlag, int RigidFlag);

  // To use Guimonds program I spit out a parameter file in the needed form 
  int transfereWarfieldToGuimondParameterFile(char *WarfieldFileName, char *GuimondFileName);
  // Takes Warfield parameter file, calculates the inverse and returns it into a guimond file 
  int generateInverseFromWarfieldToGuimondParameterFile(char *WarfieldFileName, char *GuimondFileName);
  // Generates the inverse from Warfield parameters and writes it into Warfield Parameters
  int generateInverseFromWarfieldToWarfieldParameterFile(char *WarfieldFileName, char *InverseWarfieldFileName);

  // Description
  // For Tcl interface 
  // returns a 1 if it did not work  and otherwise 0 
  int ReadFileParameter(char *WarfieldFileName) ;
  // Description
  // Inverts the file parameters -> if you do it twcie you have the original parameters back.
  // returns a 1 if it did not work and otherwise 0  
  int InvertFileParameter();
  
  // Tried out other ways did not work 
  // Translation and Rotation 
  vtkGetVector6Macro(FileParameter1,double);
  // Scaling 
  vtkGetVector3Macro(FileParameter2,double);

protected:
  vtkSimonParameterReaderWriter() { 
     FileParameter1 = this->FileParameter;
     FileParameter2 = &this->FileParameter[6];
  };
  ~vtkSimonParameterReaderWriter() {};
private:
  vtkSimonParameterReaderWriter(const vtkSimonParameterReaderWriter&);  // Not implemented.
  void operator=(const vtkSimonParameterReaderWriter&);  // Not implemented.

  double FileParameter[10];
  double *FileParameter1;
  double *FileParameter2;
};

#endif

/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================auto=*/
// .NAME vtkImageCurveRegion - Abstract Filter used in slicer to plot graphs
#ifndef __vtkImageCurveRegion_h
#define __vtkImageCurveRegion_h
#include "vtkImageData.h"
#include "vtkImageSource.h"
#include "vtkSlicer.h"


// Copied from vtkImageGeneral.h
// Abuse the type system.
#define COERCE(x, type) (*((type *)(&(x))))
// Some constants having to do with the way single
// floats are represented on alphas and sparcs
#define EMSEGMENT_MANTSIZE (23)
#define EMSEGMENT_SIGNBIT (1 << 31)
#define EMSEGMENT_EXPMASK (255 << EMSEGMENT_MANTSIZE)
#define EMSEGMENT_MENTMASK ((~EMSEGMENT_EXPMASK)&(~EMSEGMENT_SIGNBIT))
#define EMSEGMENT_PHANTOM_BIT (1 << EMSEGMENT_MANTSIZE)
#define EMSEGMENT_EXPBIAS 127
#define EMSEGMENT_SHIFTED_BIAS (EMSEGMENT_EXPBIAS << EMSEGMENT_MANTSIZE)
#define EMSEGMENT_SHIFTED_BIAS_COMP ((~ EMSEGMENT_SHIFTED_BIAS) + 1)
#define EMSEGMENT_ONE_OVER_2_PI 0.5/3.14159265358979
#define EMSEGMENT_ONE_OVER_ROOT_2_PI sqrt(EMSEGMENT_ONE_OVER_2_PI)
#define EMSEGMENT_MINUS_ONE_OVER_2_LOG_2 ((float) -.72134752)

class VTK_SLICER_BASE_EXPORT vtkImageCurveRegion : public vtkImageSource  {
public:
  static vtkImageCurveRegion *New();
  vtkTypeMacro(vtkImageCurveRegion,vtkImageSource);
  void PrintSelf(ostream& os, vtkIndent indent);

  void SetDimension(int value);
  vtkGetMacro(Dimension, int);
  vtkSetMacro(Xmin, float);
  vtkGetMacro(Xmin, float);
  vtkSetMacro(Xmax, float);
  vtkGetMacro(Xmax, float);
  // Description:
  // Define space between two points on the X-Axis to be calculated, e.g. 0.1, or define Xlength 
  vtkSetMacro(Xunit, float);
  vtkGetMacro(Xunit, float);
  vtkSetMacro(Xlength, int);
  vtkGetMacro(Xlength, int);
  // Description:
  // Ymin, Ymax, Yunit can only be set for Dimension = 2 (=> to calculate reange in second dimension, like Xmin Xmax for first dimension)
  vtkSetMacro(Ymin, float);
  vtkGetMacro(Ymin, float);
  vtkSetMacro(Ymax, float);
  vtkGetMacro(Ymax, float);
  // Description:
  // Define space between two points on the Y-Axis to be calculated, e.g. 0.1, or define Ylength 
  vtkSetMacro(Yunit, float);
  vtkGetMacro(Yunit, float);

  vtkSetMacro(Ylength, int);
  vtkGetMacro(Ylength, int);

  // Description:
  // Maximum and Minimum value of the function
  vtkGetMacro(FctMax, float);
  vtkGetMacro(FctMin, float);

  // Description:
  // Function to be executed. Currently implemented
  // 1 = Gausian
  // 2 = Log Gausian, where *min, *max, *unit correspond to the non log values (e.g. 0, 100, 1.0) 
  //     and Covariance and mean to log values  (3.65, 0.2)
  // 3 = Data will be read from file (FileName has to be defined)
  void SetFunction(int val);
  vtkGetMacro(Function, int);
  // Description:
  // Define first dimension before defining mean and covariance 
  // Also if you choose Function 2 Mean and Covirance correpsond to their log values , e.g. Mean = 50 => for function 2 enter 
  // Mean = log(50+1) !!
  void  SetMean(float value, int x) { if (this->Mean[x] != value) {this->Mean[x] = value; this->Modified();}} 
  float GetMean(int x) {return this->Mean[x];} 

  void  SetCovariance(float value, int y, int x) {if (this->Covariance[y][x] != value) {this->Covariance[y][x] = value; this->Modified();}} 
  float GetCovariance(int y, int x) {return this->Covariance[y][x];} 

  // Description:
  // Global probability, necessary if you have different curves with different weights 
  vtkSetMacro(Probability, float);
  vtkGetMacro(Probability, float);

  vtkGetStringMacro(FileName);
  vtkSetStringMacro(FileName);

protected:
  vtkImageCurveRegion();
  void DeleteVariables();
  ~vtkImageCurveRegion() {this->DeleteVariables();};

  int Dimension;
  int Function;
  float Xmin;
  float Xmax;
  float Xunit;
  int Xlength;

  float Ymin;
  float Ymax;
  float Yunit;
  int Ylength;

  float FctMax;
  float FctMin;

  float* Mean;
  float** Covariance;
  float Probability;

  char* FileName;

  virtual void ExecuteInformation();
  virtual void ExecuteData(vtkDataObject *data);
  void ExecuteDataReadFile(vtkDataObject *output) ;
  void ExecuteDataGauss(vtkDataObject *output);
private:
  vtkImageCurveRegion(const vtkImageCurveRegion&);  // Not implemented.
  void operator=(const vtkImageCurveRegion&);  // Not implemented.
};


#endif

  

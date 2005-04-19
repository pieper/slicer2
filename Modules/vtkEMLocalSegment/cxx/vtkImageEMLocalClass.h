/*=auto=========================================================================

(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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
// .NAME vtkImageEMLocalClass
#ifndef __vtkImageEMLocalClass_h
#define __vtkImageEMLocalClass_h 
  
#include <vtkEMLocalSegmentConfigure.h> 

#include "vtkSlicer.h"
#include "vtkImageEMGenericClass.h"

#define EMSEGMENT_NUM_OF_QUALITY_MEASURE 1

class VTK_EMLOCALSEGMENT_EXPORT vtkImageEMLocalClass : public vtkImageEMGenericClass
{
  public:
  // -----------------------------------------------------
  // Genral Functions for the filter
  // -----------------------------------------------------
  static vtkImageEMLocalClass *New();
  vtkTypeMacro(vtkImageEMLocalClass,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // ----------------------------------------- 
  // Image Data input 

  // Description:
  // Probability Data defining the spatial conditional label distribution
  void SetProbDataPtr(vtkImageData *image) {this->SetInput(1,image);}
  //BTX
  void SetProbDataPtr(void* initptr) {this->ProbDataPtr = initptr;}
  void* GetProbDataPtr() {return this->ProbDataPtr;}
  //ETX

  // Description:
  // Increments for probability data in Y and Z Dimension 
  vtkGetMacro(ProbDataIncY,int);
  vtkGetMacro(ProbDataIncZ,int);

  // Description:
  // Reference standard to define the performance of the algorithm, e.g. output is compared to the Dice Measure 
  void SetReferenceStandard(vtkImageData *image) {this->ReferenceStandardPtr = image;}
  //BTX
  vtkImageData* GetReferenceStandard() {return this->ReferenceStandardPtr;}
  //ETX

  // --------------------------------------------
  // Normal Data Input

  // Description:
  // Set label for the class
  vtkSetMacro(Label,short);

  // Description:
  // Set the Number of Input Images for subclasses
  void SetNumInputImages(int number); 

   void     SetLogCovariance(double value, int y, int x);
  //BTX
   double** GetLogCovariance() {return this->LogCovariance;}
   double*  GetLogCovariance(int i) {return this->LogCovariance[i];}
  //ETX

   void     SetLogMu(double mu, int x); 
   //BTX
   double*  GetLogMu(){ return this->LogMu;}
  //ETX

  // -------------------------------------------------------
  // Print Functions
 
  // Description:
  // Currenly only the following values defined 
  // 0 = Do not Print out any print quality 
  // 1 = Do a DICE comparison
  void SetPrintQuality(int init);

  vtkGetMacro(PrintQuality,int);
  
  // Check if the input image data is consistence with what we expect
  int CheckInputImage(vtkImageData * inData,int DataTypeOrig, int num, int outExt[6]);
  // Check and Assign ImageData to the different parameters 
  int CheckAndAssignImageData(vtkImageData *inData, int outExt[6]);

protected:
  vtkImageEMLocalClass();
  vtkImageEMLocalClass(const vtkImageEMLocalClass&) {};
  ~vtkImageEMLocalClass() {this->DeleteClassVariables();}
  void DeleteClassVariables();

  void operator=(const vtkImageEMLocalClass&) {};
  void ThreadedExecute(vtkImageData **inData, vtkImageData *outData,int outExt[6], int id) {};
  void ExecuteData(vtkDataObject *);   

  double *LogMu;                 // Intensity distribution of the classes (changed for Multi Dim Version)
  double **LogCovariance;        // Intensity distribution of the classes (changed for Multi Dim Version) -> This is the Coveriance Matrix
                                 // Be careful: the Matrix must be symmetric and positiv definite,
  void   *ProbDataPtr;           // Pointer to Probability Data 
  int    ProbDataIncY;           // Increments for probability data in Y Dimension 
  int    ProbDataIncZ;           // Increments for probability data in Z Dimension

  vtkImageData*  ReferenceStandardPtr;  // Reference Standard for measuring the performance of the algorithm

  int PrintQuality;        // Prints out a quality measure of the current result ( 1=  Dice )
};
#endif












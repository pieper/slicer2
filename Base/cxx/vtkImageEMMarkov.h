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
// .NAME vtkImageEMMarkov

#ifndef __vtkImageEMMarkov_h
#define __vtkImageEMMarkov_h


#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
 
#include "vtkImageData.h"
#include "vtkImageToImageFilter.h"
#include "vtkSlicer.h"

// Error Definiton  
// 1  = eveything correct
// -1 = Number of Classes not defined correctly
// -2 = Not all Mu defined correctly
// -3 = Not all Sigma defined correctly
// -4 = Not all Prob defined correctly
// -6 = Wrong index when in GetProbability
// -7 = SetMu: Incorrect index (" << index << ") or mu ("<<mu<<") < 0 !" << endl;
// -8 = SetSigma: Incorrect index (" << index << ") or sigma ("<<mu<<") < 0 !" << endl;
// -10 = Minimum Brightness value below 0 !



class VTK_SLICER_BASE_EXPORT vtkImageEMMarkov : public vtkImageToImageFilter
{
  public:
  static vtkImageEMMarkov *New();
  vtkTypeMacro(vtkImageEMMarkov,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Changing NumClasses re-defines also all the arrays which depon the number of classes e.g. prob

  void SetNumClasses(int Num);
  vtkGetMacro(NumClasses,int);

  vtkSetMacro(StartSlice, int);
  vtkGetMacro(StartSlice, int);

  vtkSetMacro(EndSlice, int);
  vtkGetMacro(EndSlice, int);

  vtkSetMacro(Error, int);
  vtkGetMacro(Error, int);

  // Description:
  // Setting the ImgTestNo > 0 executes the EM algorithm with a test picture
  // test = 1 => Creates horizontal stripped matrix with image values from 0 to 255 
  // test = 2 => Creates squared matrix with image values from 0 to 255 
  // test = 3 => Creates vertical stripped matrix with image values from 0 to 255
  vtkSetMacro(ImgTestNo,int);       
  vtkGetMacro(ImgTestNo,int);       
  // Description:
  // ImgTestDivision = the number of colors that the pixture should be devided in 
  //            if pixel == -1 => the number of divisions of the picture
  vtkSetMacro(ImgTestDivision, int); 
  vtkGetMacro(ImgTestDivision, int); 
  // Description:
  // ImgTestPixel = pixel length of one divison
  vtkSetMacro(ImgTestPixel, int); 
  vtkGetMacro(ImgTestPixel, int); 


  double GetProbability(int index);

  void SetLabelNumber(int index, int num);
  void SetLabel(int index, int Label);

  // Description:
  // Creates a Tissue Class Interation Matrix form the given image
  void TrainMarkovMatrix(int ***Image,int Ydim, int Xdim, float * outPtr);

  // Description:
  // Setting up Test Matrix to test 
  // test = 1 => Creates horizontal stripped matrix with image values from 0 to 255 
  // test = 2 => Creates squared matrix with image values from 0 to 255 
  // test = 3 => Creates vertical stripped matrix with image values from 0 to 255 
  // division = the number of colors that the pixture should be devided in 
  //            if pixel == -1 => the number of divisions of the picture
  // pixel = pixel length of one divison
  void setMatrix3DTest(int ***mat3D,int maxZ, int maxY, int maxX, int test,int division,int pixel);

protected:

  vtkImageEMMarkov();
  vtkImageEMMarkov(const vtkImageEMMarkov&) {};
  ~vtkImageEMMarkov();

  void operator=(const vtkImageEMMarkov&) {};

  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData,int outExt[6], int id);
  void ExecuteInformation(){this->vtkImageToImageFilter::ExecuteInformation();};
  void ExecuteInformation(vtkImageData *inData,vtkImageData *outData);
  // Necessary function for VTK 
  void ComputeInputUpdateExtent(int inExt[6], int outExt[6]);

  // Description:
  // Calculates the Maximum Class Probability for each brightness value  
  void CalculateMaxClassProb(double *MaxClass, int &ImageMin,int &ImageMax);

  void DeleteVariables();

  // Description:
  // Setting up Test Matrix to test 
  // test = 1 => Creates horizontal stripped matrix with image values from 0 to 255 
  // test = 2 => Creates squared matrix with image values from 0 to 255 
  // test = 3 => Creates vertical stripped matrix with image values from 0 to 255 
  // division = the number of colors that the pixture should be devided in 
  //            if pixel == -1 => the number of divisions of the picture
  // pixel = pixel length of one divison
  void setMatrixTest(int **mat, int maxY, int maxX, int test,int division, int pixel, int offset);
  void setVectorTest(int *vec, int maxX,int test,int division,int pixel, int offset);

  void setMatrix(int **mat, int maxY, int maxX, int val);

  int Error;           // Error Flag while executing 
  int NumClasses;      // Number of Classes

  int StartSlice;      // First Slide to be segmented
  int EndSlice;        // Last Slide to be segmented 

  int ImgTestNo;       // Segment an image test picture (-1 => No, > 0 =>certain Test pictures)
  int ImgTestDivision; // Number of divisions/colors of the picture
  int ImgTestPixel;    // Pixel lenght on one diviosn (pixel == -1 => max pixel length for devision)


  int **Label;            // Intensity distribution of the classes
  int *LabelNumber;            // Intensity distribution of the classes
  double *ClassProbability;  // Prior Probability of the classes
};
#endif



 








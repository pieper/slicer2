/*=auto=========================================================================

(c) Copyright 2002 Massachusetts Institute of Technology

Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for any purpose, 
provided that the above copyright notice and the following three paragraphs 
appear on all copies of this software.  Use of this software constitutes 
acceptance of these terms and conditions.

IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================auto=*/
// .NAME vtkImageEMMarkov
#ifndef __vtkImageEMMarkov_h
#define __vtkImageEMMarkov_h


#include <stdio.h>
#include <math.h>
#include <cmath>
#include <ctype.h>
#include <string.h>
 
#include "vtkImageToImageFilter.h"

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


class VTK_EXPORT vtkImageEMMarkov : public vtkImageToImageFilter
{
  public:
  static vtkImageEMMarkov *New();
  vtkTypeMacro(vtkImageEMMarkov,vtkObject);
  void PrintSelf(ostream& os);

  // Description:
  // Changing NumClasses re-defines also all the arrays which depon the number of classes e.g. prob
  void SetNumClasses(int NumberOfClasses);
  int GetNumClasses() {return this->NumClasses;}

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
  // Description:
  // Defines the tissue class interaction matrix MarkovMatrix(z,y,x)
  // where z = direction [1...6], 
  //       y = class of the current pixel t+1 
  //       x = class of neighbouring pixel t 
  double GetMarkovMatrix(int x,int y, int z);

  int get_StartSlice() {return StartSlice;}
  int get_EndSlice() {return EndSlice;}
  int get_NumClasses() {return NumClasses;}
  // Description:
  // Getting the ImgTestNo > 0 executes the EM algorithm with a test picture
  // test = 1 => Creates horizontal stripped matrix with image values from 0 to 255 
  // test = 2 => Creates squared matrix with image values from 0 to 255 
  // test = 3 => Creates vertical stripped matrix with image values from 0 to 255
  int get_ImgTestNo () {return this->ImgTestNo;}
  // Description:
  // ImgTestDivision = the number of colors that the pixture should be devided in 
  //            if pixel == -1 => the number of divisions of the picture
  int get_ImgTestDivision () {return this->ImgTestDivision;}
  // Description:
  // ImgTestPixel = pixel length of one divison
  int get_ImgTestPixel () {return this->ImgTestPixel;}

  void SetMu(double mu, int index);
  void SetSigma(double sigma, int index);

  void SetComponentExtent(int extent[6]);
  void SetComponentExtent(int minX, int maxX, int minY, int maxY,
    int minZ, int maxZ);

  void GetComponentExtent(int extent[6]);
  int *GetComponentExtent() {return this->ComponentExtent;}

  // Description:
  // Creates a Tissue Class Interation Matrix form the given image
  void TrainMarkovMatrix(double ***Image,int Ydim, int Xdim,int ImageMin, int ImageMax);

  // Description:
  // Setting up Test Matrix to test 
  // test = 1 => Creates horizontal stripped matrix with image values from 0 to 255 
  // test = 2 => Creates squared matrix with image values from 0 to 255 
  // test = 3 => Creates vertical stripped matrix with image values from 0 to 255 
  // division = the number of colors that the pixture should be devided in 
  //            if pixel == -1 => the number of divisions of the picture
  // pixel = pixel length of one divison
  void setMatrix3DTest(double ***mat3D,int maxZ, int maxY, int maxX, int test,int division,int pixel);

  double *** MarkovMatrix; // Markov Model Parameters: Matrix3D mrfparams(this->NumClasses,this->NumClasses,4);

protected:

  vtkImageEMMarkov();
  vtkImageEMMarkov(const vtkImageEMMarkov&) {};
  ~vtkImageEMMarkov();

  void operator=(const vtkImageEMMarkov&) {};

  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData,int outExt[6], int id);
  void ExecuteInformation(){this->vtkImageToImageFilter::ExecuteInformation();};
  void ExecuteInformation(vtkImageData *inData,vtkImageData *outData);
  void ComputeInputUpdateExtent(int inExt[6], int outExt[6]);

  // Description:
  // Calculates the Maximum Class Probability for each brightness value  
  void CalculateMaxClassProb(double *MaxClass, int &ImageMin,int &ImageMax);

  // Description:
  // Before training starts it checks of all values are set correctly
  int checkValues(int ImageMin); // Checks of all paramters are declared 

  void DeleteVariables();

  // Description:
  // Setting up Test Matrix to test 
  // test = 1 => Creates horizontal stripped matrix with image values from 0 to 255 
  // test = 2 => Creates squared matrix with image values from 0 to 255 
  // test = 3 => Creates vertical stripped matrix with image values from 0 to 255 
  // division = the number of colors that the pixture should be devided in 
  //            if pixel == -1 => the number of divisions of the picture
  // pixel = pixel length of one divison
  void setMatrixTest(double **mat, int maxY, int maxX, int test,int division, int pixel, int offset);
  void setVectorTest(double *vec, int maxX,int test,int division,int pixel, int offset);

  void setMatrix(double **mat, int maxY, int maxX, double val);

  int Error;           // Error Flag while executing 
  int NumClasses;      // Number of Classes

  int StartSlice;      // First Slide to be segmented
  int EndSlice;        // Last Slide to be segmented 

  int ComponentExtent[6]; // Needed to define MarkovField Length 

  int ImgTestNo;       // Segment an image test picture (-1 => No, > 0 =>certain Test pictures)
  int ImgTestDivision; // Number of divisions/colors of the picture
  int ImgTestPixel;    // Pixel lenght on one diviosn (pixel == -1 => max pixel length for devision)

  double *Mu;          // Intensity distribution of the classes
  double *Sigma;       // Intensity distribution of the classes
  double *ClassProbability;        // Prior Probability of the classes
};
#endif



 








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
// .NAME vtkImageEMSegmenter
 
#ifndef __vtkImageEMSegmenter_h
#define __vtkImageEMSegmenter_h

#include <math.h>
#include <cmath>
#include "vtkSlicer.h"
#include "vtkImageData.h"
#include "vtkImageToImageFilter.h"

#define EMSEGMENT_ONE_OVER_2_PI 0.5/3.14159265358979
#define EMSEGMENT_ONE_OVER_ROOT_2_PI sqrt(EMSEGMENT_ONE_OVER_2_PI)

class VTK_SLICER_BASE_EXPORT vtkImageEMSegmenter : public vtkImageToImageFilter
{
  public:

  static vtkImageEMSegmenter *New();
  vtkTypeMacro(vtkImageEMSegmenter,vtkImageToImageFilter);
  void PrintSelf(ostream& os);

  // Description:
  // Segmentats the Image using the EM-MF Algorithm   
  void vtkImageEMAlgorithm(double *InputOutputVector,int imgX,int imgY, int  NumSlices, int ImageMax);
 
  // Description:
  // Changing NumClasses re-defines also all the arrays which depon the number of classes e.g. prob
  void SetNumClasses(int NumberOfClasses);

  vtkGetMacro(NumClasses, int);

  vtkSetMacro(NumIter, int);
  vtkGetMacro(NumIter, int);

  vtkSetMacro(NumRegIter, int);
  vtkGetMacro(NumRegIter, int);

  vtkSetMacro(Alpha, double);
  vtkGetMacro(Alpha, double);

  vtkSetMacro(SmoothingWidth, int);
  vtkGetMacro(SmoothingWidth, int);

  vtkSetMacro(SmoothingSigma, int);
  vtkGetMacro(SmoothingSigma, int);

  vtkSetMacro(StartSlice, int);
  vtkGetMacro(StartSlice, int);

  vtkSetMacro(EndSlice, int);
  vtkGetMacro(EndSlice, int);

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
  // Description:
  // PrintIntermediateResults = 1 => Prints out intermediate result of 
  // the algorithm in a matlab file. The file is called  EMSegmResult<iteration>.m  
  vtkSetMacro(PrintIntermediateResults, int); 
  vtkGetMacro(PrintIntermediateResults, int); 
  // Description:
  // Print out the result of which slice 
  vtkSetMacro(PrintIntermediateSlice, int); 
  vtkGetMacro(PrintIntermediateSlice, int); 
  // Description:
  // Print out the result after how many iteration steps
  vtkSetMacro(PrintIntermediateFrequency, int); 
  vtkGetMacro(PrintIntermediateFrequency, int); 

 
  void SetProbability(double prob, int index);
  void SetMu(double mu, int index);
  void SetSigma(double sigma, int index);
  void SetLabel(int label, int index);

  // Description:
  // Defines the tissue class interaction matrix MrfParams(k,i,j)
  // where k = direction [1...6], 
  //       i = class of the current pixel t+1 
  //       j = class of neighbouring pixel t 
  void SetMarkovMatrix(double value, int j, int i, int k);
  int get_StartSlice () {return this->StartSlice;}
  int get_EndSlice () {return this->EndSlice;}

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

  void TestConv();

protected:
  vtkImageEMSegmenter();
  vtkImageEMSegmenter(const vtkImageEMSegmenter&) {};
  ~vtkImageEMSegmenter();
  void DeleteVariables();

  void operator=(const vtkImageEMSegmenter&) {};
  // void ExecuteInformation(){this->vtkImageToImageFilter::ExecuteInformation();};
  // void ExecuteInformation(vtkImageData *inData, vtkImageData *outData); 
  // When it works on parallel machines use : void ThreadedExecute(vtkImageData *inData, vtkImageData *outData,int outExt[6], int id);
  // otherwise :
  void ExecuteData(vtkDataObject *);

  // Description:
  // Opens up a new file and writes down result in the file
  void WriteVectorToFile (char *filename, char *varname,double *vec, int xMax) const;

  // Description:
  // Writes Vector to file in Matlab format if name is specified otherwise just 
  // writes the values in the file
  void WriteVectorToFile (FILE *f,char *name, double *vec, int xMax) const;

  // Description:
  // Opens up a new file and writes down result in the file
  void WriteMatrixToFile (char *filename,char *varname, double **mat, int imgY, int imgX) const;

  // Description:
  // Writes Matrix to file in Matlab format if name is specified otherwise just 
  // writes the values in the file
  void WriteMatrixToFile (FILE *f,char *name,double **mat, int imgY, int imgX) const;

  // Description
  //  Smoothes  3D-Matrix
  // w(k) = sum(u(j)*v(k+1-j))
  // returns Matrix of size r_m
  void smoothConv(double ***mat3D, int mat3DZlen, int mat3DYlen, int mat3DXlen, double v[],int vLen);

  // Description:
  // Convolution and polynomial multiplication . 
  // This is assuming u and 'this' have the same dimension
  void convMatrix3D(double*** mat3D, double*** U,int mat3DZlen, int mat3DYlen, int mat3DXlen, double v[],int vLen);

  // Description:
  // Convolution and polynomial multiplication . 
  // This is assuming u and 'this' have the same dimension
  // Convolution and polynomial multiplication . 
  // This is assuming u and 'this' have the same dimension
  void convMatrix(double** mat, double** U, int matYlen, int matXlen, double v[], int vLen);

  // Description:
  // Same just v is a row vector instead of column one
  // We use the following equation :
  // conv(U,v) = conv(U',v')' => conv(U,v') = conv(U',v)';
  void convMatrixT(double** mat, double** U, int matYlen, int matXlen, double v[], int vLen);

  // Description:
  // Convolution and polynomial multiplication . 
  // This is assuming u and 'this' have the same dimensio
  void convVector(double vec[], double u[], int uLen, double v[], int vLen);

  void PrintMatrix(double **mat, int yMax,int xMax);

  // Description:
  // Before EM Algorithm is started it checks of all values are set correctly
  int checkValues(); 

  // Description:
  // Defines the Label map of a given image
  void DeterminLabelMap(double *LabelMap, double **w_m, int imgXY,int imgZ);

  // Description:
  // Print out intermediate result of the algorithm in a matlab file
  // The file is called  EMSegmResult<iteration>.m
  void PrintMatlabGraphResults(int iter,int slice,int FullProgram,int imgXY, int imgY, int imgX, double **w_m,double *b_m);

  int NumClasses;      // Number of Classes
  int NumIter;         // Number of EM-iterations
  int NumRegIter;      // Number of iteration in E- Step to regularize weights 
  double Alpha;        // alpha - Paramter 0<= alpaha <= 1

  int SmoothingWidth;  // Width for Gausian to regularize weights   
  int SmoothingSigma;  // Sigma paramter for regularizing Gaussian
 
  int StartSlice;      // First Slide to be segmented
  int EndSlice;        // Last Slide to be segmented 

  int ImgTestNo;       // Segment an image test picture (-1 => No, > 0 =>certain Test pictures)
  int ImgTestDivision; // Number of divisions/colors of the picture
  int ImgTestPixel;    // Pixel lenght on one diviosn (pixel == -1 => max pixel length for devision)

  int PrintIntermediateResults;    //  Print intermediate results in a Matlab File (No = 0, Yes = 1)
  int PrintIntermediateSlice;      //  Print out the result of which slide 
  int PrintIntermediateFrequency;   //  Print out the result after how many steps 

  double *Mu;                   // Intensity distribution of the classes
  double *Sigma;                // Intensity distribution of the classes
  double *Prob;                 // Prior Probability of the classes
  double *Label;                // Prior Probability of the classes

  double ***MrfParams;         // Markov Model Parameters: Matrix3D mrfparams(this->NumClasses,this->NumClasses,4);
};
#endif


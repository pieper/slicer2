/*=auto=========================================================================

(c) Copyright 2001 Massachusetts Institute of Technology

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
// .NAME vtkImageEMSegmenter
 
#ifndef __vtkImageEMSegmenter_h
#define __vtkImageEMSegmenter_h

#include "vtkImageEMMarkov.h"

class VTK_EXPORT vtkImageEMSegmenter : public vtkImageToImageFilter
{
  public:

  static vtkImageEMSegmenter *New();
  vtkTypeMacro(vtkImageEMSegmenter,vtkObject);
  void PrintSelf(ostream& os);

//BTX
  // Description:
  // Segmentats the Image using the EM-MF Algorithm   
  template <class T> void vtkImageEMAlgorithm(T *in1Ptr, T *outPtr,int imgX,int imgY, int  NumSlices, int inIncY, int inIncZ, int outIncY, int outIncZ );
  // void vtkImageEMAlgorithm(T *in1Ptr, T *outPtr,int imgX,int imgY, int  NumSlices, int inIncY, int inIncZ, int outIncY, int outIncZ );
//ETX
 
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

  // Description:
  // Tests the convolution function
  void TestConv();  // Test Convolution function : Works !

protected:

  vtkImageEMSegmenter();
  vtkImageEMSegmenter(const vtkImageEMSegmenter&) {};
  ~vtkImageEMSegmenter(){};
  void operator=(const vtkImageEMSegmenter&) {};
  
  void ExecuteInformation(){this->vtkImageToImageFilter::ExecuteInformation();};
  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData,int outExt[6], int id);
 
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

  // Do not Tcl the following lines 
//BTX
  vtkImageEMVector Mu, Sigma;   // Intensity distribution of the classes
  vtkImageEMVector Prob;        // Prior Probability of the classes
  vtkImageEMVector Label;       // Prior Probability of the classes
  vtkImageEMMatrix3D MrfParams; // Markov Model Parameters: Matrix3D mrfparams(this->NumClasses,this->NumClasses,4);

  // Description:
  //Caluclates the Gaussian with mean m, sigma s and input x 
  inline double Gauss(double x,double m,double s); 

  // Description:
  // Before EM Algorithm is started it checks of all values are set correctly
  int checkValues(vtkImageEMMatrix3D Volume); 

  // Description:
  // Print out intermediate result of the algorithm in a matlab file
  // The file is called  EMSegmResult<iteration>.m
  template <class T>  void PrintMatlabGraphResults(int iter,int slice,int FullProgram,int imgXY, int imgY, int imgX,T *outPtr,int outIncY, int outIncZ, vtkImageEMMatrix w_m,vtkImageEMMatrix3D b_m);

  // Description:
  // Defines the Label map of a given image
  template <class T> void DeterminLabelMap(T *outPtr, vtkImageEMMatrix w_m, int imgX,int imgY, int  NumSlices, int imgXY,  int NumClassPlus, int outIncY, int outIncZ);

//ETX
};
#endif












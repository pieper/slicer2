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
// .NAME vtkImageEMMarkov

#ifndef __vtkImageEMMarkov_h
#define __vtkImageEMMarkov_h


#include <stdio.h>
#include <math.h>
#include <cmath>
#include <ctype.h>
#include <string.h>

#include "vtkImageToImageFilter.h"

class vtkImageEMMatrix3D;
class vtkImageEMMatrix;

//BTX
class vtkImageEMError {
public:
  vtkImageEMError(const char* s) {
    std::cerr <<"Error: " << s << "\n";
    // exit(0);
  } 
};
class vtkImageEMVector {
  int len;
  double* vec;

  void allocate(int dim=0);            // Allocates memory for class
  void allocate(int dim, double val);  // Allocates memory for class
  void deallocate();  // Deallocates memory for class
  
public:
  vtkImageEMVector(int dim=0) {  this->allocate(dim);}
  vtkImageEMVector(int dim,double val) { this->allocate(dim,val);}
  vtkImageEMVector(const vtkImageEMVector & in); // Creats new vector by copiing 'in' 
  ~vtkImageEMVector(){deallocate();}

  double& operator [](int i) {return this->vec[i];}
  const double& operator [] (int i) const { return this->vec[i]; }

  vtkImageEMVector& operator = ( const vtkImageEMVector& v);

  int get_len() {return len;}
  void setVector(double val);                    // Sets with value val
  void setVector(vtkImageEMMatrix v,int start, int end, int column); // Sets vec = v[start:end][column]

  void Resize(int dim=0);                        // Resizes Vector to length dim 
  void Resize(int dim, double val);              // Resizes Vector to length dim with value 0.0

  void Reshape(vtkImageEMMatrix3D im);           // Turns the 3D Matrix into a vector
  void Reshape(vtkImageEMMatrix im);             // Turns the Matrix into a vector

  void PrintVector(int xMax = -1);               // Prints out the Vector vec
  void MatrixCol(vtkImageEMMatrix im, int col);  // Sets 'this' equal to  Column 'col' of Matrix im  
  void conv(vtkImageEMVector u,vtkImageEMVector v); // Convolution and polynomial multiplication . 
  void WriteVectorToFile (char *filename,char *varname) const; 
  // Open up a file and writes Vector to it (varname != NULL => Matlab Format)
  void WriteVectorToFile (FILE *f,char *name) const; // Writes Vector to a file (Name != NULL => Matlab Format)
  void setVectorTest(int test,int division,int pixel, int offset); // Defines a test image vector look in cxx file for setting
};

class vtkImageEMMatrix {
  int dimy;
  vtkImageEMVector* mat;

  void allocate(int y=0,int x=0);
  void allocate(int y,int x, double val);
  void deallocate();

public:
  vtkImageEMMatrix(int y=0,int x=0) {this->allocate(y,x);}
  vtkImageEMMatrix(int y,int x, double val) {this->allocate(y,x,val);}
  vtkImageEMMatrix(const vtkImageEMMatrix & in);       // Creats new matrix by copiing 'in' 
  ~vtkImageEMMatrix() {deallocate();}
 
  vtkImageEMVector& operator [] (int y);
    const vtkImageEMVector& operator [] (int i) const;

  vtkImageEMMatrix& operator = (const vtkImageEMMatrix& Equal); 

  int get_dim(int dim) ;                               // dim =  1 => Y Dimension, =2 => X Dimension  
  void setMatrix(double val);                          // Set all the values to the matrix value 

  void Resize(int y=0, int x= 0);                      // Resizes Matrix to size y x 
  void Resize(int y, int x, double val);              // Resizes Vector to length dim with value 0.0

  void PrintMatrix(int yMax = -1,int xMax = -1);       // Prints out the Matrix
  void ColMatrix(vtkImageEMVector v, int col);         // Sets column 'col' of this->mat to vector v
  void Transpose(vtkImageEMMatrix Trans);              // Transpose the Matrix Trans
  void SetEqual(double value);                         // Sets the whole Matrix equal to one value

  void conv(vtkImageEMMatrix U, vtkImageEMVector v);   // Convolution and polynomial multiplication 
  void convT(vtkImageEMMatrix U, vtkImageEMVector v);  // Convoluton where v is seen as row vector  
  void WriteMatrixToFile (char *filename,char *varname) const; //Opens up a new file and writes down result in the file
  void WriteMatrixToFile (FILE *f,char *name) const; // Writes Matrix to a file (Name != NULL => Matlab Format)
  void setMatrixTest(int test,int division, int pixel, int offset); // Defines a test image vector look in cxx file for setting
}; 

class vtkImageEMMatrix3D {
  int dimz;
  vtkImageEMMatrix* mat3D;

  void allocate(int z=0, int y = 0, int x = 0 );
  void allocate(int z, int y, int x, double val);
  void deallocate();

public:
  vtkImageEMMatrix3D(int z=0,int y=0,int x = 0) {this->allocate(z,y,x);}
  vtkImageEMMatrix3D(int z,int y,int x, double val) {this->allocate(z,y,x,val);}
  vtkImageEMMatrix3D(const vtkImageEMMatrix3D & in);                // Creates new 3D Matrix by copiing 'in' 

  ~vtkImageEMMatrix3D() {this->deallocate();}

  // Be carefull: vtkImageEMMatrix3D M
  // => M[z][y][x] not M[y][x][z] as we would expect !
  vtkImageEMMatrix& operator [] (int z);
  const vtkImageEMMatrix & operator [] (int i) const; 
  
  void Resize(int z = 0, int y = 0, int x= 0);
  void Resize(int z, int y, int x, double val);
  void PrintMatrix3D(int zMax = -1,int yMax = -1,int xMax = -1); // Prints out the Matrix3D mat3D


  void setMatrix3D(double val);                   // Creates new 3D matrix with value val
  int get_dim(int dim);                          // dim =1 => Z Dimension, =2 => Y Dimension, = 3 => X Dimension  
  void Reshape(vtkImageEMVector v);              // Turns the vector into a 3D Matrix
  void smoothConv(vtkImageEMMatrix3D U,vtkImageEMVector v); // Smoothes 3D-Matrix through convolution in Y,X,Z Dimension
  void conv(vtkImageEMMatrix3D U,vtkImageEMVector v);       // Convolution and polynomial multiplication in Z Dimension

  void WriteMatrix3DToFile (FILE *f,char *name) const; // Writes 3DMatrix to a file (Name != NULL => Matlab Format) 
  void setMatrix3DTest(int test,int division, int pixel); // Defines a test image vector look in cxx file for setting
};

//ETX

// Error Definiton  
// 1  = eveything correct
// -1 = Dimension of Volume not correct
// -2 = Not all Mu defined correctly
// -3 = Not all Sigma defined correctly
// -4 = Not all Prob defined correctly
// -6 = Wrong index when in GetProbability
// -7 = SetMu: Incorrect index (" << index << ") or mu ("<<mu<<") < 0 !" << endl;
// -8 = SetSigma: Incorrect index (" << index << ") or sigma ("<<mu<<") < 0 !" << endl;
// -10 = Number of Classes not defined correctly


class VTK_EXPORT vtkImageEMMarkov : public vtkImageToImageFilter
{
  public:
  static vtkImageEMMarkov *New();
  vtkTypeMacro(vtkImageEMMarkov,vtkObject);
  void PrintSelf(ostream& os);

  // Description:
  // Changing NumClasses re-defines also all the arrays which depon the number of classes e.g. prob
  void SetNumClasses(int NumberOfClasses);
  vtkGetMacro(NumClasses, int);

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
  void TrainMarkovMatrix(vtkImageEMMatrix3D Image,int ImageMin, int ImageMax);
  
//BTX
  vtkImageEMMatrix3D MarkovMatrix; // Markov Model Parameters: Matrix3D mrfparams(this->NumClasses,this->NumClasses,4);
//ETX

protected:

  vtkImageEMMarkov();
  vtkImageEMMarkov(const vtkImageEMMarkov&) {};

  // critical to have a virtual descructor!
  // virtual ~vtkImageEMSegm();
  // Kilian: I do not think so 
  ~vtkImageEMMarkov(){};

  void operator=(const vtkImageEMMarkov&) {};

  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData,int outExt[6], int id);
  void ExecuteInformation(){this->vtkImageToImageFilter::ExecuteInformation();};

  // Description:
  // Calculates the Maximum Class Probability for each brightness value  
  void CalculateMaxClassProb(vtkImageEMVector & MaxClass, int &ImageMin,int &ImageMax);
  // Description:
  //Caluclates the Gaussian with mean m, sigma s and input x 
  double Gauss(int x,double m,double s); //Caluclates the Gaussian with mean m, sigma s and input x 
  // Description:
  // Before training starts it checks of all values are set correctly
  int checkValues(vtkImageEMMatrix3D Volume); // Checks of all paramters are declared 
 
  int Error;           // Error Flag while executing 
  int NumClasses;      // Number of Classes

  int StartSlice;      // First Slide to be segmented
  int EndSlice;        // Last Slide to be segmented 

  int ComponentExtent[6]; // Needed to define MarkovField Length 

  int ImgTestNo;       // Segment an image test picture (-1 => No, > 0 =>certain Test pictures)
  int ImgTestDivision; // Number of divisions/colors of the picture
  int ImgTestPixel;    // Pixel lenght on one diviosn (pixel == -1 => max pixel length for devision)

  // Do not Tcl the following lines 
//BTX
  vtkImageEMVector Mu, Sigma;   // Intensity distribution of the classes
  vtkImageEMVector ClassProbability;        // Prior Probability of the classes
//ETX
};
#endif












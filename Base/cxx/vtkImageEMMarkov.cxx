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
#include "vtkImageEMMarkov.h"
#include "vtkObjectFactory.h"

static inline double EMMarkovGauss(int x,double m,double s) { 
  if (s > 0) return (1 / (sqrt(2*3.14159265358979)*s)*exp(-(x-m)*(x-m)/(2*pow(s,2))));
  return (m == x ? 1e20:0);
} 

//------------------------------------------------------------------------------
vtkImageEMMarkov* vtkImageEMMarkov::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageEMMarkov");
  if(ret)
  {
    return (vtkImageEMMarkov*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageEMMarkov;
}

//----------------------------------------------------------------------------
vtkImageEMMarkov::vtkImageEMMarkov()
{
  this->NumClasses = 0;       // Number of Classes - has to be set to 0 bc currently nothing is set
  this->StartSlice = 1;       // First Slide to be segmented
  this->EndSlice = 1;         // Last Slide to be segmented  
  this->Error = 1;             // Error flag -> if Error < 0 than error occured
  this->ComponentExtent[0] =  0; // What size will be the output Markov matrix 
  this->ComponentExtent[1] =  0; 
  this->ComponentExtent[2] =  0; 
  this->ComponentExtent[3] =  0; 
  this->ComponentExtent[4] =  0; 
  this->ComponentExtent[5] =  0; 
  this->ImgTestNo = -1;       // Segment an image test picture (-1 => No, > 0 =>certain Test pictures)
  this->ImgTestDivision = -1; // Number of divisions/colors of the picture
  this->ImgTestPixel = -1;    // Pixel lenght on one diviosn (pixel == -1 => max pixel length for devision)
  this->Mu = NULL;this->Sigma = NULL;this->ClassProbability = NULL;this->MarkovMatrix = NULL;
}

vtkImageEMMarkov::~vtkImageEMMarkov(){
  this->DeleteVariables();
}

//----------------------------------------------------------------------------
void vtkImageEMMarkov::ComputeInputUpdateExtent(int inExt[6], int outExt[6])
{
   this->GetInput()->GetWholeExtent(inExt);
   // Stole this from some other code -> might help
   // int *wholeExtent;

   // outExt = outExt;
   // wholeExtent = this->GetInput()->GetWholeExtent();
   // memcpy(inExt, wholeExtent, 6*sizeof(int));
}


//----------------------------------------------------------------------------
void vtkImageEMMarkov::PrintSelf(ostream& os, vtkIndent indent)
{
}

// To chage anything about output us this executed before Thread

//----------------------------------------------------------------------------
void vtkImageEMMarkov::ExecuteInformation(vtkImageData *inData, vtkImageData *outData) 
{
  
  int ext[6];
  float spacing[3], origin[3];
  spacing[0] = 1.0;
  spacing[1] = 1.0;
  spacing[2] = 1.0;

  origin[0] = 0.0;
  origin[1] = 0.0;
  origin[2] = 0.0;


  // Check if extend is sset correctly 
  ext[0] = ext[2] = ext[4] = 0;
  ext[1] = ext[3] = this->NumClasses-1;
  ext[5] = 5;
   
  outData->SetOrigin(origin);
  outData->SetNumberOfScalarComponents(1);
  outData->SetWholeExtent(ext);
  outData->SetSpacing(spacing);
  // outData->SetScalarType(VTK_FLOAT);
  // The problem why I cannot use the outcome for the filter directly reading from tcl bc if I set the filter to VTK_FLOAT I get grabage
  // If I set it to VTK_SHORT everything is fine
  outData->SetScalarType(VTK_FLOAT);
}





//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageEMMarkovExecute(vtkImageEMMarkov *self,vtkImageData *in1Data, T *in1Ptr,int inExt[6],vtkImageData *outData, T *outPtr,int outExt[6], int maxZ, int id)
{
  int idxR, idxY, idxZ;
  int maxY;
  int inIncX, inIncY, inIncZ;
  int outIncX, outIncY, outIncZ;
  int rowLength;
  int StartEndSlice;

  // find the region to loop over
  rowLength = (inExt[1] - inExt[0]+1)*in1Data->GetNumberOfScalarComponents();
  maxY = inExt[3] - inExt[2] + 1; // outExt[3/2] = Relative Maximum/Minimum Y index  
  StartEndSlice = self->get_EndSlice() - self->get_StartSlice() + 1;

  // Get increments to march through data 
  in1Data->GetContinuousIncrements(inExt, inIncX, inIncY, inIncZ);
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);

  // The Slices of the images 
  double ***Volume = new double**[StartEndSlice];
  for (idxZ = 0; idxZ < StartEndSlice; idxZ++) {
    Volume[idxZ]  = new double*[maxY];
    for (idxY = 0; idxY < maxY; idxY++)
      Volume[idxZ][idxY] = new double[rowLength];
  }
  // Loop through input pixels 

  // 1.) Read thorugh beginning slides we do not want to read through for input and output
  in1Ptr += ((rowLength+inIncY)*maxY + inIncZ)*(self->get_StartSlice() -1); 

  // 2.) Read thorugh the slides that should be segmented 
  int Max =  ((int)*in1Ptr);
  int Min = ((int)*in1Ptr);

  if (self->get_ImgTestNo() < 1) {
    for (idxZ = 0; idxZ < StartEndSlice ; idxZ++) {
      for (idxY = 0; idxY < maxY; idxY++) {
        for (idxR = 0; idxR < rowLength; idxR++) {
          Volume[idxZ][idxY][idxR] = ((double) * in1Ptr);
          if (((int)*in1Ptr) > Max) Max = ((int)*in1Ptr);
          else if (((int)*in1Ptr) < Min) Min = ((int)*in1Ptr);
          in1Ptr++;
        }
        in1Ptr += inIncY;
      }
      in1Ptr += inIncZ;
    }
  } else {
    self->setMatrix3DTest(Volume,StartEndSlice,maxY,rowLength,self->get_ImgTestNo(),self->get_ImgTestDivision(),self->get_ImgTestPixel());
    Max = 255;
    Min = 0;
  } 
  // 3.) Run Algorith to find out Matrix
  self->TrainMarkovMatrix(Volume, maxY, rowLength, Min, Max);

 // The problem why I cannot use the outcome for the filter directly reading from tcl bc if I set the filter to VTK_FLOAT I get grabage
  // If I set it to VTK_SHORT everything is fine

 // 4.) Transfere CIM Matrix
  for (idxZ = 1; idxZ < 7; idxZ++) {
    for (idxY = 1; idxY <= self->GetNumClasses(); idxY++) {
      for (idxR = 1; idxR <= self->GetNumClasses(); idxR++) {
        *outPtr = (T) 1.01;
    // Kilian: this cannot work outIncX == 0 -> should be outPtr ++ 
        outPtr ++;
      }
      outPtr += outIncY;
    }
    outPtr += outIncZ; 
  }
  for (idxZ = 0; idxZ < StartEndSlice; idxZ++) {
    for (idxY = 0; idxY < maxY; idxY++)
      delete []Volume[idxZ][idxY];
    delete[] Volume[idxZ];
  }
  delete[] Volume;
}

//----------------------------------------------------------------------------
// This method is passed a input and output datas, and executes the filter
// algorithm to fill the output from the inputs.
// It just executes a switch statement to call the correct function for
// the datas data types.

void vtkImageEMMarkov::ThreadedExecute(vtkImageData *inData, vtkImageData *outData,int outExt[6], int id)
{
  void *inPtr;
  void *outPtr;

  int inExt[6];
  int maxZ;
  this->ComputeInputUpdateExtent(inExt,outExt);

  vtkDebugMacro(<< "Execute: inData = " << inData << ", outData = " << outData);
 
  if (inData == NULL) {
    vtkErrorMacro(<< "Input " << 0 << " must be specified.");
    return;
  }

  inPtr = inData->GetScalarPointerForExtent(inExt);
  outPtr = outData->GetScalarPointerForExtent(outExt);

  maxZ = inExt[5] - inExt[4] + 1; // outExt[5/4] = Relative Maximum/Minimum Slice index  

  // Making sure values are set correctly
  if ((this->StartSlice < 1) || (this->StartSlice > this->EndSlice) || (this->EndSlice > maxZ)) {
    vtkErrorMacro(<< "Start Slice,"<<this->StartSlice<< ", or EndSlice," << this->EndSlice << ", not defined correctly !");
    return;
  }
  
  switch (inData->GetScalarType()) {
    vtkTemplateMacro9(vtkImageEMMarkovExecute, this, inData, (VTK_TT *)(inPtr),inExt, outData, (VTK_TT *)(outPtr),outExt, maxZ,id);
  default:
    vtkErrorMacro(<< "Execute: Unknown ScalarType");
    return;
  }
  // Debug to see what's coming out 
  // outData->Print(cout);
}

void vtkImageEMMarkov::SetComponentExtent(int extent[6])
{
  int idx, modified = 0;
  
  for (idx = 0; idx < 6; ++idx)
    {
    if (this->ComponentExtent[idx] != extent[idx])
      {
      this->ComponentExtent[idx] = extent[idx];
      this->Modified();
      }
    }
  if (modified)
    {
    this->Modified();
    }
}

//----------------------------------------------------------------------------
void vtkImageEMMarkov::SetComponentExtent(int minX, int maxX, int minY, int maxY, int minZ, int maxZ)
{
  int extent[6];
  
  extent[0] = minX;  extent[1] = maxX;
  extent[2] = minY;  extent[3] = maxY;
  extent[4] = minZ;  extent[5] = maxZ;
  this->SetComponentExtent(extent);
}

//----------------------------------------------------------------------------
void vtkImageEMMarkov::GetComponentExtent(int extent[6])
{
  int idx;
  
  for (idx = 0; idx < 6; ++idx)
    {
    extent[idx] = this->ComponentExtent[idx];
    }
}

//----------------------------------------------------------------------------
// We need to create a special SetNumClass, bc whenver NumClasses are set, we 
// also have to set arrays which depend on the number of classes e.g. prob
//----------------------------------------------------------------------------

void vtkImageEMMarkov::SetNumClasses(int NumberOfClasses)
{
  int z,y,x;
  if (this->NumClasses == NumberOfClasses) return;
  this->DeleteVariables();

  if (NumberOfClasses > 0 ) {

    // Create new space for variables
    this->Mu                = new double[NumberOfClasses];
    this->Sigma             = new double[NumberOfClasses];
    this->ClassProbability  = new double[NumberOfClasses];
    this->MarkovMatrix = new double**[6];
    for (z=0; z < 6; z++) {
      this->MarkovMatrix[z] = new double*[NumberOfClasses];
      for (y=0;y < NumberOfClasses; y ++) 
        this->MarkovMatrix[z][y] = new double[NumberOfClasses];
    }
  
    // this->SetComponentExtent(0,NumberOfClasses - 1, 0, NumberOfClasses - 1, 0,5);

    // Set all initial values to -1 and 0
    for (x= 0; x < NumberOfClasses; x++) {
      *this->Mu++                = -1;
      *this->Sigma++             = -1;
      *this->ClassProbability++  = -1;
      for (z=0; z < 6; z++) {
        for (y=0;y < NumberOfClasses; y ++) this->MarkovMatrix[z][y][x] = 0.0;
      }
    }
    this->Mu -= NumberOfClasses; this->Sigma  -= NumberOfClasses;this->ClassProbability  -= NumberOfClasses;
  } else {
    this->Mu = NULL;this->Sigma = NULL;this->ClassProbability = NULL;this->MarkovMatrix = NULL;
  }
  this->NumClasses = NumberOfClasses;
}

//---------------------------------------------------------------------
// Checks of all paramters are declared  
// 1  = eveything correct
// -1 = Number of Classes not defined correctly
// -2 = Not all Mu defined correctly
// -3 = Not all Sigma defined correctly
// -10 = Minimum Brightness value below 0 !

int vtkImageEMMarkov::checkValues(int ImageMin)
{ 
  int i=0;
  if (this->NumClasses < 1 ) {
    vtkErrorMacro(<< "vtkImageEMMarkov:checkValues:  Number of classes smaller 1 !");
    this->Error = -1;
    return -1;
  }
  if (ImageMin < 0 ) {
    vtkErrorMacro(<< "vtkImageEMMarkov:checkValues:  Minimum Brightness Value cannot be smaller than 0 !");
    this->Error = -10;
    return -10;
  }
  while (i < this->NumClasses) {
    i++;
    if (this->Mu[i] < 0) {
      vtkErrorMacro(<<"vtkImageEMMarkov:checkValues:  Mu[" << i <<"] = " << this->Mu[i] << " is not between 0 and 255!");
      this->Error = -2;
      return -2;
    }
    if (this->Sigma[i] < 0)  {
      vtkErrorMacro(<< "vtkImageEMMarkov:checkValues:  Sigma[" << i <<"] = " << this->Sigma[i] << " is not greater than 0!");
      this->Error = -3;
      return -3;
    }
  }
  
  return 1;
}

//----------------------------------------------------------------------------
// Define the arrays 
//----------------------------------------------------------------------------

double vtkImageEMMarkov::GetProbability(int index) {
  if ((index<1) || (index > this->NumClasses)) {
    vtkErrorMacro(<< "Error:vtkImageEMSegm::GetProbability: Index exceeds dimensions : " << index);
    this->Error = -6;
    return -6;
  }
  return this->ClassProbability[index-1];
}

double vtkImageEMMarkov::GetMarkovMatrix(int x, int y, int z) {
  if ((x<1) || (x > this->NumClasses) ||
      (y<1) || (y > this->NumClasses) ||
      (z<1) || (z > 6)) {
    vtkErrorMacro(<< "Error:vtkImageEMSegm::GetMarkovMatrix: Indicees exceeds dimensions :");
    this->Error = -11;
    return -11;
  }
  return this->MarkovMatrix[z-1][y-1][x-1];
}
void vtkImageEMMarkov::SetMu(double mu, int index){
  if ((index<1) || (index > this->NumClasses) ||
      (mu < 0) ){
    vtkErrorMacro(<< "Error:vtkImageEMMarkov::SetMu: Incorrect index (" << index << ") or mu ("<<mu<<") < 0 !");
    this->Error = -7;
    return;
  }
  this->Mu[index-1] = mu;
}

void vtkImageEMMarkov::SetSigma(double sigma, int index){
  if ((index<1) || (index > this->NumClasses) || (sigma < 0)) {
    vtkErrorMacro(<< "Error:vtkImageEMMarkov::SetSigma: Incorrect index (" << index << ") or sigma ("<< sigma <<") < 0 !");
    this->Error = -8;
    return;
  }
  this->Sigma[index-1] = sigma;
}

//----------------------------------------------------------------------------
// Given the input volumes it derives a Markov Matrix

void vtkImageEMMarkov::TrainMarkovMatrix(double ***Image,int Ydim, int Xdim,int ImageMin, int ImageMax) {
  // Check consistency of all veluyes and start with output 
  if (this->checkValues(ImageMin) < 1) return;
  // Nothing to do
  if (this->NumClasses == 0 ) return;
 
  int z,x,y;
  int NumSlices = this->EndSlice - this->StartSlice +1;
  double NormMarkov;

  for (z = 0 ; z < 6; z++)
      this->setMatrix(this->MarkovMatrix[z],this->NumClasses,this->NumClasses, 0.0);
  // Lookup Table for the most likely Class given a certain Image value 
  ImageMax ++;
  double *MaxImageClassProb = new double[ImageMax],*MaxImageClassProbPtr = MaxImageClassProb ;
  for (x = 0; x < ImageMax; x++) (*MaxImageClassProb++) = 0.0;
  MaxImageClassProb = MaxImageClassProbPtr;

  this->CalculateMaxClassProb(MaxImageClassProb,ImageMin,ImageMax);
  
  // Class Assignment for every Pixel
  double *** ClassAssignment = new double**[NumSlices];
  for (z = 0; z < NumSlices ; z++) {
    ClassAssignment[z] = new double *[Ydim];
    for (y=0; y < Ydim ; y++) ClassAssignment[z][y] = new double[Xdim];
  }
  for (z=0; z < NumSlices; z++) {
    for (y=0; y < Ydim; y++) {
      for (x=0; x < Xdim; x++) {
        ClassAssignment[z][y][x] = MaxImageClassProb[(int)Image[z][y][x]];
      }
    }
  }
  // Define Markov Matrix and stationary Prior for classes
  for (z=0; z < NumSlices; z++) {
    for (y=0; y < Ydim; y++) {
      for (x=0; x < Xdim; x++) {
        // This way the matrix is defined correctly : Y-Dim = Current Pixel, X-Dim = Neighbouring Pixel 
        // Relationship between pixel and neighbour to the west
        if (x > 0) this->MarkovMatrix[0][(int)ClassAssignment[z][y][x]][(int)ClassAssignment[z][y][x-1]] += 1;
        // Relationship between pixel and neighbour to the east
        if (x < (Xdim-1)) {
          this->MarkovMatrix[3][(int)ClassAssignment[z][y][x]][(int)ClassAssignment[z][y][x+1]] += 1;
        }
        // Picture is displayed upside down on the screen -> y= 1 -> down, y = Ymax-> up
        // Relationship between pixel and neighbour to the South
        if (y > 0) this->MarkovMatrix[4][int(ClassAssignment[z][y][x])][int(ClassAssignment[z][y-1][x])] += 1;
        // Relationship between pixel and neighbour to the North
        if (y < (Ydim-1)) {
          this->MarkovMatrix[1][int(ClassAssignment[z][y][x])][int(ClassAssignment[z][y+1][x])] += 1;
        }
        // Relationship between pixel and pixel of the previous slice
        if (z > 0) this->MarkovMatrix[5][(int)ClassAssignment[z][y][x]][(int)ClassAssignment[z-1][y][x]] += 1;
        // Relationship between pixel and pixel of the next slice
        if (z < (NumSlices-1)) this->MarkovMatrix[2][(int)ClassAssignment[z][y][x]][(int)ClassAssignment[z+1][y][x]] += 1;
        // Define class probablities/stationary priors
        this->ClassProbability[(int)ClassAssignment[z][y][x]] += 1;
      }
    }
  } 
  // Rows have to be normalized to 1 !
  // Normalize and round up MarkovMatrix and stationary prios
  for (y=0; y < this->NumClasses; y++) {
    this->ClassProbability[y] /= Ydim*Xdim*NumSlices;
    for (z=0; z < 6; z++) {
      NormMarkov = 0.0;

      for (x=0; x < this->NumClasses; x++) 
        NormMarkov +=  this->MarkovMatrix[z][y][x];
       
      if (NormMarkov > 0.0) { 
        for (x=0; x < this->NumClasses; x++) 
          this->MarkovMatrix[z][y][x] = int(this->MarkovMatrix[z][y][x]/NormMarkov*1000+0.5)/1000.0;
       } else 
          this->MarkovMatrix[z][y][y] =  1.0;
    }
  }
  delete[] MaxImageClassProb;

  for (z = 0; z < NumSlices ; z++) {
    for (y=0; y < Ydim ; y++) delete[] ClassAssignment[z][y];
    delete[] ClassAssignment[z];
  }
  delete[] ClassAssignment;
}

// Calculates the maximum likely class for each image value
void vtkImageEMMarkov::CalculateMaxClassProb(double *MaxClass, int &ImageMin,int &ImageMax) {
  int i,c; 
  double prob;
  for (i = ImageMin; i < ImageMax; i++) {
    prob = EMMarkovGauss(i,this->Mu[0],this->Sigma[0]);
    MaxClass[i] = 0;
    for (c = 1; c < this->NumClasses; c++) {
      if (prob < EMMarkovGauss(i,this->Mu[c],this->Sigma[c])) {
        prob = EMMarkovGauss(i,this->Mu[c],this->Sigma[c]);
        MaxClass[i] = c;
      } 
    }
  }
} 

//----------------------------------------
// Special Vector and Matrix functions

void vtkImageEMMarkov::DeleteVariables() {
  int z,y;
  if (this->NumClasses > 0 ){
    delete[] this->Mu; 
    delete[] this->Sigma;
    delete[] this->ClassProbability;

    for (z=0; z < 6; z++){ 
      for (y=0; y < this->NumClasses; y++)
        delete[] this->MarkovMatrix[z][y];
      delete[] this->MarkovMatrix[z];
    }
    delete[] this->MarkovMatrix;

    this->Mu = NULL;this->Sigma = NULL;this->ClassProbability = NULL;this->MarkovMatrix = NULL;
  } 
}

// Setting up Test Matrix to test 
// test = 1 => Creates horizontal stripped matrix with image values from 0 to 255 
// test = 2 => Creates squared matrix with image values from 0 to 255 
// test = 3 => Creates vertical stripped matrix with image values from 0 to 255 
// division = the number of colors that the pixture should be devided in 
//            if pixel == -1 => the number of divisions of the picture
// pixel = pixel length of one divison

void vtkImageEMMarkov::setMatrix3DTest(double ***mat3D,int maxZ, int maxY, int maxX, int test,int division,int pixel){
  int z;
  if (division < 2) {
    for (z = 0 ; z <maxZ; z++)
      this->setMatrix(mat3D[z],maxY,maxX, 0.0);
    return;
  }
  for (z =0 ; z < maxZ; z++) 
    this->setMatrixTest(mat3D[z],maxY, maxX, test,division,pixel,z+1);
}
void vtkImageEMMarkov::setMatrixTest(double **mat, int maxY, int maxX, int test,int division, int pixel, int offset){
  int y;
  double YScale = 0;
  if (division < 2) {
      this->setMatrix(mat,maxY,maxX,0.0);
      return;
  }

  if (pixel > 0) YScale = double(pixel);
  else YScale = maxY / double(division);

  int xoffset= offset % division; 
  int offsetadd = (division >2 ? 2:1);
  maxY++;
  for (y=1; y < maxY; y++) {
    if ((test < 3) && (int(fmod(y,YScale)) == 1) && (int(fmod(y-1,YScale)) == 0)) xoffset = (xoffset + offsetadd) % division;
    this->setVectorTest(mat[y-1],maxX,test,division,pixel,xoffset);
  }
}

void vtkImageEMMarkov::setMatrix(double **mat, int maxY, int maxX, double val){
  int y,x;
  for (y = 0; y < maxY; y++){ 
    for (x = 0; x < maxX; x++) mat[y][x] = val;
  }
}

void vtkImageEMMarkov::setVectorTest(double *vec, int maxX,int test,int division,int pixel, int offset){
  int x;
  int ImgColor = 0;
  if (division < 2) {
    for (x = 0; x < maxX; x++) vec[x] = 0;
    return;
  }
  double ImgScale = 255/double(division-1);
  double XScale ;

  if (pixel > 0) XScale = double(pixel);
  else XScale = double(maxX) /double(division);

  if (test < 2) ImgColor = int(ImgScale*(offset%division));
  maxX++;
  for (x =1 ; x < maxX; x++) {
    if (test> 1) ImgColor = int(ImgScale*((int((x-1)/XScale)+offset)%division));
    vec[x-1] = ImgColor;
  }
}


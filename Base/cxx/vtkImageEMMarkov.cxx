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
#include "vtkImageEMMarkov.h"
#include "vtkObjectFactory.h"

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

}

//----------------------------------------------------------------------------
void vtkImageEMMarkov::PrintSelf(ostream& os)
{
}

// To chage anything about output us this executed before Thread

//----------------------------------------------------------------------------
// void vtkImageEMMarkov::ExecuteInformation(vtkImageData *vtkNotUsed(input), 
//				    vtkImageData *output) {
  
  // output->SetWholeExtent(this->ComponentExtent); // size of output
  //output->SetOrigin(0.0,0.0,0.0);
// output->SetSpacing(1.0, 1.0, 1.0);
//  output->SetNumberOfScalarComponents(1);
// output->SetScalarType(VTK_DOUBLE);      // type of output float, integer 
  // cout << " Got to execute information" << endl;
// }





//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageEMMarkovExecute(vtkImageEMMarkov *self,vtkImageData *in1Data, T *in1Ptr,
				  vtkImageData *outData, T *outPtr,int outExt[6], int id)
{
  int idxR, idxY, idxZ;
  int maxY, maxZ;
  int inIncX, inIncY, inIncZ;
  int outIncX, outIncY, outIncZ;
  int rowLength;
  int StartSlice = self->get_StartSlice();
  int EndSlice = self->get_EndSlice();
  int StartEndSlice; 
  
  // find the region to loop over
  rowLength = (outExt[1] - outExt[0]+1)*in1Data->GetNumberOfScalarComponents();
  maxY = outExt[3] - outExt[2] + 1; // outExt[3/2] = Relative Maximum/Minimum Y index  
  maxZ = outExt[5] - outExt[4] + 1; // outExt[5/4] = Relative Maximum/Minimum Slice index  

  // Making sure values are set correctly
  if (1 > StartSlice) StartSlice = 1;
  if (maxZ < EndSlice) EndSlice = maxZ; 
  else if (StartSlice > EndSlice) EndSlice = StartSlice -1;  
  StartEndSlice = EndSlice - StartSlice + 1;

  // Get increments to march through data 
  in1Data->GetContinuousIncrements(outExt, inIncX, inIncY, inIncZ);
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);

  // The Slices of the images 
  vtkImageEMMatrix3D Volume(StartEndSlice,maxY,rowLength);    
 
  // Loop through input pixels 

  // 1.) Read thorugh beginning slides we do not want to read through for input and output
  for (idxZ = 1; idxZ < StartSlice; idxZ++) {
    for (idxY = 0; idxY < maxY; idxY++) {
      for (idxR = 0; idxR < rowLength; idxR++) {
	*outPtr = (T) (*in1Ptr);
	in1Ptr++;
      }
      in1Ptr += inIncY;
    }
    in1Ptr += inIncZ;
  }

  // 2.) Read thorugh the slides that should be segmented 
  StartEndSlice ++;
  maxY ++;
  rowLength ++;
  int Max =  ((int)*in1Ptr);
  int Min = ((int)*in1Ptr);

  if (self->get_ImgTestNo() < 1) {
    for (idxZ = 1; idxZ < StartEndSlice ; idxZ++) {
      for (idxY = 1; idxY < maxY; idxY++) {
	for (idxR = 1; idxR < rowLength; idxR++) {
	  // Copied from  vtkMrmlSlicer::GetBackPixel
	  // in1data->GetWholeExtent(ext);
	  // in1Data->GetPointData()->GetScalars()->GetScalar(y*(ext[1]-ext[0]+1)+x);
	  // Pixel operaton
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
    Volume.setMatrix3DTest(self->get_ImgTestNo(),self->get_ImgTestDivision(),self->get_ImgTestPixel());
    Max = 255;
    Min = 0;
  }
  // 3.) Run Algorith to find out Matrix
  // Debugging 
  self->TrainMarkovMatrix(Volume,Min,Max);

// 4.) Write result in outPtr
  //  for (idxZ = 1; idxZ < 7; idxZ++) {
  //   for (idxY = 1; idxY <= self->get_NumClasses(); idxY++) {
   //    for (idxR = 1; idxR <= self->get_NumClasses(); idxR++) {
  //	*outPtr = (T) (self->MarkovMatrix[idxZ][idxY][idxR]);
  //	outPtr++;
  //  }
  //  outPtr++;
  //  outPtr += outIncY;
  //}
  //   outPtr += outIncZ;
  // }
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
   
  vtkDebugMacro(<< "Execute: inData = " << inData 
		<< ", outData = " << outData);
  

  if (inData == NULL)
    {
    vtkErrorMacro(<< "Input " << 0 << " must be specified.");
    return;
    }

  inPtr = inData->GetScalarPointerForExtent(outExt);
  outPtr = outData->GetScalarPointerForExtent(outExt);
  
  switch (inData->GetScalarType()) {
    vtkTemplateMacro7(vtkImageEMMarkovExecute, this, inData, (VTK_TT *)(inPtr), 
                      outData, (VTK_TT *)(outPtr),outExt, id);
  default:
    vtkErrorMacro(<< "Execute: Unknown ScalarType");
    return;
  }
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
void vtkImageEMMarkov::SetComponentExtent(int minX, int maxX, 
					    int minY, int maxY,
					    int minZ, int maxZ)
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
  this->NumClasses = NumberOfClasses;
  // Delete Old instances if they existed
  this->Mu.Resize(this->NumClasses,-1.0);
  this->Sigma.Resize(this->NumClasses,-1.0); 
  this->ClassProbability.Resize(this->NumClasses,0.0);
  this->MarkovMatrix.Resize(6,this->NumClasses,this->NumClasses,0.0);  
  // this->SetComponentExtent(0,NumberOfClasses - 1, 0, NumberOfClasses - 1, 0,5);
}
//---------------------------------------------------------------------
// Checks of all paramters are declared  
// 1  = eveything correct
// -1 = Dimension of Volume not correct
// -2 = Not all Mu defined correctly
// -3 = Not all Sigma defined correctly
// -4 = Number of Classes not defined correctly

int vtkImageEMMarkov::checkValues(vtkImageEMMatrix3D Volume)
{
  int i=0,j,k;
  if (Volume.get_dim(1) != (this->EndSlice - this->StartSlice + 1)) {
    vtkErrorMacro(<< "vtkImageEMMarkov:checkValues:  Volume does not have the right dimension !");
    this->Error = -1;
    return -1;
  }
  if (this->NumClasses < 1 ) {
    vtkErrorMacro(<< "vtkImageEMMarkov:checkValues:  Number of classes smaller 1 !");
    this->Error = -10;
    return -4;
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
  return this->ClassProbability[index];
}

double vtkImageEMMarkov::GetMarkovMatrix(int x, int y, int z) {
  if ((x<1) || (x > this->NumClasses) ||
      (y<1) || (y > this->NumClasses) ||
      (z<1) || (z > 6)) {
    vtkErrorMacro(<< "Error:vtkImageEMSegm::GetMarkovMatrix: Indicees exceeds dimensions :");
    this->Error = -11;
    return -11;
  }
  return this->MarkovMatrix[z][y][x];
}
void vtkImageEMMarkov::SetMu(double mu, int index){
  if ((index<1) || (index > this->NumClasses) ||
      (mu < 0) ){
    vtkErrorMacro(<< "Error:vtkImageEMMarkov::SetMu: Incorrect index (" << index << ") or mu ("<<mu<<") < 0 !");
    this->Error = -7;
    return;
  }
  this->Mu[index] = mu;
}

void vtkImageEMMarkov::SetSigma(double sigma, int index){
  if ((index<1) || (index > this->NumClasses) || (sigma < 0)) {
    vtkErrorMacro(<< "Error:vtkImageEMMarkov::SetSigma: Incorrect index (" << index << ") or sigma ("<< sigma <<") < 0 !");
    this->Error = -8;
    return;
  }
  this->Sigma[index] = sigma;
}

//----------------------------------------------------------------------------
// Given the input volumes it derives a Markov Matrix

void vtkImageEMMarkov::TrainMarkovMatrix(vtkImageEMMatrix3D Image,int ImageMin, int ImageMax) {

  // Check consistency of all veluyes and start with output 
  if (this->checkValues(Image) < 1) return;
  // Nothing to do
  if (this->NumClasses == 0 ) return;
 
  int z,x,y;
  int NumSlices = this->EndSlice - this->StartSlice +1;
  int NumSlicesPlus = NumSlices +1;
  int NumClassesPlus = this->NumClasses+1;
  int Xdim = Image.get_dim(3);
  int Ydim = Image.get_dim(2);
  int Xmax = Xdim + 1;
  int Ymax = Ydim + 1;
  double NormMarkov;

  this->MarkovMatrix.setMatrix3D(0);

  // Lookup Table for the most likely Class given a certain Image value 
  vtkImageEMVector MaxImageClassProb(ImageMax,0.0);
  this->CalculateMaxClassProb(MaxImageClassProb,ImageMin,ImageMax);
  
  // Class Assignment for every Pixel
  // Be careful MaxImageClassProb[0] is also used here -> so do not use any vector functions , bc 
  // bc they all defined for index > 0
  vtkImageEMMatrix3D ClassAssignment(NumSlices,Ydim,Xdim);
  for (z=1; z < NumSlicesPlus; z++) {
    for (y=1; y < Ymax; y++) {
      for (x=1; x < Xmax; x++) {
	ClassAssignment[z][y][x] = MaxImageClassProb[(int)Image[z][y][x]];
      }
    }
  }

  // Define Markov Matrix and stationary Prior for classes
  for (z=1; z < NumSlicesPlus; z++) {
    for (y=1; y < Ymax; y++) {
      for (x=1; x < Xmax; x++) {
	// This way the matrix is defined correctly : Y-Dim = Current Pixel, X-Dim = Neighbouring Pixel 
        // Relationship between pixel and neighbour to the west
	if (x > 1) this->MarkovMatrix[1][(int)ClassAssignment[z][y][x]][(int)ClassAssignment[z][y][x-1]] += 1;
        // Relationship between pixel and neighbour to the east
	if (x < Xdim) {
	  this->MarkovMatrix[4][(int)ClassAssignment[z][y][x]][(int)ClassAssignment[z][y][x+1]] += 1;
	}
        // Picture is displayed upside down on the screen -> y= 1 -> down, y = Ymax-> up
        // Relationship between pixel and neighbour to the South
	if (y > 1) this->MarkovMatrix[5][int(ClassAssignment[z][y][x])][int(ClassAssignment[z][y-1][x])] += 1;
        // Relationship between pixel and neighbour to the North
	if (y < Ydim) {
	  this->MarkovMatrix[2][int(ClassAssignment[z][y][x])][int(ClassAssignment[z][y+1][x])] += 1;
	}
        // Relationship between pixel and pixel of the previous slice
	if (z > 1) this->MarkovMatrix[6][(int)ClassAssignment[z][y][x]][(int)ClassAssignment[z-1][y][x]] += 1;
        // Relationship between pixel and pixel of the next slice
	if (z < NumSlices) this->MarkovMatrix[3][(int)ClassAssignment[z][y][x]][(int)ClassAssignment[z+1][y][x]] += 1;
	// Define class probablities/stationary priors
	this->ClassProbability[(int)ClassAssignment[z][y][x]] += 1;
      }
    }
  } 
  // Rows have to be normalized to 1 !
  // Normalize and round up MarkovMatrix and stationary prios
  for (y=1; y < NumClassesPlus; y++) {
    this->ClassProbability[y] /= Ydim*Xdim*NumSlices;
    for (z=1; z < 7; z++) {
      NormMarkov = 0.0;

      for (x=1; x < NumClassesPlus; x++) 
	NormMarkov +=  this->MarkovMatrix[z][y][x];
       
      if (NormMarkov > 0.0) { 
	for (x=1; x < NumClassesPlus; x++) 
	  this->MarkovMatrix[z][y][x] = int(this->MarkovMatrix[z][y][x]/NormMarkov*1000+0.5)/1000.0;
      } else 
	  this->MarkovMatrix[z][y][y] =  1.0;
    }
  }
}
      // Calculates the maximum likely class for each image value
void vtkImageEMMarkov::CalculateMaxClassProb(vtkImageEMVector & MaxClass, int &ImageMin,int &ImageMax) {
  int i,c; 
  int iMax =ImageMax + 1;
  int cMax = this->NumClasses + 1;
  double prob;
  for (i = ImageMin; i < iMax; i++) {
    prob = Gauss(i,this->Mu[1],this->Sigma[1]);
    MaxClass[i] = 1;
    for (c = 2; c < cMax; c++) {
      if (prob < this->Gauss(i,this->Mu[c],this->Sigma[c])) {
	prob = this->Gauss(i,this->Mu[c],this->Sigma[c]);
	MaxClass[i] = c;
      } 
    }
  }
} 

double vtkImageEMMarkov::Gauss(int x,double m,double s) { 
  if (s > 0) return (1 / (sqrt(2*3.14159265358979)*s)*exp(-(x-m)*(x-m)/(2*pow(s,2))));
  return (m == x ? 1e20:0);
} 

//------------------------------------------------------------------------------
// ---------------------------------------------------------   
// Class: vtkImageEMMatrix3D
// ---------------------------------------------------------  
void vtkImageEMMatrix3D::allocate(int z=0, int y = 0, int x = 0 ){
  this->dimz = z;
  int size = this->dimz+1;
  if (z > 0) { 
    this->mat3D = new vtkImageEMMatrix[size];
    if (!this->mat3D) {
      cout << "Did not get any Memory" << endl;
      exit(1);
    }
    if (y > 0)
      for (int i=1; i < size; i++) this->mat3D[i].Resize(y,x);
  }
  else this->mat3D = NULL;
}

void vtkImageEMMatrix3D::allocate(int z, int y, int x, double val){
  this->dimz = z;
  int size = this->dimz+1;
  if (z > 0) { 
    this->mat3D = new vtkImageEMMatrix[size];
    if (!this->mat3D) {
      cout << "Did not get any Memory" << endl;
      exit(1);
    }
    if (y > 0)
      for (int i=1; i < size; i++) this->mat3D[i].Resize(y,x,val);
  }
  else this->mat3D = NULL;
}

void vtkImageEMMatrix3D::deallocate (){
  if (this->dimz > 0) {
    //int size = this->dimz+1;
    //for (int i = 1; i < size;i++)
    //  delete (*this->mat3D)[i];
    this->dimz = 0;
    delete[] this->mat3D;
    this->mat3D = NULL;
  }
}

vtkImageEMMatrix3D::vtkImageEMMatrix3D(const vtkImageEMMatrix3D & in){  
  this->allocate(in.dimz,0,0);
  int size = this->dimz +1;
  for (int i=1; i < size; i++){
    this->mat3D[i] = in.mat3D[i];
  }
}

vtkImageEMMatrix& vtkImageEMMatrix3D::operator [] (int z) {return this->mat3D[z];}
const vtkImageEMMatrix& vtkImageEMMatrix3D::operator [] (int i) const { return this->mat3D[i]; }

void vtkImageEMMatrix3D::Resize(int z=0,int y = 0, int x = 0) {
  if (z == this->dimz) {
    if (z== 0) return ;
    if ((y == this->get_dim(2)) && (x == this->get_dim(3))) return;
  }
  this->deallocate();
  this->allocate(z,y,x);
}

void vtkImageEMMatrix3D::Resize(int z,int y, int x, double val) {
  if (z == this->dimz) {
    if (z== 0) return ;
    if ((y == this->get_dim(2)) && (x == this->get_dim(3))) {
      for (int i=1; i <= z; i++) this->mat3D[i].setMatrix(val);
      return;
    }
  } 
  this->deallocate();
  this->allocate(z,y,x,val);
}
int vtkImageEMMatrix3D::get_dim(int dim) {
  if ((dim == 3) || (dim==2)) {
    if (dimz == 0) return 0;
    else return this->mat3D[1].get_dim(dim-1);
  }
  if (dim == 1) return this->dimz;
  return -1; 
}

void vtkImageEMMatrix3D::PrintMatrix3D(int zMax = -1,int yMax = -1,int xMax = -1) {
  int Max = (zMax > -1 ? (zMax > this->dimz ? this->dimz : zMax) : this->dimz)  + 1;
  for (int z = 1; z < Max; z++)
    this->mat3D[z].PrintMatrix(yMax,xMax);
}

void vtkImageEMMatrix3D::setMatrix3D(double val){
  int size = this->dimz+1;
  for (int i =1 ; i < size; i++) 
    this->mat3D[i].setMatrix(val);
}
 
void vtkImageEMMatrix3D::Reshape(vtkImageEMVector v) {
  int dimY= this->get_dim(2),dimX= this->get_dim(3),dimZ= this->get_dim(1);
  int i,j,k, index;
  if (v.get_len() != (dimY*dimX*dimZ))
    vtkImageEMError::vtkImageEMError("vtkImageEMMatrix3D:Reshape dimension of input vector and 3D Matrix do not match!");
  for (k=1; k<= dimZ; k++) {
    index = (k-1)*dimY*dimX;
    for (i=1; i <= dimX; i++) {
      for (j=1; j <= dimY; j++) {
        this->mat3D[k][j][i] = v[index+j];
      }
      index +=dimY;
    }
  }
}

//  Smoothes  3D-Matrix
// w(k) = sum(u(j)*v(k+1-j))
// returns Matrix of size r_m
void vtkImageEMMatrix3D::smoothConv(vtkImageEMMatrix3D U,vtkImageEMVector v) {
  if ((this->get_dim(1) != U.get_dim(1))||
      (this->get_dim(2) != U.get_dim(2)) || 
      (this->get_dim(3) != U.get_dim(3))) 
         vtkImageEMError::vtkImageEMError("vtkImageEMMatrix3D:smoothConv : Dimensions of 3D Matrixs do not match");

  int i, iMax = this->get_dim(1) + 1;
  vtkImageEMMatrix3D resultY(U.get_dim(1),U.get_dim(2),U.get_dim(3)), resultX(U.get_dim(1),U.get_dim(2),U.get_dim(3));            
  // First: convolut in Y Direction 
  for (i = 1; i < iMax; i++){
    resultY.mat3D[i].conv(U[i],v); 
  }
  // Second: convolut in X Direction 
  for (i = 1; i < iMax; i++){
    resultX.mat3D[i].convT(resultY[i],v); 
  }
  // Third: in Z direction
  this->conv(resultX,v);
}

// Convolution and polynomial multiplication . 
// This is assuming u and 'this' have the same dimension
void vtkImageEMMatrix3D::conv(vtkImageEMMatrix3D U,vtkImageEMVector v) {
  if ((this->get_dim(1) != U.get_dim(1))||
      (this->get_dim(2) != U.get_dim(2)) || 
      (this->get_dim(3) != U.get_dim(3))) 
         vtkImageEMError::vtkImageEMError("vtkImageEMMatrix3D:conv : Dimensions of 3D Matrixs do not match");
   
  int UDimX = U.get_dim(3)+1,
      UDimY = U.get_dim(2)+1,
      UDimZ = U.get_dim(1),
      vLen = v.get_len();
  int stump = vLen /2;
  int k,j,jMin,jMax,x,y;
  int kMax = UDimZ + stump +1;
  for (k = stump + 1; k <  kMax; k++) {
    this->mat3D[k-stump].SetEqual(0);
    jMin = (1 > k + 1 - vLen ? 1 : k + 1 - vLen ); //  max(1,k+1-vLen):
    jMax = (k < UDimZ ? k : UDimZ)+1;              //  min(k,uLen) 

    // this->mat3D[k-stump] += U[j]*v[k+1-j];
    for (j=jMin; j < jMax; j++) {
      for (y = 1; y < UDimY; y++) {
	for (x = 1; x < UDimX; x++) {
	  this->mat3D[k-stump][y][x] += U[j][y][x]*v[k+1-j];
	}
      }
    }
  }  
}

// Writes 3D Matrix to file in Matlab format if name is specified otherwise just 
// writes the values in the file
void vtkImageEMMatrix3D::WriteMatrix3DToFile (FILE *f,char *name) const
{
  int zMax = this->dimz +1;
  char mname[40];
  for (int z = 1; z < zMax; z++ ) {
    if (name != NULL) { 
      sprintf(mname,"%s(:,:,%d)",name,z);
      this->mat3D[z].WriteMatrixToFile(f,mname); 
    } else {this->mat3D[z].WriteMatrixToFile(f,NULL); }

  }
}


// Setting up Test Matrix to test 
// test = 1 => Creates horizontal stripped matrix with image values from 0 to 255 
// test = 2 => Creates squared matrix with image values from 0 to 255 
// test = 3 => Creates vertical stripped matrix with image values from 0 to 255 
// division = the number of colors that the pixture should be devided in 
//            if pixel == -1 => the number of divisions of the picture
// pixel = pixel length of one divison

void vtkImageEMMatrix3D::setMatrix3DTest(int test,int division,int pixel){
  int Zsize = this->dimz+1;
  int z;
  if (division < 2) {
    this->setMatrix3D(0);
    return;
  }
  for (z =1 ; z < Zsize; z++) 
    this->mat3D[z].setMatrixTest(test,division,pixel,z);
}

// ---------------------------------------------------------   
// Class: vtkImageEMMatrix
// ---------------------------------------------------------   

void vtkImageEMMatrix::allocate(int y=0,int x=0){
  this->dimy = y; 
  int size = y+1;
  if (y > 0) {
    this->mat = new vtkImageEMVector[size];
    if (!this->mat) {
      cout << "Did not get any Memory" << endl;
      exit(1);
    }
    if (x > 0) {
      for (int i=1; i <size; i++) this->mat[i].Resize(x);
    }
  } else {
    this->mat = NULL;
  }
}
void vtkImageEMMatrix::allocate(int y,int x,double val){
  this->dimy = y; 
  int size = y+1;
  if (y > 0) {
    this->mat = new vtkImageEMVector[size];
    if (!this->mat) {
      cout << "Did not get any Memory" << endl;
      exit(1);
    }
    if (x > 0) {
      for (int i=1; i <size; i++) this->mat[i].Resize(x,val);
    }
  } else {
    this->mat = NULL;
  }
}

void vtkImageEMMatrix::deallocate(){
  if (this->dimy > 0) {
    int size = this->dimy +1; 
    //for (int i = 1; i < size;i++) {
    //  delete this->mat[i];
    //}
    delete[] this->mat;
    this->mat = NULL;
    this->dimy = 0;
  }
}

vtkImageEMMatrix::vtkImageEMMatrix(const vtkImageEMMatrix & in){  
  this->allocate(in.dimy,0);
  int size = this->dimy +1;
  for (int i=1; i < size; i++){
    this->mat[i] = in.mat[i];
  }
}
  
vtkImageEMVector& vtkImageEMMatrix::operator [] (int y) {return this->mat[y];}
const vtkImageEMVector& vtkImageEMMatrix::operator [] (int i) const { return this->mat[i]; }

vtkImageEMMatrix& vtkImageEMMatrix::operator = (const vtkImageEMMatrix& Equal) {
  if (Equal.mat == this->mat) return *this;
  if (Equal.dimy < 1) {
    this->Resize(0,0);
    return *this;    
  }
   
  this->Resize(Equal.dimy,Equal.mat[1].get_len()); 
   
  int x,y, yMax = this->get_dim(1)+1, xMax = this->get_dim(2)+1;
  for (y = 1; y < yMax; y++) {
    for (x = 1; x < xMax; x++) {
      this->mat[y][x] = Equal[y][x];
    }
  }
  return *this;    
}

int vtkImageEMMatrix::get_dim(int dim) {
  if (dim == 1) return dimy;
  else  
    if (dim == 2) {
      if (this->mat !=NULL) return this->mat[1].get_len();
      else return 0;
    }
  return -1; 
}

void vtkImageEMMatrix::setMatrix(double val){
  int size =  this->dimy+1;
  for (int i=1; i < size; i++)
      this->mat[i].setVector(val);
}

void vtkImageEMMatrix::Resize(int y, int x, double val) {
  if (y == this->dimy) {
    if (y == 0) return ;
    if (x == this->get_dim(2)) {
      for (int i=1; i <= y; i++) this->mat[i].setVector(val);
      return;
    }
  }
  this->deallocate(); 
  this->allocate(y,x,val);
}

void vtkImageEMMatrix::Resize(int y = 0, int x = 0) {
  if (y == this->dimy) {
    if (y== 0) return ;
    if (x == this->get_dim(2)) return;
  }
  this->deallocate();
  this->allocate(y,x);
}

void vtkImageEMMatrix::PrintMatrix(int yMax = -1,int xMax = -1) {
  int Max = (yMax > -1 ? (yMax > this->dimy ? this->dimy : yMax) : this->dimy)  + 1;
  for (int y = 1; y < Max; y++)
    this->mat[y].PrintVector(xMax);

  cout << endl;
}
  
// Sets column 'col' of this->mat to vector v
void vtkImageEMMatrix::ColMatrix(vtkImageEMVector v, int col) {
  if ((this->dimy != v.get_len()) ||
      (this->get_dim(2) < col)) 
         vtkImageEMError::vtkImageEMError("vtkImageEMMatrix:ColMatrix: Dimensions of input do not match matrix  !");
  int iMax = this->dimy+1;
  for (int i=1; i < iMax; i++) this->mat[i][col] = v[i];
} 

void vtkImageEMMatrix::Transpose(vtkImageEMMatrix Trans) {
  if ((this->get_dim(2) != Trans.get_dim(1))||
      (this->get_dim(1) != Trans.get_dim(2))) 
         vtkImageEMError::vtkImageEMError("vtkImageEMMatrix:Transpose: Dimensions of input do not match matrix  !");

  int x,y, yMax = this->get_dim(1)+1, xMax = this->get_dim(2)+1;
  for (y = 1; y < yMax; y++) {
    for (x = 1; x < xMax; x++) {
      this->mat[y][x] = Trans[x][y];
    }
  } 
}

void vtkImageEMMatrix::SetEqual(double value) {
  int x,y, yMax = this->get_dim(1)+1, xMax = this->get_dim(2)+1;
  for (y = 1; y < yMax; y++) {
    for (x = 1; x < xMax; x++) {
      this->mat[y][x] = value;
    }
  }    
}

// Convolution and polynomial multiplication . 
// This is assuming u and 'this' have the same dimension

void vtkImageEMMatrix::conv(vtkImageEMMatrix U, vtkImageEMVector v) {
  if ((this->get_dim(1) != U.get_dim(1))||
      (this->get_dim(2) != U.get_dim(2))) 
         vtkImageEMError::vtkImageEMError("vtkImageEMMatrix::conv: Dimensions of input do not match Matrix  !");
  int iMax = U.get_dim(2)+1,
      i;
  vtkImageEMMatrix Utrans(U.get_dim(2),U.get_dim(1));
  Utrans.Transpose(U); // => Utrans[i] represents the i column of U;    
  vtkImageEMVector result(U.get_dim(1));       

  for (i = 1; i < iMax; i++) {
     result.conv(Utrans[i],v);  // Use the i-th Rows of Utrans;
     this->ColMatrix(result,i); // Write result to this->mat as i-th column
  }
} 

// Same just v is a row vector instead of column one
// We use the following equation :
// conv(U,v) = conv(U',v')' => conv(U,v') = conv(U',v)';
void vtkImageEMMatrix::convT(vtkImageEMMatrix U, vtkImageEMVector v) {
  if ((this->get_dim(1) != U.get_dim(1))||
      (this->get_dim(2) != U.get_dim(2))) 
         vtkImageEMError::vtkImageEMError("vtkImageEMMatrix::conv: Dimensions of input do not match Matrix  !");

  int iMax = U.get_dim(1)+1,
      i;

  // Use the i-th Rows of U = ith Column of U';
  // write it to the i-th Row of 'this' => Transpose again
  for (i = 1; i < iMax; i++) {
     this->mat[i].conv(U[i],v);  
  }
} 

// Opens up a new file and writes down result in the file
void vtkImageEMMatrix::WriteMatrixToFile (char *filename,char *varname) const {
  int appendFlag = 0;
  FILE *f = (strcmp(filename,"-"))?fopen(filename,((appendFlag)?"a":"w")):stdout;
  if ( f == NULL ) {
    cerr << "Could not open file " << filename << "\n";
    return;
  }
  this->WriteMatrixToFile(f,varname);
  fflush(f);
  fclose(f);
}

// Writes Matrix to file in Matlab format if name is specified otherwise just 
// writes the values in the file

void vtkImageEMMatrix::WriteMatrixToFile (FILE *f,char *name) const
{
  int yMax = this->dimy +1;
  if (name != NULL) fprintf(f,"%s = [", name);
  for (int y = 1; y < yMax; y++ ) {
    this->mat[y].WriteVectorToFile(f,NULL); 
    if (y < this->dimy) fprintf(f,";\n");
  }
  if (name != NULL) fprintf(f,"];\n", name);
  fprintf(f,"\n", name);
}



// Setting up Test Matrix to test 
// test = 1 => Creates horizontal stripped matrix with image values from 0 to 255 
// test = 2 => Creates squared matrix with image values from 0 to 255 
// test = 3 => Creates vertical stripped matrix with image values from 0 to 255 
// division = the number of colors that the pixture should be devided in 
//            if pixel == -1 => the number of divisions of the picture
// pixel = pixel length of one divison

void vtkImageEMMatrix::setMatrixTest(int test,int division, int pixel, int offset){
  int Ysize = this->dimy+1;
  int y;
  double YScale;
  if (division < 2) {
      this->setMatrix(0);
      return;
  }

  if (pixel > 0) YScale = double(pixel);
  else YScale = this->dimy / double(division);

  int xoffset= offset % division; 
  int offsetadd = (division >2 ? 2:1);
  for (y=1; y < Ysize; y++) {
    if ((test < 3) && (int(fmod(y,YScale)) == 1) && (int(fmod(y-1,YScale)) == 0)) xoffset = (xoffset + offsetadd) % division;
    this->mat[y].setVectorTest(test,division,pixel,xoffset);
  }
}

// ---------------------------------------------------------   
// Class: vtkImageEMVector
// ---------------------------------------------------------   

void vtkImageEMVector::allocate(int dim=0){
  this->len = dim;
  if (dim > 0) {
    this->vec = new double[dim+1];
    if (!this->vec) {
      cout << "Did not get any Memory" << endl;
      exit(1);
    }
  } else {
    this->vec = NULL;
  }
}

void vtkImageEMVector::allocate(int dim,double val){
  this->len = dim;
  if (dim > 0) {
    this->vec = new double[dim+1];
    if (!this->vec) {
      cout << "Did not get any Memory" << endl;
      exit(1);
    }
    for (int i= 1; i <= dim; i++)
      this->vec[i] = val;
  } else {
    this->vec = NULL;
  }
}

vtkImageEMVector::vtkImageEMVector(const vtkImageEMVector & in){  
  this->allocate(in.len);
  for (int i=1; i <= this->len; i++){
    this->vec[i] = in.vec[i];
  }
}

void vtkImageEMVector::deallocate() {
    if (this->len > 0 ){
      delete[] this->vec; 
      this->len = 0;
      this->vec = NULL;
    } 
}

vtkImageEMVector& vtkImageEMVector::operator = ( const vtkImageEMVector& v) { 
   if ( this->vec == v.vec) return *this;
   this->Resize(v.len);
   int size = v.len+1;  
   for ( int i = 0; i < size; i++ )
     this->vec[i] = v.vec[i];
   return *this;
}

void vtkImageEMVector::setVector(double val){
  int size = this->get_len()+1;
  for (int i=1; i < size; i++){
    this->vec[i] = val;
  }
}

void vtkImageEMVector::setVector(vtkImageEMMatrix v,int start, int end, int column) {
  int iMax = end - start +1;
  if ((start < 0) || (end > v.get_dim(1)) || (iMax > this->len)) return;
  iMax ++;
  start --;
  for (int i = 1; i < iMax; i++)
    this->vec[i] = v[start+i][column];
}

void vtkImageEMVector::Resize(int dim=0) {
  if (dim != this->len) {
    this->deallocate();
    this->allocate(dim);
  }
}
 
void vtkImageEMVector::Resize(int dim, double val) {
  if (dim != this->len) {
    this->deallocate();
    this->allocate(dim,val);
  } else {
    for (int i= 1; i <= dim; i++)
      this->vec[i] = val;
  }
}

// void vtkImageEMVector::Resize(const vtkImageEMVector & input) {
//  if (this->len != input.len) {
//    this->deallocate();
//    this->allocate(input.len);
//  }
//  for (int i= 1; i <= input.len; i++)
//    this->vec[i] = input.vec[i];
// } 


void vtkImageEMVector::Reshape(vtkImageEMMatrix3D im) {
  int dimY= im.get_dim(2),dimX= im.get_dim(3),dimZ= im.get_dim(1);
  int i,j,k, index;
  if (len != (dimY*dimX*dimZ)) 
     vtkImageEMError::vtkImageEMError("vtkImageEMVector::Reshape: Dimension of input does not match vector!");

  for (k=1; k<= dimZ; k++) {
    index = (k-1)*dimY*dimX;
    for (i=1; i <= dimX; i++) {
      for (j=1; j <= dimY; j++) {
         this->vec[index+j] = im[k][j][i];
      }
      index +=dimY;
    }
  }
}

void vtkImageEMVector::Reshape(vtkImageEMMatrix im) {
  int dimY= im.get_dim(1), dimYPlus = dimY+1,
      dimX= im.get_dim(2), dimXPlus = dimX+1;
  int i,j, index = 0;
  if (len != (dimY*dimX)) 
     vtkImageEMError::vtkImageEMError("vtkImageEMVector::Reshape: Dimension of input does not match vector!");

  for (i=1; i < dimXPlus; i++) {
    for (j=1; j < dimYPlus; j++) {
         this->vec[index+j] = im[j][i];
    }
    index +=dimY;
  }
}


void vtkImageEMVector::PrintVector(int xMax) {
  int Max = (xMax > -1 ? (xMax > this->len ? this->len : xMax) : this->len) + 1;
  for (int i = 1; i < Max; i++)
    cout << this->vec[i] << " ";
  cout << endl;
}

// Sets 'this equal to  Column 'col' of Matrix im  
void vtkImageEMVector::MatrixCol(vtkImageEMMatrix im, int col) {
  if ((this->len != im.get_dim(1)) ||
      (im.get_dim(2) < col)) 
     vtkImageEMError::vtkImageEMError("vtkImageEMVector::MatrixCol: Dimension of input does not match vector!");
  int iMax = this->len+1;
  for (int i=1; i < iMax; i++) this->vec[i] = im[i][col];
}  

// Convolution and polynomial multiplication . 
// This is assuming u and 'this' have the same dimensio
void vtkImageEMVector::conv(vtkImageEMVector u,vtkImageEMVector v){
  int uLen = u.get_len(), vLen = v.get_len();
  if (this->len != uLen) 
     vtkImageEMError::vtkImageEMError("vtkImageEMVector::conv: Dimension of input does not match vector!");
  int stump = vLen /2;
  int k,j,jMin,jMax;
  int kMax = this->len + stump +1;
  for (k = stump + 1; k <  kMax; k++) {
    this->vec[k-stump] = 0;
    jMin = (1 > k + 1 - vLen ? 1 : k + 1 - vLen ); //  max(1,k+1-vLen):
    jMax = (k < uLen ? k : uLen)+1;                  //min(k,uLen) 
    for (j=jMin; j < jMax; j++)
      this->vec[k-stump] += u[j]*v[k+1-j];
  }  
}

// Opens up a new file and writes down result in the file
void vtkImageEMVector::WriteVectorToFile (char *filename,char *varname) const {
  int appendFlag = 0;
  FILE *f = (strcmp(filename,"-"))?fopen(filename,((appendFlag)?"a":"w")):stdout;
  if ( f == NULL ) {
    cerr << "Could not open file " << filename << "\n";
    return;
  }
  this->WriteVectorToFile(f,varname);
  fflush(f);
  fclose(f);
}

// Writes Vector to file in Matlab format if name is specified otherwise just 
// writes the values in the file
void vtkImageEMVector::WriteVectorToFile (FILE *f,char *name) const
{
  int xMax = this->len +1;
  if (name != NULL) fprintf(f,"%s = [", name);
  for (int x = 1; x < xMax; x++ )
      fprintf(f,"%10.6f ", this->vec[x]);
  if (name != NULL) fprintf(f,"];\n");
}

void vtkImageEMVector::setVectorTest(int test,int division,int pixel, int offset){
  int Xsize = this->len+1;
  int x;
  if (division < 2) {
    this->setVector(0);
    return;
  }
  double ImgScale = 255/double(division-1);
  double XScale ;
  int ImgColor;

  if (pixel > 0) XScale = double(pixel);
  else XScale = double(this->len) /double(division);

  if (test < 2) ImgColor = int(ImgScale*(offset%division));
  for (x =1 ; x < Xsize; x++) {
    if (test> 1) ImgColor = int(ImgScale*((int((x-1)/XScale)+offset)%division));
    this->vec[x] = ImgColor;
  }
}





























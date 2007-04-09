/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkEstimateDiffusionTensor.cxx,v $
  Date:      $Date: 2007/04/09 08:03:38 $
  Version:   $Revision: 1.4.2.1 $

=========================================================================auto=*/
#include "vtkEstimateDiffusionTensor.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "vtkFloatArray.h"
#include "vtkMathUtils.h"
#include "vtkMath.h"

#include "vnl/vnl_matrix.h"
#include "vnl/algo/vnl_matrix_inverse.h"

#define VTKEPS 10e-12

//----------------------------------------------------------------------------
vtkEstimateDiffusionTensor* vtkEstimateDiffusionTensor::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkEstimateDiffusionTensor");
  if(ret)
    {
      return (vtkEstimateDiffusionTensor*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkEstimateDiffusionTensor;
}


//----------------------------------------------------------------------------
vtkEstimateDiffusionTensor::vtkEstimateDiffusionTensor()
{
  // may be set by user
  this->Transform = NULL;

  this->NumberOfRequiredInputs = 1;
  this->NumberOfGradients = 7;
  this->PinvA = NULL;
  this->A = NULL;
  //this->AllocateInternals();
  
  // this is LeBihan's b factor for physical MR gradient parameters 
  // (same number as C-F uses)
  this->BValues = vtkDoubleArray::New();
  this->BValues->SetNumberOfComponents(1);
  this->BValues->SetNumberOfTuples(this->NumberOfGradients);
  for (int i=0; i<this->NumberOfGradients;i++)
    this->BValues->SetValue(i,1000);

  // Internal variable to scale the B values, so we get a
  // much better conditioned matrix so solve the Tensor model
  // LS problem. The diffusion matrix should be scaled back with 
  // this factor to get the real diffusion.
  this->ScaleFactor = 10000;
  
  this->WeightedFitting = 0;

  // Output images beside the estimated tensor
  this->Baseline = vtkImageData::New();
  this->AverageDWI = vtkImageData::New();


  // defaults are from DT-MRI 
  // (from Processing and Visualization for 
  // Diffusion Tensor MRI, C-F Westin, pg 8)
  this->DiffusionGradients = vtkDoubleArray::New();
  this->DiffusionGradients->SetNumberOfComponents(3);
  this->DiffusionGradients->SetNumberOfTuples(this->NumberOfGradients);
  this->SetDiffusionGradient(0,0,0,0);
  this->SetDiffusionGradient(1,1,1,0);
  this->SetDiffusionGradient(2,0,1,1);
  this->SetDiffusionGradient(3,1,0,1);
  this->SetDiffusionGradient(4,0,1,-1);
  this->SetDiffusionGradient(5,1,-1,0);
  this->SetDiffusionGradient(6,-1,0,1);

}
vtkEstimateDiffusionTensor::~vtkEstimateDiffusionTensor()
{
  this->BValues->Delete();
  this->DiffusionGradients->Delete(); 
  this->Baseline->Delete();
  this->AverageDWI->Delete();
  if (this->Transform) 
    {
    this->Transform->Delete();
    }    
}

//----------------------------------------------------------------------------
void vtkEstimateDiffusionTensor::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageToImageFilter::PrintSelf(os,indent);

  os << indent << "NumberOfGradients: " << this->NumberOfGradients << "\n";
  double g[3];
  // print all of the gradients
  for (int i = 0; i < this->NumberOfGradients; i++ ) 
    {
      this->GetDiffusionGradient(i,g);
      os << indent << "Gradient " << i << ": (" 
         << g[0] << ", "
         << g[1] << ", "
         << g[2] << ")"
         <<  "B value: "
         << this->BValues->GetValue(i) << "\n";
    }
}

//----------------------------------------------------------------------------
void vtkEstimateDiffusionTensor::SetDiffusionGradients(vtkDoubleArray *grad)
{
  this->DiffusionGradients->DeepCopy(grad);
  this->NumberOfGradients = this->DiffusionGradients->GetNumberOfTuples();
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkEstimateDiffusionTensor::SetBValues(vtkDoubleArray *bValues)
{
  this->BValues->DeepCopy(bValues);
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkEstimateDiffusionTensor::TransformDiffusionGradients()
{
  vtkFloatingPointType gradient[3];
  double g[3];
  double norm;
  // if matrix has not been set by user don't use it
  if (this->Transform == NULL) 
    {
      return;
    }

  vtkDebugMacro("Transforming diffusion gradients");
  //this->Transform->Print(cout);


  // transform each gradient by this matrix
  for (int i = 0; i < this->NumberOfGradients; i++ ) 
    {
      this->GetDiffusionGradient(i,g);
      this->Transform->TransformPoint(g,gradient);
      norm = sqrt(gradient[0]*gradient[0]+gradient[1]*gradient[1]+gradient[2]*gradient[2]);
      if (norm > VTKEPS)
        {
        gradient[0] /=norm;
        gradient[1] /=norm;
        gradient[2] /=norm;
        }
      // set the gradient to the transformed one 
      this->SetDiffusionGradient(i,gradient);
    }
}

//----------------------------------------------------------------------------
// The number of required inputs is one more than the number of
// diffusion gradients applied.  (Since image 0 is an image
// acquired without diffusion gradients).
void vtkEstimateDiffusionTensor::SetNumberOfGradients(int num) 
{
  if (this->NumberOfGradients != num)
    {
      vtkDebugMacro ("setting num gradients to " << num);
      // internal array for storage of gradient vectors
      this->DiffusionGradients->SetNumberOfTuples(num);
      this->BValues->SetNumberOfTuples(num);
      // this class's info
      this->NumberOfGradients = num;
      //this->NumberOfRequiredInputs = num;
      this->Modified();
    }
}

//----------------------------------------------------------------------------
//
void vtkEstimateDiffusionTensor::ExecuteInformation(vtkImageData *inData, 
                                             vtkImageData *outData)
{
  // We always want to output input[0] scalars Type
  outData->SetScalarType(inData->GetScalarType());
  // We output one scalar components: baseline (for legacy issues)
  outData->SetNumberOfScalarComponents(1);

  this->Baseline->CopyTypeSpecificInformation( this->GetInput() );
  this->AverageDWI->CopyTypeSpecificInformation( this->GetInput() );
  this->Baseline->SetScalarType(inData->GetScalarType());
  this->AverageDWI->SetScalarType(inData->GetScalarType());
  this->Baseline->SetNumberOfScalarComponents(1);
  this->AverageDWI->SetNumberOfScalarComponents(1);

}

//----------------------------------------------------------------------------
// Replace superclass Execute with a function that allocates tensors
// as well as scalars.  This gets called before multithreader starts
// (after which we can't allocate, for example in ThreadedExecute).
// Note we return to the regular pipeline at the end of this function.
void vtkEstimateDiffusionTensor::ExecuteData(vtkDataObject *out)
{
 vtkImageData *output = vtkImageData::SafeDownCast(out);
 vtkImageData *inData = (vtkImageData *) this->GetInput();

  //Check inputs numbertenEstimateMethodLLS
  if (inData == NULL) {
    vtkErrorMacro("Input with DWIs has not been assigned");
    return;
  }  

  //Check if this input is multicomponent and match the number of  gradients
  int ncomp = this->GetInput()->GetPointData()->GetScalars()->GetNumberOfComponents();
  if (ncomp != this->NumberOfGradients) {
      vtkErrorMacro("The input has to have a number of components equal to the number of gradients");
      return;
    }  
  
  // set extent so we know how many tensors to allocate
  output->SetExtent(output->GetUpdateExtent());

  // allocate output tensors
  vtkFloatArray* data = vtkFloatArray::New();
  int* dims = output->GetDimensions();
  vtkDebugMacro("Allocating output tensors, dims " << dims[0] <<" " << dims[1] << " " << dims[2]);
  data->SetNumberOfComponents(9);
  data->SetNumberOfTuples(dims[0]*dims[1]*dims[2]);
  output->GetPointData()->SetTensors(data);
  data->Delete();

  // Allocate baseline and averageDWI images
  this->Baseline->SetExtent(output->GetUpdateExtent());
  this->AverageDWI->SetExtent(output->GetUpdateExtent());
  this->Baseline->AllocateScalars();
  this->AverageDWI->AllocateScalars();
  this->Baseline->GetPointData()->GetScalars()->SetName("Baseline");
  this->AverageDWI->GetPointData()->GetScalars()->SetName("AverageDWI");

  // make sure our gradient matrix is up to date
  //This update is not thread safe and it has to be performed outside
  // the threaded part.
  // if the user has transformed the coordinate system
  this->TransformDiffusionGradients();
  
  //Allocate Internals
  int N = this->GetNumberOfGradients();
 
  this->PinvA=this->AllocateMatrix(7,N);
  this->A=this->AllocateMatrix(N,7);  
   
  if (this->WeightedFitting == 0)
     this->CalculatePseudoInverse();
  else 
     this->CalculateA();   
  
  // jump back into normal pipeline: call standard superclass method here
  //Do not jump to do the proper allocation of output data
  this->vtkImageToImageFilter::ExecuteData(out);
  
  //Deallocate Internals
  this->DeallocateMatrix(PinvA,7,N);
  this->DeallocateMatrix(A,N,7);  
  
}



//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
template <class T>
static void vtkEstimateDiffusionTensorExecute(vtkEstimateDiffusionTensor *self,
                                           vtkImageData *inData, 
                                           T *inPtr,
                                           vtkImageData *outData, 
                                           T * outPtr,
                                           int outExt[6], int id)
{
  int idxX, idxY, idxZ;
  int maxX, maxY, maxZ;
  int inIncX, inIncY, inIncZ;
  int outIncX, outIncY, outIncZ;
  unsigned long count = 0;
  unsigned long target;
  int numInputs, k,i,j;
  double *dwi;
  double **PinvA;
  double **A;
  double B0;
  double averageDWI;    
  vtkDataArray *outTensors;
  double D[3][3];
  float outT[3][3];
  int ptId;
  double **ATW2, **invATW2A, **ATW2A, **localPinvA;

  T * baselinePtr;
  T * averageDWIPtr;
  // Get information to march through output tensor data
  outTensors = self->GetOutput()->GetPointData()->GetTensors();
  
  // Get pointer to already calculated G matrix
  PinvA =  self->GetPinvA();
  A = self->GetA();

  // Preallocate WLS helper matrices
   int N = self->GetNumberOfGradients();
  if (self->GetWeightedFitting())
    {
    ATW2=self->AllocateMatrix(7,N);
    invATW2A=self->AllocateMatrix(7,7);
    ATW2A=self->AllocateMatrix(7,7);
    localPinvA = self->AllocateMatrix(7,N);
    }
  // changed from arrays to pointers
  int *outInc,*outFullUpdateExt;
  outInc = self->GetOutput()->GetIncrements();
  outFullUpdateExt = self->GetOutput()->GetUpdateExtent(); //We are only working over the update extent
  ptId = ((outExt[0] - outFullUpdateExt[0]) * outInc[0]
         + (outExt[2] - outFullUpdateExt[2]) * outInc[1]
         + (outExt[4] - outFullUpdateExt[4]) * outInc[2]);

  // Get pointer to Baseline and AverageDWI Images
  baselinePtr = (T *) self->GetBaseline()->GetScalarPointerForExtent(outExt);
  averageDWIPtr = (T *) self->GetAverageDWI()->GetScalarPointerForExtent(outExt);

  // find the region to loop over
  maxX = outExt[1] - outExt[0];
  maxY = outExt[3] - outExt[2]; 
  maxZ = outExt[5] - outExt[4];
  target = (unsigned long)(outData->GetNumberOfScalarComponents()*
                           (maxZ+1)*(maxY+1)/50.0);
  target++;

  // Get increments to march through image data 
  inData->GetContinuousIncrements(outExt, inIncX, inIncY, inIncZ);
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);

  numInputs = inData->GetNumberOfScalarComponents();
  dwi = new double[numInputs];

  for (idxZ = 0; idxZ <= maxZ; idxZ++)
    {
      for (idxY = 0; !self->AbortExecute && idxY <= maxY; idxY++)
        {
          if (!id) 
            {
              if (!(count%target)) 
                {
                  self->UpdateProgress(count/(50.0*target));
                }
              count++;
            }
          for (idxX = 0; idxX <= maxX; idxX++)
            {
              // create tensor from combination of gradient inputs 
          averageDWI = 0.0;
              for (k = 0; k < numInputs; k++)
                  {
                  // diffusion from kth gradient
                  dwi[k] = (double) inPtr[k];
          averageDWI += dwi[k];
                  }

              if (self->GetWeightedFitting() == 0) {
                 self->EstimateLSTensorModel(dwi,PinvA,D,B0);
              } else {
                 self->EstimateWLSTensorModel(dwi, A, ATW2, invATW2A, ATW2A, localPinvA, D,B0);
              }
                //NOTE: Scale back Diffusion tensor.
                //When computing LS matrix (CreateLSMatrix method), we reduce B by a scale fact
                //to get a better numeric accuracy. This means that the diffusion is scale by this
                // factor. We undo this, to give back the real diffusion
              for (i = 0; i < 3; i++)
                {
                  for (j = 0; j < 3; j++)
                    {
                      outT[i][j] = (float) (D[i][j]/self->GetScaleFactor());
                    }
                }
              // Pixel operation              
              outTensors->SetTuple(ptId,(float *)outT);
              // copy no diffusion data through for scalars
              *outPtr = (T) B0;

              // Copy B0 and DWI
             *baselinePtr = (T) B0;
             *averageDWIPtr = (T) (averageDWI - B0)/(numInputs-1);

              inPtr += numInputs;
              ptId ++;
              outPtr++;
              baselinePtr++;
              averageDWIPtr++;
            }
          outPtr += outIncY;
          ptId += outIncY;
          baselinePtr += outIncY;
          averageDWIPtr += outIncY;
      inPtr += inIncY;
        }
      outPtr += outIncZ;
      ptId += outIncZ;
      baselinePtr += outIncZ;
      averageDWIPtr += outIncZ;
      inPtr += inIncZ;
    }

  // Deallocate helper matrices
  if (self->GetWeightedFitting())
    {
    self->DeallocateMatrix(invATW2A,7,7);
    self->DeallocateMatrix(ATW2A,7,7);
    self->DeallocateMatrix(ATW2,7,N);
    self->DeallocateMatrix(localPinvA,7,N);
    }
  delete [] dwi;
}

//----------------------------------------------------------------------------
// This method is passed a input and output regions, and executes the filter
// algorithm to fill the output from the inputs.
// It just executes a switch statement to call the correct function for
// the regions data types.
void vtkEstimateDiffusionTensor::ThreadedExecute(vtkImageData *inData, 
                                              vtkImageData *outData,
                                              int outExt[6], int id)
{
  int idx;
  void *inPtr;
  void *outPtr = outData->GetScalarPointerForExtent(outExt);

  vtkDebugMacro("in threaded execute, " << this->GetNumberOfInputs() << " inputs ");
  
   inPtr= inData->GetScalarPointerForExtent(outExt);

  // call Execute method to handle all data at the same time
  switch (inData->GetScalarType())
    {
      vtkTemplateMacro7(vtkEstimateDiffusionTensorExecute, this, 
                        inData, (VTK_TT *)(inPtr),
                        outData, (VTK_TT *)(outPtr), 
                        outExt, id);
    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType");
      return;
    }
  
}

//The input are the dwi images, the B matrix with gradient infor, and preallocate double pointers
// for computation.
// ATW2 is a 7xN matrix
// invATW2A is a 7x7 matrix
// ATW2A is a 7x7 matrix
// localPinvA is a 7xN matrix
void vtkEstimateDiffusionTensor::EstimateWLSTensorModel(double *dwi,double **A,double **ATW2, double **invATW2A, double **ATW2A, double **localPinvA,  double D[3][3],
  double &B0)
{

  // If weighted fitting,
  // 1. Build weights
  // 2. Compute pseudoinverse
  int N = this->GetNumberOfGradients(); 
  //double **ATW2=this->AllocateMatrix(7,N); 
  for (int i=0; i< 7; i++)
    {
     for (int j=0 ; j <N ; j++)
      {
       ATW2[i][j] = A[j][i];
      }
    }

  for (int i=0; i<7;i++)
    {
    for (int j=0; j<N;j++)
      {
      //squareweigts = dwi * dwi
      //ATW2[i][j] = diffparams->squareWeights[j]*AT[i][j];
      if (dwi[j]<1)
        dwi[j]=1;
      ATW2[i][j] = dwi[j]*dwi[j]*ATW2[i][j];
       }
    }

    //double **invATW2A;
    //double **ATW2A;
    //double **localPinvA;
    //invATW2A=this->AllocateMatrix(7,7);
    //ATW2A=this->AllocateMatrix(7,7);
    //localPinvA = this->AllocateMatrix(7,N);

    vtkMathUtils::MatrixMultiply(ATW2,A,ATW2A,7,N,N,7);
    vtkMath::InvertMatrix(ATW2A,invATW2A,7);
    vtkMathUtils::MatrixMultiply(invATW2A,ATW2,localPinvA,7,7,7,N);
    
//     this->DeallocateMatrix(invATW2A,7,7);
//     this->DeallocateMatrix(ATW2A,7,7);
//     this->DeallocateMatrix(ATW2,7,N);
//     this->DeallocateMatrix(localPinvA,7,N);

   // Solve the system
   double tmp[7];

   for (int i=0 ; i<7 ; i++)
     {
      tmp[i]=0.0;
      for (int j=0 ; j< N ; j++){
         tmp[i] += localPinvA[i][j] * log(dwi[j]+VTKEPS);
      }
    }

  // tmp is the scale version of the diffusion tensor
  D[0][0] = tmp[1];
  D[0][1] = tmp[2];
  D[1][0] = tmp[2];
  D[0][2] = tmp[3];
  D[2][0] = tmp[3];
  D[1][1] = tmp[4];
  D[1][2] = tmp[5];
  D[2][1] = tmp[5];
  D[2][2] = tmp[6];

  B0 = exp(tmp[0]);

}


//The input is the dwi array and the pseudoinverse of the gradient matrices.
void vtkEstimateDiffusionTensor::EstimateLSTensorModel(double
*dwi,double **PinvA, double D[3][3], double &B0)
{


  // If weighted fitting,
  // 1. Build weights
  // 2. Compute pseudoinverse
  int N = this->GetNumberOfGradients();
 
   // Solve the system
   double tmp[7];

   for (int i=0 ; i<7 ; i++)
     {
      tmp[i]=0.0;
      for (int j=0 ; j< N ; j++){
         tmp[i] += PinvA[i][j] * log(dwi[j] + VTKEPS);
      }
    }

  // tmp is the scale version of the diffusion tensor
  D[0][0] = tmp[1];
  D[0][1] = tmp[2];
  D[1][0] = tmp[2];
  D[0][2] = tmp[3];
  D[2][0] = tmp[3];
  D[1][1] = tmp[4];
  D[1][2] = tmp[5];
  D[2][1] = tmp[5];
  D[2][2] = tmp[6];
 
  B0 = exp(tmp[0]);
}

double **vtkEstimateDiffusionTensor::AllocateMatrix(int rows, int columns)
{
  
   double **M = new double*[rows];
    for (int i=0; i< rows; i++)
       {
          M[i] = new double[columns];
       }
  return M;
}

void vtkEstimateDiffusionTensor::DeallocateMatrix(double **M,int rows, int columns)
{

  if (M != NULL) 
    {
    for (int i=0; i< rows; i++)
     {
        if (M[i] != NULL) 
    {
          delete [] M[i];
    }
     }
  
  delete M;
  }     
}

void vtkEstimateDiffusionTensor::CalculatePseudoInverse()
{

  int N = this->GetNumberOfGradients();
 
  this->CalculateA();
   
   vnl_matrix<double> G;
   vnl_matrix<double> Ginv;
  
    G.set_size(N,7);
    Ginv.set_size(7,N);
    for(int i=0; i< N; i++)
      for(int j=0; j< 7; j++)
        G.put(i,j,this->A[i][j]);

    vnl_matrix_inverse<double>  Pinv(G);
    Ginv = Pinv.pinverse(7);
    for (int i = 0; i < 7; i++)
    {
     for (int j = 0; j< N; j++)
       {
       this->PinvA[i][j] = Ginv.get(i,j);
       }
    }

}

void vtkEstimateDiffusionTensor::CalculateA() {
   
  int N = this->GetNumberOfGradients();
 
  double b;
  double g[3];

   for (int i=0; i< N; i++)
    {
     //We scale B values by a given factor to get a better
    // condition of A matrix, then the pseudoinverse is much
    // more stable.
    // Be aware that the resulting diffusion tensor will be 
    // scale by this factor. We take care later on.
     b = this->BValues->GetValue(i)/this->ScaleFactor;
     this->GetDiffusionGradient(i,g);
     this->A[i][0]=1;
     this->A[i][1]=-b*g[0]*g[0];
     this->A[i][2]=-2*b *g[0]*g[1];
     this->A[i][3]=-2*b *g[0]*g[2];
     this->A[i][4]=-b *g[1]*g[1];
     this->A[i][5]=-2*b *g[1]*g[2];
     this->A[i][6]=-b*g[2]*g[2];
    }
}

#include "vtkVectorToOuterProductDualBasis.h"
#include "vtkObjectFactory.h"
#include "vtkMathUtils.h"
#include "vtkMath.h"

#define VTK_VECTOR_LENGTH 3

//-------------------------------------------------------------------------
vtkVectorToOuterProductDualBasis* vtkVectorToOuterProductDualBasis::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkVectorToOuterProductDualBasis");
  if(ret)
    {
      return (vtkVectorToOuterProductDualBasis*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkVectorToOuterProductDualBasis;
}

//-------------------------------------------------------------------------
vtkVectorToOuterProductDualBasis::vtkVectorToOuterProductDualBasis()
{
  this->NumberOfInputVectors = 6;
  // vectors
  this->V = NULL;
  // outer product of vectors
  this->VV = NULL;
  // VV transposed
  this->VVT = NULL;
  // VV transposed times VV
  this->VVTVV = NULL;
  // VV transposed times VV inverse
  this->VVTVVI = NULL;
  // pseudoinverse
  this->PInv = NULL;

  this->AllocateInternals();

  // defaults are from DT-MRI
  this->SetInputVector(0,1,1,0);
  this->SetInputVector(1,0,1,1);
  this->SetInputVector(2,1,0,1);
  this->SetInputVector(3,0,1,-1);
  this->SetInputVector(4,1,-1,0);
  this->SetInputVector(5,-1,0,1);
}

//----------------------------------------------------------------------------
vtkVectorToOuterProductDualBasis::~vtkVectorToOuterProductDualBasis()
{
  this->DeallocateInternals();
}

//----------------------------------------------------------------------------
void vtkVectorToOuterProductDualBasis::PrintSelf(ostream& os, vtkIndent indent)
{
  int i,j,N;

  vtkObject::PrintSelf(os,indent);

  N =   this->NumberOfInputVectors;

  os << indent << "NumberOfInputVectors: "<< this->NumberOfInputVectors << "\n";

  // vectors
  for (i=0; i< this->NumberOfInputVectors; i++)
    {
      os << indent << "Input Vector " << i << ": ";
      for (j=0; j<VTK_VECTOR_LENGTH ; j++)
        {
          os << this->V[i][j] << " ";
        }
      os << "\n";
    }
  // outer product of vectors
  os << indent << "VV (outer product) " << ": \n";
  if (this->VV)
    vtkMathUtils::PrintMatrix(this->VV,9,N,os,indent);
  // VV transposed
  os << indent << "VVT (transpose) " << ": \n";
  if (this->VVT)
    vtkMathUtils::PrintMatrix(this->VVT,N,9,os,indent);
  // VV transposed times VV
  os << indent << "VVTVV (product)" << ": \n";
  if (this->VVTVV)
    vtkMathUtils::PrintMatrix(this->VVTVV,N,N,os,indent);
  // VV transposed times VV inverse
  os << indent << "VVTVVI (inverse)" << ": \n";
  if (this->VVTVVI)
    vtkMathUtils::PrintMatrix(this->VVTVVI,N,N,os,indent);
  // pseudoinverse of VV
  os << indent << "PseudoInverse" << ": \n";
  if (this->PInv)
    vtkMathUtils::PrintMatrix(this->PInv,N,9,os,indent);
}

//----------------------------------------------------------------------------
void vtkVectorToOuterProductDualBasis::SetNumberOfInputVectors(int num) 
{
  if (this->NumberOfInputVectors != num)
    {
      // first kill old objects
      this->DeallocateInternals();

       vtkDebugMacro ("setting num input vecotrs to " << num);
      this->NumberOfInputVectors = num;
      this->AllocateInternals();
      this->Modified();
    }
}

// Allocate all the 2D arrays we use internally.
// Since vtkMath expects double**, we use that
// in this class.
void vtkVectorToOuterProductDualBasis::AllocateInternals()
{
  int i;

  if (this->NumberOfInputVectors > 0)
    {
      // allocate space for the vectors (Nx3)
      this->V = new vtkFloatingPointType*[this->NumberOfInputVectors];
      for (i=0; i< this->NumberOfInputVectors; i++)
        {
          this->V[i] = new vtkFloatingPointType[VTK_VECTOR_LENGTH];
        }


      // allocate space for the outer product matrices (9xN)
      // each column is a 9-vector from outer product
      this->VV = new double*[9];
      for (i=0; i< 9; i++)
        {
          this->VV[i] = new double[this->NumberOfInputVectors];
        }

      // allocate space for the transpose of VV (Nx9)
      this->VVT = new double*[this->NumberOfInputVectors];
      for (i=0; i< this->NumberOfInputVectors; i++)
        {
          this->VVT[i] = new double[9];
        }

      // allocate space for the transpose of VV times VV (NxN)
      this->VVTVV = new double*[this->NumberOfInputVectors];
      for (i=0; i< this->NumberOfInputVectors; i++)
        {
          this->VVTVV[i] = new double[this->NumberOfInputVectors];
        }

      // allocate space for the transpose of VV times VV inverse (NxN) 
      this->VVTVVI = new double*[this->NumberOfInputVectors];
      for (i=0; i< this->NumberOfInputVectors; i++)
        {
          this->VVTVVI[i] = new double[this->NumberOfInputVectors];
        }

      // allocate space for the pseudoinverse (Nx9)
      this->PInv = new double*[this->NumberOfInputVectors];
      for (i=0; i< this->NumberOfInputVectors; i++)
        {
          this->PInv[i] = new double[9];
        }
    }

}

void vtkVectorToOuterProductDualBasis::DeallocateInternals()
{
  int i;

  if (this->V != NULL) 
    {
      // delete interior vectors first
      for (i=0; i< this->NumberOfInputVectors; i++)
        {
          delete []this->V[i];
        }
      // now delete the pointers
      delete []this->V;
      this->V = NULL;
    }
  if (this->VV != NULL) 
    {
      for (i=0; i< this->NumberOfInputVectors; i++)
        {
          delete []this->VV[i];
        }
      delete []this->VV;
      this->VV = NULL;
    }

  if (this->VVT != NULL) 
    {
      for (i=0; i< this->NumberOfInputVectors; i++)
        {
          delete []this->VVT[i];
        }
      delete []this->VVT;
      this->VVT = NULL;
    }

  if (this->VVTVV != NULL) 
    {
      for (i=0; i< this->NumberOfInputVectors; i++)
        {
          delete []this->VVTVV[i];
        }
      delete []this->VVTVV;
      this->VVTVV = NULL;
    }
  if (this->VVTVVI != NULL) 
    {
      for (i=0; i< this->NumberOfInputVectors; i++)
        {
          delete []this->VVTVVI[i];
        }
      delete []this->VVTVVI;
      this->VVTVVI = NULL;
    }
  if (this->PInv != NULL) 
    {
      for (i=0; i< this->NumberOfInputVectors; i++)
        {
          delete []this->PInv[i];
        }
      delete []this->PInv;
      this->PInv = NULL;
    }
}


void vtkVectorToOuterProductDualBasis::SetInputVector(int num, 
                              vtkFloatingPointType vector[VTK_VECTOR_LENGTH]) 
{
  vtkFloatingPointType length = 0;

  if (num > this->NumberOfInputVectors-1)
    {
      vtkErrorMacro("We don't have that many input vectors");
      return;
    }
  if (this->V == NULL) 
    {
      this->AllocateInternals();
    }
  if (this->NumberOfInputVectors < 1) 
    {
      vtkErrorMacro("Need more than 0 vectors, use SetNumberOfInputVectors");
      return;    
    }

  // normalize vector
  for (int i=0; i < VTK_VECTOR_LENGTH; i++)
    {
      length += vector[i]*vector[i];  
    }
  length = sqrt(length);

  for (int i=0; i < VTK_VECTOR_LENGTH; i++)
    {
      this->V[num][i] = vector[i]/length;  
    }

}

void vtkVectorToOuterProductDualBasis::SetInputVector(int num, vtkFloatingPointType v0, 
                              vtkFloatingPointType v1, vtkFloatingPointType v2)
{

  vtkFloatingPointType *tmp = new vtkFloatingPointType[VTK_VECTOR_LENGTH];
  tmp[0] = v0;
  tmp[1] = v1;
  tmp[2] = v2;

  this->SetInputVector(num,tmp);

  delete [] tmp;
}

void vtkVectorToOuterProductDualBasis::CalculateDualBasis()
{
  int i,j,k,count,N, result;
  // temp storage
  vtkFloatingPointType A[VTK_VECTOR_LENGTH][VTK_VECTOR_LENGTH];

  N =   this->NumberOfInputVectors;

  vtkDebugMacro("Calculating dual basis");

  // first fill the VV (outer product) array
  // loop over all input vectors
  for (i = 0; i < N; i++)
    {
      // do outer product of each of the input vectors
      vtkMathUtils::Outer3(this->V[i],this->V[i],A);

      // copy elements of A into our class arrays
      count = 0;
      for (j = 0; j < VTK_VECTOR_LENGTH; j++)
    {
      for (k = 0; k < VTK_VECTOR_LENGTH; k++)
        {
          // place in cols of VV
          this->VV[count][i] = A[j][k];

          // also place in rows of VVT (transposed) array
          this->VVT[i][count] = A[j][k];          
          //cout << "A[j][k]" << j << " " << k << " " << A[j][k] << endl;
          count++;
        }
    }
    }

  // multiply VVT by VV to make VVTVV, symmetric invertible matrix
  vtkMathUtils::MatrixMultiply(this->VVT,this->VV,this->VVTVV,N,9,9,N);

  vtkDebugMacro("inverting VVT VV matrix");

  // invert VVTVV
  result = vtkMath::InvertMatrix (this->VVTVV, this->VVTVVI, N);

  vtkDebugMacro("result of inverting matrix was: " << result);
  if (result == 0)
    {
      vtkErrorMacro("VVTVV Matrix could not be inverted!");
      vtkMathUtils::PrintMatrix(this->VVTVV,N,N);
    }

//    vtkMathUtils::PrintMatrix(this->VVTVVI,N,N);

  vtkDebugMacro("multiplying to make pinv");

  // make pseudoinverse: (HT H)^-1 HT
  vtkMathUtils::MatrixMultiply(this->VVTVVI,this->VVT,this->PInv,N,N,N,9);
  //vtkMathUtils::PrintMatrix(this->PInv,N,9);

}

void vtkVectorToOuterProductDualBasis::PrintPseudoInverse(ostream &os)
{
  int N;
  // create this just to use the PrintMatrix function
  vtkIndent indent;

  N =   this->NumberOfInputVectors;

  // pseudoinverse of VV
  os << "PseudoInverse" << ": \n";
  if (this->PInv)
    vtkMathUtils::PrintMatrix(this->PInv,N,9,os,indent);
}

void vtkVectorToOuterProductDualBasis::PrintPseudoInverse()
{
  this->PrintPseudoInverse(cout);
}

#include "vtkTensorRegularization.h"
#include "vtkObjectFactory.h"
#include "vtkMath.h"
#include "vtkMathUtils.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkPointData.h"

//------------------------------------------------------------------------------
vtkTensorRegularization* vtkTensorRegularization::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkTensorRegularization");
  if(ret)
    {
      return (vtkTensorRegularization*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkTensorRegularization;
}

//----------------------------------------------------------------------------
vtkTensorRegularization::vtkTensorRegularization()
{
  this->ExtractEigenvalues = 1;
}

vtkTensorRegularization::~vtkTensorRegularization()
{

}

//----------------------------------------------------------------------------
// Replace superclass Execute with a function that allocates tensors
// as well as scalars.  This gets called before multithreader starts
// (after which we can't allocate, for example in ThreadedExecute).
// Note we return to the regular pipeline at the end of this function.
void vtkTensorRegularization::ExecuteData(vtkDataObject *out)
{
  vtkImageData *output = vtkImageData::SafeDownCast(out);

  // set extent so we know how many tensors to allocate
  output->SetExtent(output->GetUpdateExtent());
  
  // allocate output tensors
  vtkFloatArray* data = vtkFloatArray::New();
  int* dims = output->GetDimensions();
  data->SetNumberOfComponents(9);
  data->SetNumberOfTuples(dims[0]*dims[1]*dims[2]);
  output->GetPointData()->SetTensors(data);
  data->Delete();

  // jump back into normal pipeline: call standard superclass method here
  this->vtkImageToImageFilter::ExecuteData(out);
}


//----------------------------------------------------------------------------
//
static void vtkTensorRegularizationExecute(vtkTensorRegularization *self,
                       int ext[6],
                       vtkImageData *inData,
                       vtkImageData *outData, 
                       int id)
{
  int num0, num1, num2;
  int idx0, idx1, idx2;
  int in1Inc0, in1Inc1, in1Inc2;
  int in2Inc0, in2Inc1, in2Inc2;
  int outInc0, outInc1, outInc2;
  unsigned long count = 0;
  unsigned long target;
  // for stepping through data
  vtkDataArray *inTensors;
  vtkDataArray *outTensors;
  float inT[3][3];
  float outT[3][3];
  int ptId;
  // for eigensystem
  float *m[3], w[3], *v[3];
  float m0[3], m1[3], m2[3];
  float v0[3], v1[3], v2[3];
  float xv[3], yv[3], zv[3];
  int extractEigenvalues;
  int i,j,k;

  // set up working matrices
  m[0] = m0; m[1] = m1; m[2] = m2; 
  v[0] = v0; v[1] = v1; v[2] = v2; 

  // input tensors
  inTensors = self->GetInput()->GetPointData()->GetTensors();
  // output tensors
  outTensors = self->GetOutput()->GetPointData()->GetTensors();

  //Raul: Bad ptId inizialization
  // "GetTensorPointerForExtent" (get start spot in point data)
  // This is the id in the input and output datasets.
  //ptId = ext[0] + ext[2]*(ext[1]-ext[0]) + ext[4]*(ext[3]-ext[2]);

  int *outInc,*outFullUpdateExt;
  outInc = self->GetOutput()->GetIncrements();
  outFullUpdateExt = self->GetOutput()->GetUpdateExtent(); //We are only working over the update extent
  ptId = ((ext[0] - outFullUpdateExt[0]) * outInc[0]
     + (ext[2] - outFullUpdateExt[2]) * outInc[1]
     + (ext[4] - outFullUpdateExt[4]) * outInc[2]);


  // Get information to march through data 
  inData->GetContinuousIncrements(ext, in1Inc0, in1Inc1, in1Inc2);
  outData->GetContinuousIncrements(ext, outInc0, outInc1, outInc2);
  num0 = ext[1] - ext[0] + 1;
  num1 = ext[3] - ext[2] + 1;
  num2 = ext[5] - ext[4] + 1;
  
  target = (unsigned long)(num2*num1/50.0);
  target++;

  extractEigenvalues = self->GetExtractEigenvalues();

  // Loop through output pixels
  for (idx2 = 0; idx2 < num2; ++idx2)
    {
      for (idx1 = 0; !self->AbortExecute && idx1 < num1; ++idx1)
    {
      if (!id) 
        {
          if (!(count%target))
        {
          self->UpdateProgress(count/(50.0*target));
        }
          count++;
        }
      for (idx0 = 0; idx0 < num0; ++idx0)
        {
          inTensors->GetTuple(ptId,(float *)inT);
          //outTensors->GetTuple(ptId,(float *)outT);

          // Find eigenvalues for regularization
          if ( extractEigenvalues) // extract appropriate eigenfunctions
        {
          for (j=0; j<3; j++)
            {
              for (i=0; i<3; i++)
            {
              m[i][j] = inT[i][j];
            }
            }
          vtkMath::Jacobi(m, w, v);
          
          //copy eigenvectors
          xv[0] = v[0][0]; xv[1] = v[1][0]; xv[2] = v[2][0];
          yv[0] = v[0][1]; yv[1] = v[1][1]; yv[2] = v[2][1];
          zv[0] = v[0][2]; zv[1] = v[1][2]; zv[2] = v[2][2];
        }
          else //use tensor columns as eigenvectors
        {
          for (i=0; i<3; i++)
            {
              xv[i] = inT[i][0];
              yv[i] = inT[i][1];
              zv[i] = inT[i][2];
            }
          w[0] = vtkMath::Normalize(xv);
          w[1] = vtkMath::Normalize(yv);
          w[2] = vtkMath::Normalize(zv);
        }

          for (i=0; i<3; i++)
        {
          for (i=0; i<3; i++)
            {
              // copy through the input tensor
              //outT->DeepCopy(inT);
              outT[i][j] = inT[i][j];
            }          
        }

          //  // remove negative eigenvalues by
//            // subtracting a spherical tensor
//            // actually try doing something else, this 
//            // changes the other evalues too
//            // This means subtract the eigenvalue from the diagonal
//            // Lauren what if all are negative?  should the thing
//            // be set to all 0's?
//            int negativeEigenvaluesFound = 0;
//            for (i=0; i<3; i++)
//          {
//            if (w[i] < 0)
//              {
//                outT->AddComponent(0,0,-w[i]);
//                outT->AddComponent(1,1,-w[i]);
//                outT->AddComponent(2,2,-w[i]);
//                negativeEigenvaluesFound++;
//              }
//            }

            int negativeEigenvaluesFound = 0;
          for (i=0; i<3; i++)
        {
          if (w[i] < 0)
            {
              w[i] = 0;
              negativeEigenvaluesFound++;
            }
          }

          // re-create the tensor with no negative eigenvalues
          if (negativeEigenvaluesFound > 0)
        {
          // set tensor to all 0's
          for (i=0; i<3; i++)
            {
              for (i=0; i<3; i++)
            {
              //outT->Initialize();
              outT[i][j] = 0;
            }          
            }

          // Do outer product of each of the vectors,
          // then sum to form tensor.
          // Note this is just matrix decomposition Vt * U * V
          // where V and Vt (transpose) are eigenvector matrices
          // and U is a diagonal matrix with the eigenvalues.

//            float Vt[3][3];
//            float V[3][3];
//            float U[3][3];
//              for (i=0; i<3; i++)
//              {
//                // eigenvectors in rows
//                Vt[0][j] = xv[j];
//                Vt[1][j] = yv[j];
//                Vt[2][j] = zv[j];

//                // eigenvectors in columns
//                Vt[j][0] = xv[j];
//                Vt[j][1] = yv[j];
//                Vt[j][2] = zv[j];

//                // eigenvalues on diagonal
//                for (j=0; j<3; j++)
//              {
//                if (i==j)
//                  U[i][j] = w[i];
//                else
//                  U[i][j] = 0;
//              }
//              }
          
//            // temp storage
//            float A[3][3], B[3][3];

//            // Vt * U * V          
//            vtkMathUtils::MatrixMultiply(Vt,U,A,3,3,3);
//            vtkMathUtils::MatrixMultiply(A,V,B,3,3,3);


//            // set the output tensor
//            for (j=0; j<3; j++)
//              {
//                for (k=0; k<3; k++)
//              {
//                outT->SetComponent(j,k,B[j][k]);
//              } 
//              }

          
          float A[3][3];
          float *evectors[3];
          evectors[0] = xv;
          evectors[1] = yv;
          evectors[2] = zv;

          // for all eigenvalues do
          for (i=0; i<3; i++)
            {
              // only need to do this if w (evalue) nonzero
              if (w[i] !=0)
            {
              // multiply eigenvector by eigenvalue
              float wevector[3];
              for (j=0; j<3; j++)
                {
                  wevector[j] = evectors[i][j]*w[i];
                }

              // create tensor from outer product
              vtkMathUtils::Outer3(evectors[i],wevector,A);

              // add this tensor to output tensor
              for (j=0; j<3; j++)
                {
                  for (k=0; k<3; k++)
                {
                  outT[j][k]+=A[j][k];
                } 
                }

            } // end if this eigenvalue nonzero
            } // end for all eigenvalues do
        }  // end if negative eigenvalue found at this point


          // preprocessing steps we can take
          // multiply by (1-spherical)
          double anisotropy = 1-(3*w[2]/(w[0] + w[1] + w[2]));
          for (j=0; j<3; j++)
        {
          for (k=0; k<3; k++)
            {
              outT[j][k]*=anisotropy;
            } 
        }

          // set the output tensor to the calculated one
          outTensors->SetTuple(ptId,(float *)outT);

          ptId += 1;
        }
      ptId += outInc1;
    }
      ptId += outInc2;
    }
}


//----------------------------------------------------------------------------
// This method is passed a input and output Datas, and executes the filter
// algorithm to fill the output from the inputs.
// It just executes a switch statement to call the correct function for
// the Datas data types.
// now we ignore scalars if present, could output something there
void vtkTensorRegularization::ThreadedExecute(vtkImageData *inData, 
                          vtkImageData *outData,
                          int outExt[6], int id)
{
  void *inPtr1;
  void *outPtr;
  vtkDataArray *inTensors;

  // input tensors
  inTensors = this->GetInput()->GetPointData()->GetTensors();

  // copy input scalars through
  // if we are thread 0 pass input scalars to output
  if (id == 0) 
    {
      this->GetOutput()->GetPointData()->CopyScalarsOn();
      this->GetOutput()->GetPointData()->CopyTensorsOff();
      this->GetOutput()->GetPointData()->PassData(this->GetInput()->GetPointData());
    }
  //this->GetOutput()->GetPointData()->GetScalars()->DeepCopy(this->GetInput()->GetPointData()->GetScalars());
  

  // do we have input tensors?
  if (inTensors) 
    {
      cout << "Executing now  " << id << endl;
      vtkTensorRegularizationExecute(this, outExt, inData, 
                     outData, id);
      
      cout << "Done Executing now" << endl;
    }
  else
    {
      vtkErrorMacro(<< "Execute: no tensors in input, exiting.");
      return;
    }
}

void vtkTensorRegularization::PrintSelf(ostream& os, vtkIndent indent)
{

}


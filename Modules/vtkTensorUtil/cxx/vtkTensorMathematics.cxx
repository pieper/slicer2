#include "vtkImageData.h"
#include "vtkTensorMathematics.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkTransform.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "time.h"


//----------------------------------------------------------------------------
vtkTensorMathematics* vtkTensorMathematics::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkTensorMathematics");
  if(ret)
    {
    return (vtkTensorMathematics*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkTensorMathematics;
}






//----------------------------------------------------------------------------
vtkTensorMathematics::vtkTensorMathematics()
{
  this->Operation = VTK_TENS_TRACE;

  this->ScaleFactor = 1.0;
  this->ExtractEigenvalues = 1;
  this->TensorRotationMatrix = NULL;
}



//----------------------------------------------------------------------------
// 
void vtkTensorMathematics::ExecuteInformation(vtkImageData **inDatas, 
                         vtkImageData *outData)
{
  int ext[6];
  //int ext[6], *ext2, idx;

  inDatas[0]->GetWholeExtent(ext);
  
  // We want the whole extent of the tensors, and should
  // allocate the matching size image data.
  vtkDebugMacro(<<"Execute information extent: " << 
  ext[0] << " " << ext[1] << " " << ext[2] << " " <<
  ext[3] << " " << ext[4] << " " << ext[5]);

  // We always want to output float, unless it is color
  outData->SetScalarType(VTK_FLOAT);

  if (this->Operation == VTK_TENS_COLOR_ORIENTATION) 
    {
      // output color (RGBA)
      outData->SetNumberOfScalarComponents(4);
      outData->SetScalarType(VTK_UNSIGNED_CHAR);
    }

  outData->SetWholeExtent(ext);
}






//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
// Handles the one input operations.
// Handles the ops where eigensystems are not computed.
template <class T>
static void vtkTensorMathematicsExecute1(vtkTensorMathematics *self,
                    vtkImageData *in1Data, 
                    vtkImageData *outData, T *outPtr,
                    int outExt[6], int id)
{
  // image variables
  int idxR, idxY, idxZ;
  int maxY, maxZ;
  int outIncX, outIncY, outIncZ;
  int inIncX, inIncY, inIncZ;
  int rowLength;
  // progress
  unsigned long count = 0;
  unsigned long target;
  // math operation
  int op = self->GetOperation();
  // tensor variables
  vtkDataArray *inTensors;
  float tensor[3][3];
  vtkPointData *pd;
  int numPts, inPtId;
  // time
  clock_t tStart=0;
  tStart = clock();
  // working matrices
  float *m[3], w[3], *v[3];
  float m0[3], m1[3], m2[3];
  float v0[3], v1[3], v2[3];
  m[0] = m0; m[1] = m1; m[2] = m2; 
  v[0] = v0; v[1] = v1; v[2] = v2;
  int i, j;
  // scaling
  float scaleFactor = self->GetScaleFactor();

  // find the input region to loop over
  pd = in1Data->GetPointData();
  inTensors = pd->GetTensors();
  numPts = in1Data->GetNumberOfPoints();

  if ( !inTensors || numPts < 1 )
    {
      vtkGenericWarningMacro(<<"No input tensor data to filter!");
      return;
    }

  // find the output region to loop over
  rowLength = (outExt[1] - outExt[0]+1);
  maxY = outExt[3] - outExt[2]; 
  maxZ = outExt[5] - outExt[4];
  target = (unsigned long)((maxZ+1)*(maxY+1)/50.0);
  target++;
  
  // Get increments to march through output data 
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);
  in1Data->GetContinuousIncrements(outExt, inIncX, inIncY, inIncZ);

  //Initialize ptId to walk through tensor volume
  int *inInc,*inFullUpdateExt;
  inInc = in1Data->GetIncrements();
  inFullUpdateExt = in1Data->GetExtent(); //We are only working over the update extent
  inPtId = ((outExt[0] - inFullUpdateExt[0]) * inInc[0]
     + (outExt[2] - inFullUpdateExt[2]) * inInc[1]
     + (outExt[4] - inFullUpdateExt[4]) * inInc[2]);



  for (idxZ = 0; idxZ <= maxZ; idxZ++)
    {
      for (idxY = 0; idxY <= maxY; idxY++)
    {
      if (!id) 
        {
          if (!(count%target))
        {
          self->UpdateProgress(count/(50.0*target));
        }
          count++;
        }

      for (idxR = 0; idxR < rowLength; idxR++)
        {
          // tensor at this voxel
          inTensors->GetTuple(inPtId,(float *)tensor);

          // pixel operation
          switch (op)
        {
        case VTK_TENS_D11:
          *outPtr = (T)(scaleFactor*tensor[0][0]);
          break;

        case VTK_TENS_D22:
          *outPtr = (T)(scaleFactor*tensor[1][1]);
          break;

        case VTK_TENS_D33:
          *outPtr = (T)(scaleFactor*tensor[2][2]);
          break;

        case VTK_TENS_TRACE:
          *outPtr = (T)(scaleFactor*(tensor[0][0]
                         +tensor[1][1]
                         +tensor[2][2]));
          break;

        case VTK_TENS_DETERMINANT:
          *outPtr = 
            (T)(scaleFactor*(vtkMath::Determinant3x3(tensor)));
          break;
        }

          if (inPtId > numPts) 
        {
          vtkGenericWarningMacro(<<"not enough input pts for output extent "<<numPts<<" "<<inPtId);
        }
          outPtr++;
          inPtId++;
        }
      outPtr += outIncY;
          inPtId += inIncY;
    }
      outPtr += outIncZ;
      inPtId += outIncZ;
    }

  //cout << "tensor math time: " << clock() - tStart << endl;
}

//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
// Handles the one input operations.
// Handles the ops where eigensystems are computed.
template <class T>
static void vtkTensorMathematicsExecute1Eigen(vtkTensorMathematics *self,
                          vtkImageData *in1Data, 
                          vtkImageData *outData, 
                          T *outPtr,
                          int outExt[6], int id)
{
  // image variables
  int idxR, idxY, idxZ;
  int maxY, maxZ;
  int outIncX, outIncY, outIncZ;
  int inIncX, inIncY, inIncZ;
  int rowLength;
  // progress
  unsigned long count = 0;
  unsigned long target;
  // math operation
  int op = self->GetOperation();
  // tensor variables
  vtkDataArray *inTensors;
  float tensor[3][3];
  vtkPointData *pd;
  int numPts, inPtId;
  // time
  clock_t tStart=0;
  tStart = clock();
  // working matrices
  float *m[3], w[3], *v[3];
  float m0[3], m1[3], m2[3];
  float v0[3], v1[3], v2[3];
  m[0] = m0; m[1] = m1; m[2] = m2; 
  v[0] = v0; v[1] = v1; v[2] = v2;
  int i, j;
  float trace, norm;
  int extractEigenvalues;
  // scaling
  float scaleFactor = self->GetScaleFactor();
  // transformation of tensor orientations for coloring
  vtkTransform *trans = vtkTransform::New();
  int useTransform = 0;

  // find the input region to loop over
  pd = in1Data->GetPointData();
  inTensors = pd->GetTensors();
  numPts = in1Data->GetNumberOfPoints();

  if ( !inTensors || numPts < 1 )
    {
      vtkGenericWarningMacro(<<"No input tensor data to filter!");
      return;
    }

  // find the output region to loop over
  rowLength = (outExt[1] - outExt[0]+1);
  maxY = outExt[3] - outExt[2]; 
  maxZ = outExt[5] - outExt[4];
  target = (unsigned long)((maxZ+1)*(maxY+1)/50.0);
  target++;
  
  // Get increments to march through output data 
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);
  in1Data->GetContinuousIncrements(outExt, inIncX, inIncY, inIncZ);

  //Initialize ptId to walk through tensor volume
  int *inInc,*inFullUpdateExt;
  inInc = in1Data->GetIncrements();
  inFullUpdateExt = in1Data->GetExtent(); //We are only working over the update extent
  inPtId = ((outExt[0] - inFullUpdateExt[0]) * inInc[0]
     + (outExt[2] - inFullUpdateExt[2]) * inInc[1]
     + (outExt[4] - inFullUpdateExt[4]) * inInc[2]);

  // decide whether to extract eigenfunctions or just use input cols
  extractEigenvalues = self->GetExtractEigenvalues();

  // if the user has set this matrix grab it
  if (self->GetTensorRotationMatrix())
    {
      trans->SetMatrix(self->GetTensorRotationMatrix());
      useTransform = 1;
    }

  // Loop through output pixels and input points

  for (idxZ = 0; idxZ <= maxZ; idxZ++)
    {
      for (idxY = 0; idxY <= maxY; idxY++)
    {
      if (!id) 
        {
          if (!(count%target))
        {
          self->UpdateProgress(count/(50.0*target));
        }
          count++;
        }

      for (idxR = 0; idxR < rowLength; idxR++)
        {
          // tensor at this voxel
          inTensors->GetTuple(inPtId,(float *)tensor);

          // get eigenvalues and eigenvectors appropriately
          if (extractEigenvalues) 
        {
          for (j=0; j<3; j++)
            {
              for (i=0; i<3; i++)
            {
              // transpose
              m[i][j] = tensor[j][i];
            }
            }
          // compute eigensystem
          vtkMath::Jacobi(m, w, v);
        }
          else
        {
          // tensor columns are evectors scaled by evals
          for (i=0; i<3; i++)
            {
              v0[i] = tensor[i][0];
              v1[i] = tensor[i][1];
              v2[i] = tensor[i][2];
            }
          w[0] = vtkMath::Normalize(v0);
          w[1] = vtkMath::Normalize(v1);
          w[2] = vtkMath::Normalize(v2);
        }

        //Correct for negative eigenvalues: absolut value
        w[0] = fabs(w[0]);
        w[1] = fabs(w[1]);
        w[2] = fabs(w[2]);
       

          // trace is sum of eigenvalues
          trace = w[0]+w[1]+w[2];

          

          // regularization to compensate for small trace values
          //float r = 0.000001;
          float r = 0.0001;
          
          // we are not interested in regions with trace < r
          int ignore = 0;
          if (trace < r ) 
            ignore = 1;
          
          trace +=r;
          
          // Lauren note that RA and LA could be computed
          // without diagonalization.  This should be implementred
          // instead for speed.

          // pixel operation
          switch (op)
        {
        case VTK_TENS_RELATIVE_ANISOTROPY:
          *outPtr = (T)((0.70710678)*
                (sqrt((w[0]-w[1])*(w[0]-w[1]) + 
                      (w[2]-w[1])*(w[2]-w[1]) +
                      (w[2]-w[0])*(w[2]-w[0])))/trace);
          break;
        case VTK_TENS_FRACTIONAL_ANISOTROPY:
          norm = sqrt(w[0]*w[0]+ w[1]*w[1] +  w[2]*w[2]);
          norm += r;
          *outPtr = (T)((0.70710678)*
                (sqrt((w[0]-w[1])*(w[0]-w[1]) + 
                      (w[2]-w[1])*(w[2]-w[1]) +
                      (w[2]-w[0])*(w[2]-w[0])))/norm);
          break;

        case VTK_TENS_LINEAR_MEASURE:
          *outPtr = (T) ((w[0] - w[1])/trace);
          break;

        case VTK_TENS_PLANAR_MEASURE:
          *outPtr = (T) (2*(w[1] - w[2])/trace);
          break;

        case VTK_TENS_SPHERICAL_MEASURE:
          *outPtr = (T) (3*w[2]/trace);
          break;

        case VTK_TENS_MAX_EIGENVALUE:
          *outPtr = (T)w[0];
          break;

        case VTK_TENS_MID_EIGENVALUE:
          *outPtr = (T)w[1];
          break;

        case VTK_TENS_MIN_EIGENVALUE:
          *outPtr = (T)w[2];
          break;

        case VTK_TENS_COLOR_ORIENTATION:
          if (ignore) 
            {
              memset(outPtr,0,4*sizeof(unsigned char));
              outPtr++;
              outPtr++;
              outPtr++;
            } 
          else
            {
              // map 0..1 values into the range a char takes on
              const int scale = 255;
              
              // If the user has set the rotation matrix
              // then transform the eigensystem first
              if (useTransform)
                {
                 trans->TransformPoint(v0,v0);
                }
              // Color R, G, B depending on max eigenvector
              *outPtr = (T)(scale*fabs(v[0][0]));
              outPtr++;
              *outPtr = (T)(scale*fabs(v[1][0]));
              outPtr++;
              *outPtr = (T)(scale*fabs(v[2][0]));
              outPtr++;
              
              // A: alpha (opacity) depends on anisotropy
              // We want opacity to be less in spherical case.
              // Also in general less when trace is small.
              // 1 = opaque (high anisotropy), 0 = transparent
              
              // this is 1 - spherical anisotropy measure:
              *outPtr = (T)(scale*(1 - 3*w[2]/trace));
              }
           break;

        }

        // we are not interested in regions with trace < r
        if (ignore)
          *outPtr = (T) 0;

          // scale floats if the user requested this
          if (scaleFactor != 1 && op != VTK_TENS_COLOR_ORIENTATION)
        *outPtr = (T) ((*outPtr) * scaleFactor);

//           if (inPtId > numPts) 
//         {
//           vtkGenericWarningMacro(<<"not enough input pts for output extent "<<numPts<<" "<<inPtId);
//         }
          outPtr++;
          inPtId++;
        }
      outPtr += outIncY;
      inPtId += inIncY;
    }
      outPtr += outIncZ;
      inPtId += outIncZ;
    }

  //cout << "tensor math time: " << clock() - tStart << endl;
}

//----------------------------------------------------------------------------
// This method is passed a input and output datas, and executes the filter
// algorithm to fill the output from the inputs.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkTensorMathematics::ThreadedExecute(vtkImageData **inData, 
                      vtkImageData *outData,
                      int outExt[6], int id)
{
  void *outPtr;
  
  vtkDebugMacro(<< "Execute: inData = " << inData 
        << ", outData = " << outData);
  

  if (inData[0] == NULL)
    {
      vtkErrorMacro(<< "Input " << 0 << " must be specified.");
      return;
    }


  outPtr = outData->GetScalarPointerForExtent(outExt);
  
  // single input only for now
  vtkDebugMacro ("In Threaded Execute. scalar type is " << inData[0]->GetScalarType() << "op is: " << this->Operation);

  switch (this->GetOperation()) 
    {
      
      // Operations where eigenvalues are not computed
    case VTK_TENS_D11:
    case VTK_TENS_D22:
    case VTK_TENS_D33:
    case VTK_TENS_TRACE:
    case VTK_TENS_DETERMINANT:
      switch (outData->GetScalarType())
    {
      // we set the output data scalar type depending on the op
      // already.  And we only access the input tensors
      // which are float.  So this switch statement on output
      // scalar type is sufficient.
      vtkTemplateMacro6(vtkTensorMathematicsExecute1,
                this,inData[0], outData, 
                (VTK_TT *)(outPtr), outExt, id);
    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType");
      return;
    }
      break;

      // Operations where eigenvalues are computed      
    case VTK_TENS_RELATIVE_ANISOTROPY:
    case VTK_TENS_FRACTIONAL_ANISOTROPY:
    case VTK_TENS_LINEAR_MEASURE:
    case VTK_TENS_PLANAR_MEASURE:
    case VTK_TENS_SPHERICAL_MEASURE:
    case VTK_TENS_MAX_EIGENVALUE:
    case VTK_TENS_MID_EIGENVALUE:
    case VTK_TENS_MIN_EIGENVALUE:
    case VTK_TENS_COLOR_ORIENTATION:
      switch (outData->GetScalarType())
    {
      vtkTemplateMacro6(vtkTensorMathematicsExecute1Eigen,
                this,inData[0], outData, 
                (VTK_TT *)(outPtr), outExt, id);
    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType");
      return;
    }
      break;
    }

}

void vtkTensorMathematics::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageTwoInputFilter::PrintSelf(os,indent);

  os << indent << "Operation: " << this->Operation << "\n";
}


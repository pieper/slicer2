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
#include <time.h>
#include "vtkVersion.h"
#if (VTK_MAJOR_VERSION == 3 && VTK_MINOR_VERSION == 2)
#include "vtkCommand.h"
#endif
#include "vtkObjectFactory.h"
#include "vtkImageMIReg.h"
#include "vtkImageFastGaussian.h"
#include "vtkImageShrink3D.h"
#include "vtkMrmlVolumeNode.h"
#include "qgauss.h"

//----------------------------------------------------------------------------
// New
//----------------------------------------------------------------------------
vtkImageMIReg* vtkImageMIReg::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageMIReg");
  if(ret)
  {
    return (vtkImageMIReg*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageMIReg;
}

//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
vtkImageMIReg::vtkImageMIReg()
{
  int i;
  
  // Inputs
  this->Reference = NULL;
  this->Subject = NULL;
  this->RefNode = NULL;
  this->SubNode = NULL;
  this->InitialPose = NULL;
  
  // Output
  this->FinalPose = vtkMatrix4x4::New();
  this->CurrentPose = vtkMatrix4x4::New();

  // Workspace
  for (i=0; i < 4; i++) {
    this->Refs[i] = NULL;
    this->Subs[i] = NULL;
    this->RefIjkToRas[i] = vtkMatrix4x4::New();
    this->RefRasToIjk[i] = vtkMatrix4x4::New();
    this->SubRasToIjk[i] = vtkMatrix4x4::New();
  }

  // Params
  this->SampleSize = 50;
  this->SigmaUU    = 2.0f;
  this->SigmaVV    = 2.0f;
  this->SigmaV     = 4.0f;
  this->PMin       = 0.01f;
  this->UpdateIterations = 200;

  // Params per resolution
  //
  // Coursest (smallest image)
  this->NumIterations[0]      = 16000;
  this->LambdaDisplacement[0] = 1.0f;
  this->LambdaRotation[0]     = 0.0005f;
  //
  this->NumIterations[1]      = 4000;
  this->LambdaDisplacement[1] = 0.4;
  this->LambdaRotation[1]     = 0.00008f;
  //
  this->NumIterations[2]      = 4000;
  this->LambdaDisplacement[2] = 0.05f;
  this->LambdaRotation[2]     = 0.000005f;
  //
  // Finest (full size image)
  this->NumIterations[3]      = 4000;
  this->LambdaDisplacement[3] = 0.01f;
  this->LambdaRotation[3]     = 0.000001f;

  this->Reset();
}

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
vtkImageMIReg::~vtkImageMIReg()
{
  int i;
  
  // Inputs
  if (this->Reference != NULL) {
    this->Reference->UnRegister(this);
  }
  if (this->Subject != NULL) {
    this->Subject->UnRegister(this);
  }
  if (this->RefNode != NULL) {
    this->RefNode->UnRegister(this);
  }
  if (this->SubNode != NULL) {
    this->SubNode->UnRegister(this);
  }

  // Transforms
  for (i=0; i<4; i++)
  {
    if (this->RefIjkToRas[i]) {
      this->RefIjkToRas[i]->Delete();
      this->RefIjkToRas[i] = NULL;
    }
    if (this->RefRasToIjk[i]) {
      this->RefRasToIjk[i]->Delete();
      this->RefRasToIjk[i] = NULL;
    }
    if (this->SubRasToIjk[i]) {
      this->SubRasToIjk[i]->Delete();
      this->SubRasToIjk[i] = NULL;
    }
  }

  // Outputs
  // Signal that we're no longer using them
  if (this->InitialPose != NULL) {
    this->InitialPose->UnRegister(this);
  }
  if (this->FinalPose != NULL) {
    this->FinalPose->UnRegister(this);
  }
  if (this->CurrentPose != NULL) {
    this->CurrentPose->UnRegister(this);
  }
}

//----------------------------------------------------------------------------
// Reset
//
// Call before the Update iterations
//----------------------------------------------------------------------------
void vtkImageMIReg::Reset()
{
  // Reset
  this->CurIteration[0] = 0;
  this->CurIteration[1] = 0;
  this->CurIteration[2] = 0;
  this->CurIteration[3] = 0;
  this->RunTime = 0;
  this->InProgress = 0;
}

//----------------------------------------------------------------------------
// Update
//
// Written with vtkImageIteratedFilter as an example.
//----------------------------------------------------------------------------
void vtkImageMIReg::Update() 
{
  // Ensure inputs exist
  if (!this->Reference) {
    vtkErrorMacro("vtkImageMIReg::Update: Reference Image not set");
    return;
  }
  if (!this->Subject) {
    vtkErrorMacro("vtkImageMIReg::Update: Subject Image not set");
    return;
  }

  // If inputs or parameters changed, then reset
  if (this->GetMTime() > this->UTime.GetMTime())
  {
    // Update upstream pipeline
    this->Reference->Update();
    this->Subject->Update();

    // Prepare to execute
    this->Reset();
    this->InProgress = 1;
    clock_t tStart = clock();
    if (this->Initialize()) return;
    this->RunTime += clock() - tStart;

    // Don't enter this loop again until something changes
    this->UTime.Modified();
    return;
  }

  // If iteration is in progress, continue executing
  if (this->InProgress) 
  {
    clock_t tStart = clock();
    this->Execute();
    this->RunTime += clock() - tStart;

    // If that ends it, de-allocate memory
    if (!this->InProgress) {
      this->Cleanup();
    }
  }
}

//----------------------------------------------------------------------------
// Initialize
//
// Returns -1 if unable to continue, else 0.
//----------------------------------------------------------------------------
int vtkImageMIReg::Initialize()
{
  int i, *ext;
  vtkImageFastGaussian *smooth;
  vtkImageShrink3D *down;
  vtkMrmlVolumeNode *node = vtkMrmlVolumeNode::New();
  vtkMatrix4x4 *identity = vtkMatrix4x4::New();
  float *spacing = this->Subject->GetSpacing();

  //
  // Validate inputs
  //
  if (this->Reference->GetScalarType() != VTK_SHORT) {
    vtkErrorMacro(<<"Reference scalar type must be short."); 
    return -1;
  }
  if (this->Reference->GetNumberOfScalarComponents() != 1) {
    vtkErrorMacro(<<"Reference must have 1 component."); 
    return -1;
  }
  if (this->Subject->GetScalarType() != VTK_SHORT) {
    vtkErrorMacro(<<"Subject scalar type must be short."); 
    return -1;
  }
  if (this->Subject->GetNumberOfScalarComponents() != 1) {
    vtkErrorMacro(<<"Subject must have 1 component."); 
    return -1;
  }
  if (this->RefNode == NULL) {
    vtkErrorMacro(<<"No Reference node."); 
    return -1;
  }
  if (this->SubNode == NULL) {
    vtkErrorMacro(<<"No Subject node."); 
    return -1;
  }
  if (!spacing[0] || !spacing[1] || !spacing[2]) {
    vtkErrorMacro(<<"Subject Spacing can't be 0, for gradient purposes.");
    return -1;
  }

  // If no matrices, allocate them
  if (!this->InitialPose) {
    vtkMatrix4x4 *initPose = vtkMatrix4x4::New();
    initPose->Identity();
    this->SetInitialPose(initPose);
  }

  // Highest resolution (3) corresponds to the external input images.
  this->Refs[3] = this->Reference;
  this->Refs[3]->Register((vtkObject*)NULL);
  this->Subs[3] = this->Subject;
  this->Subs[3]->Register((vtkObject*)NULL);
  this->RefRasToIjk[3]->DeepCopy(this->RefNode->GetRasToIjk());
  this->RefIjkToRas[3]->DeepCopy(this->RefRasToIjk[3]);
  this->RefIjkToRas[3]->Invert();
  this->SubRasToIjk[3]->DeepCopy(this->SubNode->GetRasToIjk());

  //
  // Downsample images to create various resolutions
  //
  if (this->NumIterations[0] || this->NumIterations[1] || this->NumIterations[2])
  {
    for (i=2; i >= 0; i--)
    {
      //
      // Reference image
      //

      // Smooth image
      smooth = vtkImageFastGaussian::New();
      smooth->SetInput(this->Refs[i+1]);
      smooth->Modified(); 
      smooth->Update();

      // Downsample image
      down = vtkImageShrink3D::New();
      down->SetMean(0);
      down->SetShrinkFactors(2,2,2);
      down->SetInput(smooth->GetOutput()); 
      down->Modified(); 
      down->Update();

      // Attach output to me
      this->Refs[i] = down->GetOutput();
      this->Refs[i]->Register((vtkObject*)NULL);

      // Detach output from source
      this->Refs[i]->SetSource(NULL);
      smooth->SetOutput(NULL);
      smooth->Delete();
      down->SetOutput(NULL);
      down->Delete();

      // Compute Ijk-To-Ras matrices for downsampled image
      ext = Refs[i]->GetExtent();
      node->SetImageRange(ext[4], ext[5]);
      node->SetDimensions(ext[1]-ext[0]+1, ext[3]-ext[2]+1);
      spacing = this->Refs[i]->GetSpacing();
      node->SetSpacing(spacing);
      node->ComputeRasToIjkFromScanOrder(this->RefNode->GetScanOrder());
      node->SetRasToWld(identity);
      this->RefRasToIjk[i]->DeepCopy(node->GetRasToIjk());
      this->RefIjkToRas[i]->DeepCopy(this->RefRasToIjk[i]);
      this->RefIjkToRas[i]->Invert();

      //
      // Subject image
      //

      // Smooth image
      smooth = vtkImageFastGaussian::New();
      smooth->SetInput(this->Subs[i+1]);
      smooth->Modified(); 
      smooth->Update();

      // Downsample image
      down = vtkImageShrink3D::New();
      down->SetMean(0);
      down->SetShrinkFactors(2,2,2);
      down->SetInput(smooth->GetOutput()); 
      down->Modified(); 
      down->Update();

      // Attach output to me
      this->Subs[i] = down->GetOutput();
      this->Subs[i]->Register((vtkObject*)NULL);

      // Detach output from source
      this->Subs[i]->SetSource(NULL);
      smooth->SetOutput(NULL);
      smooth->Delete();
      down->SetOutput(NULL);
      down->Delete();

      // Compute Ras-to-Ijk matrices for downsampled image
      ext = Subs[i]->GetExtent();
      node->SetImageRange(ext[4], ext[5]);
      node->SetDimensions(ext[1]-ext[0]+1, ext[3]-ext[2]+1);
      node->SetSpacing(this->Subs[i]->GetSpacing());
      node->ComputeRasToIjkFromScanOrder(this->SubNode->GetScanOrder());
      node->SetRasToWld(identity);
      this->SubRasToIjk[i]->DeepCopy(node->GetRasToIjk());
    }
  }

  // Initialize Pose
  this->CurrentPose->DeepCopy(this->InitialPose);

  identity->Delete();
  node->Delete();
  return 0;
}


//----------------------------------------------------------------------------
// Cleanup
//----------------------------------------------------------------------------
void vtkImageMIReg::Cleanup()
{
  int i;

  // Deallocate voxels of downsampled images
  for (i=0; i<4; i++) 
  {
    this->Refs[i]->UnRegister(this);
    this->Refs[i] = NULL;
    this->Subs[i]->UnRegister(this);
    this->Subs[i] = NULL;
  }
}

//----------------------------------------------------------------------------
// RandomCoordinate
//
// Draw a random integer data coordinate from an image.
// Express as 0-based IJK, not extent-based.
//----------------------------------------------------------------------------
static void RandomIjkCoordinate(double *B, vtkImageData *data)
{
  int *ext = data->GetExtent();
  
  B[0] = (int)(vtkMath::Random() * (float)(ext[1]-ext[0]+1)); 
  B[1] = (int)(vtkMath::Random() * (float)(ext[3]-ext[2]+1)); 
  B[2] = (int)(vtkMath::Random() * (float)(ext[5]-ext[4]+1)); 
  B[4] = 1;
}

//----------------------------------------------------------------------------
// IjkToRasGradient
//
// sl = Slice order
//----------------------------------------------------------------------------
static void IjkToRasGradient(double *ras, double *ijk, char  *sl)
{
  // Apply slice orientation
  if (!strcmp(sl, "SI") || !strcmp(sl, "IS"))
  {
    // Axial: (r,a,s) = (-x,-y,z)
    ras[0] = -ijk[0];
    ras[1] = -ijk[1];
    ras[2] =  ijk[2];
  }
  else if (!strcmp(sl, "LR") || !strcmp(sl, "RL"))
  {
    // Saggital: (r,a,s) = (z,-x,-y)
    ras[0] =  ijk[2]; // Send  z to x
    ras[1] = -ijk[0]; // Send -x to y
    ras[2] = -ijk[1]; // Send -y to z
  }
  else
  {
    // Coronal: (r,a,s) = (-x,z,-y)
    ras[0] = -ijk[0]; // Send -x to x
    ras[1] =  ijk[2]; // Send  z to y
    ras[2] = -ijk[1]; // Send -y to z
  }
}
  
//----------------------------------------------------------------------------
// ImageGradientInterpolation (templated)
//----------------------------------------------------------------------------
template <class T>
static void ImageGradientInterpolation(vtkImageData *data, 
  vtkMatrix4x4 *rasToIjk, double *grad, double *ras, T *inPtr, double *value)
{
  double ijk[3];
    double x, y, z, x0, y0, z0, x1, y1, z1, dx0, dx1, dxy0, dxy1;
  int xi, yi, zi, nx, ny, nz, nxy, idx;
    double v000, v001, v010, v011, v100, v101, v110, v111;
  float sx, sy, sz, *spacing = data->GetSpacing();
  T *ptr;
  int *ext = data->GetExtent();
  nx = ext[1]-ext[0]+1;
  ny = ext[3]-ext[2]+1;
  nz = ext[5]-ext[4]+1;
  nxy = nx * ny;

  // Get spacing
  sx = 1.0 / (double)spacing[0];
  sy = 1.0 / (double)spacing[1];
  sz = 1.0 / (double)spacing[2];

  // Convert from RAS space (mm) to IJK space (indices)
  rasToIjk->MultiplyPoint(ras, ijk);
  x = ijk[0];
  y = ijk[1];
  z = ijk[2];

  // Compute integer parts of volume coordinates
  xi = (int)x;
  yi = (int)y;
  zi = (int)z;

  // Test if coordinates are outside volume
  if ((xi < 0   ) || (yi < 0   ) || (zi < 0   ) ||
            (xi > nx-2) || (yi > ny-2) || (zi > nz-1))
  {
    // Out of bounds
    memset(grad, 0, sizeof(double));
        *value = 0;
    }
  // Handle the case of being on the last slice
  else if (zi == nz-1)
  {
    x1 = x - (double)xi;
    y1 = y - (double)yi;
 
    x0 = 1.0 - x1;
    y0 = 1.0 - y1;

    // Get values of 4 nearest neighbors
    idx = zi*nxy + yi*nx + xi;
    ptr = &inPtr[idx];
    v000 = ptr[0];
    v100 = ptr[1]; ptr += nx;
    v010 = ptr[0];
    v110 = ptr[1];

    // Interpolate in X and Y at Z0
    dx0 = x0*v000 + x1*v100;
    dx1 = x0*v010 + x1*v110;
    *value = y0*dx0 + y1*dx1;
    
    // Gradient
    grad[0] = ((v100 - v000) + (v110 - v010)) * sx * 0.5;
      grad[1] = ((v010 - v000) + (v110 - v100)) * sy * 0.5;
    grad[2] = 0;
  }
  else 
  {
    x1 = x - (double)xi;
    y1 = y - (double)yi;
    z1 = z - (double)zi;
 
    x0 = 1.0 - x1;
    y0 = 1.0 - y1;
    z0 = 1.0 - z1;

    // Get values of 8 nearest neighbors
    idx = zi*nxy + yi*nx + xi;
    ptr = &inPtr[idx];
    v000 = ptr[0];
    v100 = ptr[1]; ptr += nx;
    v010 = ptr[0];
    v110 = ptr[1];
    ptr = &inPtr[idx + nxy];
    v001 = ptr[0];
    v101 = ptr[1]; ptr += nx;
    v011 = ptr[0];
    v111 = ptr[1];

    // Interpolate in X and Y at Z0
    dx0 = x0*v000 + x1*v100;
    dx1 = x0*v010 + x1*v110;
    dxy0 = y0*dx0 + y1*dx1;

    // Interpolate in X and Y at Z1
    dx0 = x0*v001 + x1*v101;
    dx1 = x0*v011 + x1*v111;
    dxy1 = y0*dx0 + y1*dx1;

    // Interpolate in Z
    *value = z0*dxy0 + z1*dxy1;

    // Gradient
    grad[0] = ((v100-v000)+(v110-v010)+(v101-v001)+(v111-v011))*sx*0.25;
      grad[1] = ((v010-v000)+(v110-v100)+(v101-v001)+(v111-v011))*sy*0.25;
      grad[2] = ((v001-v000)+(v101-v100)+(v011-v010)+(v111-v110))*sz*0.25;
  }
}

//----------------------------------------------------------------------------
// GetGradientAndInterpolation
//
// Computes the non-interpolated gradient (rasGrad) at an RAS point (ras)
// in an image (data) with an RasToIjk matrix.
// Returns the interpolated value at that point.
//----------------------------------------------------------------------------
double vtkImageMIReg::GetGradientAndInterpolation(double *rasGrad, 
  vtkImageData *data, vtkMatrix4x4 *rasToIjk, double *ras, char *sliceOrder)
{
  float *spacing = data->GetSpacing();
  void *inPtr = data->GetScalarPointer();
  double value=0;
  double ijkGrad[3];
  
  switch (data->GetScalarType()) {
    vtkTemplateMacro6(ImageGradientInterpolation, data, rasToIjk, ijkGrad, ras, 
      (VTK_TT *)inPtr, &value);
     default:
        vtkErrorMacro("Execute: Unknown ScalarType");
    }

  // Convert from IJK-gradient to RAS-gradient by permutation
  IjkToRasGradient(rasGrad, ijkGrad, sliceOrder);

  return value;
}

//----------------------------------------------------------------------------
// Execute
//
// Finds the V(T(x)) associated with U(x).
// Effectively, consider T was applied to U to produce V.
// Therefore, apply inv(T) to V to align with U.
//
// This procedure computes T and returns its inverse.
//----------------------------------------------------------------------------
void vtkImageMIReg::Execute() 
{
  int i, j, i3, j3, res, SS = this->SampleSize, numIter=0;
    float inv_sigma_uu   = 1.0f / this->SigmaUU;
    float inv_sigma_vv   = 1.0f / this->SigmaVV;
    float inv_sigma_v    = 1.0f / this->SigmaV;
    float inv_sigma_v_2  = inv_sigma_v * inv_sigma_v;
    float inv_sigma_vv_2 = inv_sigma_vv * inv_sigma_vv;
  float pMin = this->PMin;
  float sum, denom, inv_denom;
  double left_part, lambda_d, lambda_r;
  vtkImageData *ref, *subj;
  double dIdd[3], dIdr[3], d[3], r[3], q[4];
  vtkMatrix4x4* refIjkToRas;
  vtkMatrix4x4* subRasToIjk;
  double r1[3][3], r2[3][3], tr[3][3], orth[3][3], td[3];
  int y, x;
  char *subSliceOrder = this->SubNode->GetScanOrder();

  //
  // Allocate workspace
  //
  double    B[4];                   // Sample coordinates of ref (ijk)
  double    X[4];                   // Sample coordinates of ref (xyz)
  double    Tx[4];                  // Transformed sample coordinates
  double*   dVdd = new double[SS*3];
  double*   dVdr = new double[SS*3]; 
  float*    U    = new float[SS];   // Sample data
  float*    V    = new float[SS];    
  float**   W_uv = new float*[SS]; 
  float**   W_v  = new float*[SS]; 
  for (i=0; i < SS; i++) {
    W_uv[i] = new float[SS];
    W_v[i]  = new float[SS];
  }

  vtkMath::RandomSeed(time(NULL));

  // Foreach image resolution
  for (res=this->GetResolution(); res < 4; res++)
  {
    // Fetch params for this resolution
    lambda_d    = this->LambdaDisplacement[res];
    lambda_r    = this->LambdaRotation[res];
    ref         = this->Refs[res];
    subj        = this->Subs[res];
    refIjkToRas = this->RefIjkToRas[res];
    subRasToIjk = this->SubRasToIjk[res];

    // Iterate
    for (; this->CurIteration[res] < this->NumIterations[res] && 
        numIter < this->UpdateIterations; 
        numIter++, this->CurIteration[res]++) 
    {
      for (int g=0; g<1000000; g++);
      /* NEED TO FIX BUG IN GetGradientAndInterpolation()
        //
        // Do some O(n) stuff..
        //
        for (i=0; i < SS; i++) 
      {
          // Choose coordinates of samples at random. Expresses as voxel indices (ijk).
        RandomIjkCoordinate(B, ref);

          // Lookup scalar values of Reference at sample coordinates
          U[i] = ref->GetScalarComponentAsFloat(B[0], B[1], B[2], 0);

          // Express coordinates in millimeter space (xyz).
        refIjkToRas->MultiplyPoint(B, X);

          // Transform the sample coordinates by the current pose
          this->CurrentPose->MultiplyPoint(X, Tx);

          // Lookup scalar values of Subject at transformed sample coordinates
          // Lookup gradient of Subject at transformed sample coordinates
          V[i] = (float)GetGradientAndInterpolation(
          &dVdd[i*3], subj, subRasToIjk, Tx, subSliceOrder);

          // Pre-calculate the differential rotation increments
        //   dVdr = Tx x dVdd
          vtkMath::Cross(Tx, &dVdd[i*3], &dVdr[i*3]);
        }

      //
        // Now, the O(n^2) stuff...
        // Calculate the weight matrices: W_v[i][j] and W_uv[i][j]
        // First, just fill them with the Gaussians...
        //
        for (i=0; i < SS; i++) 
      {
        for (j=0; j <= i; j++) 
        {
            W_v[i][j]  = W_v[j][i]  = qgauss(inv_sigma_v, V[i] - V[j]);

          W_uv[i][j] = W_uv[j][i] = qgauss(inv_sigma_uu, U[i] - U[j]) 
                * qgauss(inv_sigma_vv, V[i] - V[j]);
        }
      }

        // ...then, normalize them.
        for (i=0; i < SS; i++) 
      {
          // Normalize W_v
        sum=0;
          for (j=0; j < SS; j++) {
          sum += W_v[i][j];
        }
          denom = sum - W_v[i][i]; // Don't include W_v[i][i] which is a gaussian of zero
          inv_denom = 1.0f / (denom + pMin);
          for (j=0; j < SS; j++) {
          W_v[i][j] *= inv_denom;
        }

          // Normalize W_uv
        sum=0;
        for (j=0; j < SS; j++) {
          sum += W_uv[i][j];
        }
          denom = sum - W_uv[i][i];
          inv_denom = 1.0f / (denom + pMin);
        for (j=0; j < SS; j++) {
          W_uv[i][j] *= inv_denom;
        }
      }

        // Finally, calculate the transformation update.  First zero it,
        memset(dIdd, 0, 3*sizeof(double));
        memset(dIdr, 0, 3*sizeof(double));

      // Next, accumulate the double sum:
      for (i=0; i < SS; i++) {
        for (j=0; j < SS; j++) {
            if (i != j) {
              left_part = (double)((V[i] - V[j]) * 
              (inv_sigma_v_2 * W_v[i][j] - inv_sigma_vv_2 * W_uv[i][j]));

            // The following block is a BIG speed-up of the next 2 lines.
            // Using 1D arrays proved faster than 2D arrays.
            // dIdd += (dVdd[i] - dVdd[j]) * left_part;
              // dIdr += (dVdr[i] - dVdr[j]) * left_part;

            i3 = i*3;
            j3 = j*3;
            dIdd[0] += (dVdd[i3+0] - dVdd[j3+0]) * left_part;
            dIdd[1] += (dVdd[i3+1] - dVdd[j3+1]) * left_part;
            dIdd[2] += (dVdd[i3+2] - dVdd[j3+2]) * left_part;

            dIdr[0] += (dVdr[i3+0] - dVdr[j3+0]) * left_part;
            dIdr[1] += (dVdr[i3+1] - dVdr[j3+1]) * left_part;
            dIdr[2] += (dVdr[i3+2] - dVdr[j3+2]) * left_part;
            }
        }
      }

        // Then normalize it,
        dIdd[0] *= (1.0f / SS);
        dIdd[1] *= (1.0f / SS);
        dIdd[2] *= (1.0f / SS);

        dIdr[0] *= (1.0f / SS);
        dIdr[1] *= (1.0f / SS);
        dIdr[2] *= (1.0f / SS);

        //
      // Update the transform...
      //

        //  Calculate the small rotation and translation
      d[0] = dIdd[0] * lambda_d;
      d[1] = dIdd[1] * lambda_d;
      d[2] = dIdd[2] * lambda_d;
        
      r[0] = dIdr[0] * lambda_r;
      r[1] = dIdr[1] * lambda_r;
      r[2] = dIdr[2] * lambda_r;
    
        // Convert the small rotation, r, to a quaternion, q, 
      // using small-angle linear approximation
      r[0] *= 0.5;
      r[1] *= 0.5;
      r[2] *= 0.5;

      q[0] = 1.0;
      q[1] = r[0];
      q[2] = r[1]; 
      q[3] = r[2];

              // Compute the update for the big translation
        // and compound the small and large rotation quaternions
        // insuring that it doesn't drift from being a valid rotation.
      // delta = the small change in pose, 
      //   where pose is a quaternion q and displacement vector d.
      //
      // Set the pose to be the concatenation of poses: perform Current first, then delta.
      //
      // Concatenate rotations by multiplication, translations by addition:
      //
      // d = concat(r1*d2, d1) = r1*d2 + d1
      // r = concat(r1   , r2) = r2 * r1
      //
      vtkMath::QuaternionToMatrix3x3(q, r2);

      for (y=0; y<3; y++)
        for (x=0; x<3; x++)
          r1[y][x] = this->CurrentPose->GetElement(y,x);

      vtkMath::Multiply3x3(r2, r1, tr);
      vtkMath::Orthogonalize3x3(tr, orth);

      for (y=0; y<3; y++)
        for (x=0; x<3; x++)
          this->CurrentPose->SetElement(y,x,orth[y][x]);

      vtkMath::Multiply3x3(r1, d, td);

      this->CurrentPose->SetElement(0,3,this->CurrentPose->GetElement(0,3) + td[0]);
      this->CurrentPose->SetElement(1,3,this->CurrentPose->GetElement(1,3) + td[1]);
      this->CurrentPose->SetElement(2,3,this->CurrentPose->GetElement(2,3) + td[2]);
      */
    }
  }
     
  // Are we there yet? (for good)
  if (this->CurIteration[3] >= this->NumIterations[3]) 
  {
    this->InProgress = 0;

    this->FinalPose->DeepCopy(this->CurrentPose);
  }

  //
  // Cleanup
  //
  for (i=0; i < SS; i++) {
    delete [] W_uv[i];
    delete [] W_v[i];
  }
  delete [] U;
  delete [] V;    
  delete [] dVdd;
  delete [] dVdr; 
  delete [] W_uv; 
  delete [] W_v; 
}

//----------------------------------------------------------------------------
// GetResolution
//
// Returns the index [0,3] of the resolution level we're currently at.
// If not InProgress, returns -1.
//----------------------------------------------------------------------------
int vtkImageMIReg::GetResolution()
{
  if (!this->InProgress) return -1;
  
  int i;
  for (i=0; i<3; i++) {
    if (this->CurIteration[i] < this->NumIterations[i])
      break;
  }
  return i;
}

//----------------------------------------------------------------------------
// GetIteration
//
// Returns the current iteration number of the current resolution.
// Returns -1 if not InProgress.
//----------------------------------------------------------------------------
int vtkImageMIReg::GetIteration()
{
  if (!this->InProgress) return -1;
  
  return this->CurIteration[this->GetResolution()];
}

//----------------------------------------------------------------------------
// PrintSelf
//----------------------------------------------------------------------------
void vtkImageMIReg::PrintSelf(ostream& os, vtkIndent indent)
{
  // Images
  os << indent << "Reference:     " << this->Reference << "\n";
  if (this->Reference)
  {
    this->Reference->PrintSelf(os,indent.GetNextIndent());
  }
  os << indent << "Subject:     " << this->Subject << "\n";
  if (this->Subject)
  {
    this->Subject->PrintSelf(os,indent.GetNextIndent());
  }

  // Poses
  os << indent << "InitialPose:     " << this->InitialPose << "\n";
  if (this->InitialPose)
  {
    this->InitialPose->PrintSelf(os,indent.GetNextIndent());
  }
  os << indent << "FinalPose:     " << this->FinalPose << "\n";
  if (this->FinalPose)
  {
    this->FinalPose->PrintSelf(os,indent.GetNextIndent());
  }
  os << indent << "CurrentPose:     " << this->CurrentPose << "\n";
  if (this->CurrentPose)
  {
    this->CurrentPose->PrintSelf(os,indent.GetNextIndent());
  }
}


// .NAME vtkImageTensorDiffusion - Volumetric textures derived from
// tensor data.  
// .SECTION Description This class produces scalar volumes (textures)
// for tensor visualization.  By simulating diffusion of an input
// scalar volume in the input tensor field, "blobs" or "swirls" are
// generated.  Reaction-diffusion or simple diffusion can be used.
// The input volume defaults to noise if not set.
//
//
// Input volume contains initial conditions: placement
// of heat, or could be noise to show patterns
// in tensor data.
//
// Setting the InputSource will produce sources/sinks in the system.
// So if the Input is all 0's, you can simulate diffusion 
// away from a source, where initially the concentration everywhere
// is 0.
#ifndef __vtkImageTensorDiffusion_h
#define __vtkImageTensorDiffusion_h

#include "vtkDTMRIConfigure.h"
#include "vtkImageIterateFilter.h"
#include "vtkImageData.h"

class VTK_DTMRI_EXPORT vtkImageTensorDiffusion : public vtkImageIterateFilter
{
public:
  static vtkImageTensorDiffusion *New();
  vtkTypeMacro(vtkImageTensorDiffusion,vtkImageIterateFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Tensor field input
  // (This could be a multiple input filter but it really needs
  // to iterate instead.)
  vtkSetObjectMacro(InputTensor, vtkImageData);

  // Description:
  // internal use
  vtkGetObjectMacro(InputTensor, vtkImageData);

  // Description:
  // Input volume contains initial conditions: placement
  // of heat, or could be noise to show patterns
  // in tensor data.
  //
  vtkSetObjectMacro(InputSource, vtkImageData);

  // Description:
  // internal use
  vtkGetObjectMacro(InputSource, vtkImageData);

  // Description:
  // Sets the number of cycles in the diffusion simulation.
  void SetNumberOfIterations(int num);

  // Lauren what we want is optional diffusion equations that
  // can be applied, like vtkImageReslice does.  Then you
  // can do reaction diffusion or whatever
  //
  // Lauren also want varied boundary conditions, 0 or matching or wrap

  // Description:
  // Speed of the simulation
  vtkSetMacro(S,float);
  vtkGetMacro(S,float);

  // Description:
  // Reaction vs diffusion: higher K is more reaction
  vtkSetMacro(K,float);
  vtkGetMacro(K,float);

  // Description:
  // Diffusivity of substance a
  vtkSetMacro(Da,float);
  vtkGetMacro(Da,float);

  // Description:
  // Diffusivity of substance b
  vtkSetMacro(Db,float);
  vtkGetMacro(Db,float);

  // Description:
  // Initial concentration of substances a and b
  vtkSetMacro(InitialConcentration,float);
  vtkGetMacro(InitialConcentration,float);

  // Description:
  // for testing
  vtkSetMacro(Test,float);
  vtkGetMacro(Test,float);

  virtual void IterativeExecuteData(vtkImageData *in, vtkImageData *out) 
    { this->MultiThread(in,out);};

protected:
  vtkImageTensorDiffusion();
  ~vtkImageTensorDiffusion() {};
  vtkImageTensorDiffusion(const vtkImageTensorDiffusion&) {};
  void operator=(const vtkImageTensorDiffusion&) {};


  float S;
  float K;
  float Da;
  float Db;
  float InitialConcentration;
  float Test;

  vtkImageData *InputTensor;
  vtkImageData *InputSource;

  void ComputeInputUpdateExtent(int inExt[6], int outExt[6] );
  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData,
		       int outExt[6], int id);
};

#endif




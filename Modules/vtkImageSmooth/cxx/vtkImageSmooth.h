#ifndef __vtkImageSmooth_h
#define __vtkImageSmooth_h 



#include <vtkImageSmoothConfigure.h>
#include <vtkImageToImageFilter.h>


class VTK_IMAGESMOOTH_EXPORT vtkImageSmooth : public vtkImageToImageFilter
{
 public:
  // -----------------------------------------------------
  // Genral Functions for the filter
  // -----------------------------------------------------
  static vtkImageSmooth *New();

  vtkTypeMacro(vtkImageSmooth,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Get/Set the number of iterations of smoothing that is to be done
  vtkSetMacro(NumberOfIterations,int);
  vtkGetMacro(NumberOfIterations,int);
  
 
  void ComputeInputUpdateExtent(int inExt[6], 
                int outExt[6]);

  
  void ExecuteInformation(vtkImageData *inData, 
                                     vtkImageData *outData);
  
  float Init();
 
  //Data
  int NumberOfIterations;
  float dt;

  protected:
  vtkImageSmooth();
  ~vtkImageSmooth();
  /*
  float Smooth2D(vtkImageData *inData, vtkImageData *outData,int inExt[6],
                int outExt[6]);
  float Smooth3D(vtkImageData *inData, vtkImageData *outData,int inExt[6],
                int outExt[6]);
*/

  void ThreadedExecute(vtkImageData *inData, 
                                  vtkImageData *outData,
                                  int outExt[6], int threadid);


  
  
};

#endif

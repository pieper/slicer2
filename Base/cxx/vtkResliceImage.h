// .NAME vtkDeformImage - Deforms an image according to a tetrahedral mesh
// .SECTION Description
// This filter takes in a volume, and resamples it in the coordinates of
// the next volume. A transform that takes the coordinates (mm) in the image
// whose coordinates we will use into the input image coordinates (mm) is 
// possible.
//
// Please note that this program (by Samson Timoner) is also in his
// personal CVS tree. If you make changes, please let him know.
//

#ifndef __vtkDeformImage_h
#define __vtkDeformImage_h

class vtkMatrix4x4;
class vtkUnstructuredGrid;

#include "vtkImageToImageFilter.h"
#include "vtkUnstructuredGrid.h"
#include "vtkMatrix4x4.h"

class VTK_EXPORT vtkResliceImage : public vtkImageToImageFilter
{
public:
  // Description:
  // Default:: No transform between the coordinate systems
  // Output Extent of 1 pixel, spacing of (1,1,1), and origin of (0,0,0)
  static vtkResliceImage *New();
  vtkTypeMacro(vtkResliceImage,vtkImageToImageFilter);

 // Description:
  // The output image will have the same spacing and origin and extent
  // as this Volume.
  void SetOutputImageParam(vtkImageData *VolumeToCopyParam);

  void PrintSelf(ostream& os, vtkIndent indent);


  // Description:
  // Set the transformation between the first and final images
  // This transform takes points in the output volume's continuous coordinates
  // (mm) and maps them to the input volume's continuous coordinates (mm).
  // If set to NULL, the transform is the identity.
  vtkSetObjectMacro(TransformOutputToInput,vtkMatrix4x4);
  vtkGetObjectMacro(TransformOutputToInput,vtkMatrix4x4);
 

  // Description:
  // Set the background scalar to use if there is no 
  // information in the first image in the area specified.
  vtkSetMacro(Background,float);
  vtkGetMacro(Background,float);

  // Helper Functions:
  //

  //BTX
  static void FindInputIJK(float OtherIJK[4],
                           vtkMatrix4x4 *IJKtoIJK,
                           int i, int j, int k);

  static vtkMatrix4x4 *GetIJKtoIJKMatrix(float Spacing2[3],
                                         float Origin2[3],
                                         vtkMatrix4x4 *MM2toMM1,
                                         float Spacing1[3],
                                         float Origin1[3]);
protected:

  vtkResliceImage();
  ~vtkResliceImage() {};
  vtkResliceImage(const vtkResliceImage&) {};
  void operator=(const vtkResliceImage&) {};

  vtkMatrix4x4     *TransformOutputToInput;
  vtkMatrix4x4     *IJKtoIJK;
  float OutSpacing[3];
  float OutOrigin[3];
  int OutExtent[6];
  float Background;

  void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);
  void ComputeInputUpdateExtent(int inExt[6], int outExt[6]);
  void ExecuteInformation(){this->vtkImageToImageFilter::ExecuteInformation();};
  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData,
		       int outExt[6], int id);
  //ETX
};
#endif /* DeformImage_h */

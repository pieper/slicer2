#ifndef __vtkVoxelTimeCourseExtractor_h
#define __vtkVoxelTimeCourseExtractor_h

#include "vtkIbrowserConfigure.h"
#include "vtkFloatArray.h"
#include "vtkSimpleImageToImageFilter.h"

class  VTK_IBROWSER_EXPORT vtkVoxelTimeCourseExtractor : public vtkSimpleImageToImageFilter
{
 public:
    static vtkVoxelTimeCourseExtractor *New();
    vtkTypeMacro(vtkVoxelTimeCourseExtractor, vtkSimpleImageToImageFilter);

    // Description:
    // Returns the time course of a specified voxel (i, j, k).
    vtkFloatArray *GetTimeCourse(int i, int j, int k);
    vtkFloatArray *GetFloatTimeCourse(int i, int j, int k);
    vtkImageData *GetInput (int volNum);
    void AddInput(vtkImageData *input);
    
    float outputMax;
    float outputMin;
    float outputRange;
    float numInputs;

 protected:
    void SimpleExecute (vtkImageData* input, vtkImageData* output);
    vtkVoxelTimeCourseExtractor();
    ~vtkVoxelTimeCourseExtractor();

 private:
};


#endif

// .NAME vtkImageHistogramNormalization - 
// .SECTION Description

#ifndef __vtkImageHistogramNormalization_h
#define __vtkImageHistogramNormalization_h

#include <vtkAGConfigure.h>


#include "vtkImageShiftScale.h"

class VTK_AG_EXPORT vtkImageHistogramNormalization : public vtkImageShiftScale
{
public:
  static vtkImageHistogramNormalization *New();
  vtkTypeMacro(vtkImageHistogramNormalization,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkImageHistogramNormalization();
  ~vtkImageHistogramNormalization();
  vtkImageHistogramNormalization(const vtkImageHistogramNormalization&);
  void operator=(const vtkImageHistogramNormalization&);
  
  void ExecuteData(vtkDataObject *output);
};

#endif

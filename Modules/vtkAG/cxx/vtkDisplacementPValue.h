// .NAME vtkDisplacementPValue - 
// 
// .SECTION Description
// 
// .SECTION See Also

#ifndef __vtkDisplacementPValue_h
#define __vtkDisplacementPValue_h

#include <vtkAGConfigure.h>

// #include <vtkImageGradient.h>
#include <vtkImageTwoInputFilter.h>

class VTK_AG_EXPORT vtkDisplacementPValue : public vtkImageMultipleInputFilter
{
public:
  static vtkDisplacementPValue* New();
  vtkTypeMacro(vtkDisplacementPValue,vtkImageMultipleInputFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetMacro(NumberOfSamples, int);
  vtkGetMacro(NumberOfSamples, int);

  virtual void SetMean(vtkImageData *input)
  {
    this->SetInput(0, input);
  }
  vtkImageData* GetMean();
  
  virtual void SetSigma(vtkImageData *input)
  {
    this->SetInput(1, input);
  }
  vtkImageData* GetSigma();
  
  virtual void SetDisplacement(vtkImageData *input)
  {
    this->SetInput(2, input);
  }
  vtkImageData* GetDisplacement();

protected:
  vtkDisplacementPValue();
  ~vtkDisplacementPValue();
  vtkDisplacementPValue(const vtkDisplacementPValue&);
  void operator=(const vtkDisplacementPValue&);
  void ExecuteInformation(vtkImageData **inDatas, vtkImageData *outData);
  void ThreadedExecute(vtkImageData **inDatas, vtkImageData *outData,
		       int extent[6], int id);

  int NumberOfSamples;
};
#endif

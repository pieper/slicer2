// .NAME vtkDistributionPValue - 
// 
// .SECTION Description
// 
// .SECTION See Also

#ifndef __vtkDistributionPValue_h
#define __vtkDistributionPValue_h

#include <vtkAGConfigure.h>

// #include <vtkImageGradient.h>
#include <vtkImageTwoInputFilter.h>

class VTK_AG_EXPORT vtkDistributionPValue : public vtkImageMultipleInputFilter
{
public:
  static vtkDistributionPValue* New();
  vtkTypeMacro(vtkDistributionPValue,vtkImageMultipleInputFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetMacro(NumberOfSamples1, int);
  vtkGetMacro(NumberOfSamples1, int);

  vtkSetMacro(NumberOfSamples2, int);
  vtkGetMacro(NumberOfSamples2, int);

  virtual void SetMean1(vtkImageData *input)
  {
    this->SetInput(0, input);
  }
  vtkImageData* GetMean1();
  
  virtual void SetSigma1(vtkImageData *input)
  {
    this->SetInput(1, input);
  }
  vtkImageData* GetSigma1();
  
  virtual void SetMean2(vtkImageData *input)
  {
    this->SetInput(2, input);
  }
  vtkImageData* GetMean2();
  
  virtual void SetSigma2(vtkImageData *input)
  {
    this->SetInput(3, input);
  }
  vtkImageData* GetSigma2();
  
protected:
  vtkDistributionPValue();
  ~vtkDistributionPValue();
  vtkDistributionPValue(const vtkDistributionPValue&);
  void operator=(const vtkDistributionPValue&);
  void ExecuteInformation(vtkImageData **inDatas, vtkImageData *outData);
  void ThreadedExecute(vtkImageData **inDatas, vtkImageData *outData,
		       int extent[6], int id);

  int NumberOfSamples1;
  int NumberOfSamples2;
};
#endif

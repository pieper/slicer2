#ifndef __vtkImageResliceST_h
#define __vtkImageResliceST_h

#include <vtkAGConfigure.h>


#include "vtkImageReslice.h"

class VTK_AG_EXPORT vtkImageResliceST : public vtkImageReslice
{
public:
  static vtkImageResliceST *New();
  vtkTypeMacro(vtkImageResliceST, vtkImageReslice);

  virtual void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkImageResliceST();
  ~vtkImageResliceST();
  vtkImageResliceST(const vtkImageResliceST&);
  void operator=(const vtkImageResliceST&);

  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
               int ext[6], int id);
};

#endif






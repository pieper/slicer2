#ifndef __vtkINRReader_h
#define __vtkINRReader_h

#include <vtkAGConfigure.h>

#include <stdio.h>
#include "vtkImageReader.h"

class VTK_AG_EXPORT vtkINRReader : public vtkImageReader
{
public:
  static vtkINRReader *New();
  vtkTypeMacro(vtkINRReader,vtkImageReader);
  
protected:
  vtkINRReader() {};
  ~vtkINRReader() {};
  vtkINRReader(const vtkINRReader&) {};
  void operator=(const vtkINRReader&) {};
  void ExecuteInformation();
};

#endif



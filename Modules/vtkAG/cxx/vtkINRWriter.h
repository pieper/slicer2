#ifndef __vtkINRWriter_h
#define __vtkINRWriter_h

#include <vtkAGConfigure.h>

#include "vtkImageWriter.h"
#include "vtkImageData.h"

class VTK_AG_EXPORT vtkINRWriter : public vtkImageWriter
{
public:
  static vtkINRWriter *New();
  vtkTypeMacro(vtkINRWriter,vtkImageWriter);

  vtkSetMacro(WritePixelSizes,bool);
  vtkGetMacro(WritePixelSizes,bool);
  vtkBooleanMacro(WritePixelSizes,bool);

protected:
  vtkINRWriter();
  ~vtkINRWriter() {};
  vtkINRWriter(const vtkINRWriter&) {};
  void operator=(const vtkINRWriter&) {};

  virtual void WriteFileHeader(ofstream *, vtkImageData *);

private:
  bool WritePixelSizes;
};

#endif



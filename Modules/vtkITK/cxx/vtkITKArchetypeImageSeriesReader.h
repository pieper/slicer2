/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkITKArchetypeImageSeriesReader.h,v $

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// .NAME vtkITKArchetypeImageSeriesReader - Read a series of files
// that have a common naming convention
// .SECTION Description
// ArchetypeImageSeriesReader creates a volume from a series of images
// stored in files. The series are represented one filename. This
// filename, the archetype, is any one of the files in the series.

#ifndef __vtkITKArchetypeImageSeriesReader_h
#define __vtkITKArchetypeImageSeriesReader_h

#include "vtkImageSource.h"
#include <vector>

class VTK_EXPORT vtkITKArchetypeImageSeriesReader : public vtkImageSource
{
public:
  static vtkITKArchetypeImageSeriesReader *New();
  vtkTypeRevisionMacro(vtkITKArchetypeImageSeriesReader,vtkImageSource);
  void PrintSelf(ostream& os, vtkIndent indent);   

  // Description:
  // Specify the archetype filename for the series.
  vtkSetStringMacro(Archetype);
  vtkGetStringMacro(Archetype);

  // Description:
  // Set/Get the default spacing of the data in the file. This will be
  // used if the reader provided spacing is 1.0. (Default is 1.0)
  vtkSetVector3Macro(DefaultDataSpacing,double);
  vtkGetVector3Macro(DefaultDataSpacing,double);
  
  // Description:
  // Set/Get the default origin of the data (location of first pixel
  // in the file). This will be used if the reader provided origin is
  // 0.0. (Default is 0.0)
  vtkSetVector3Macro(DefaultDataOrigin,double);
  vtkGetVector3Macro(DefaultDataOrigin,double);

  // Description:
  // When reading files which start at an unusual index, this can be added
  // to the slice number when generating the file name (default = 0)
  vtkSetMacro(FileNameSliceOffset,int);
  vtkGetMacro(FileNameSliceOffset,int);

  // Description:
  // When reading files which have regular, but non contiguous slices
  // (eg filename.1,filename.3,filename.5)
  // a spacing can be specified to skip missing files (default = 1)
  vtkSetMacro(FileNameSliceSpacing,int);
  vtkGetMacro(FileNameSliceSpacing,int);

  // Description:
  // The maximum number of files to include in the series. If this is
  // zero, then all files will be included. (Default is 0)
  vtkSetMacro(FileNameSliceCount,int);
  vtkGetMacro(FileNameSliceCount,int);

  // Description:
  // Set the data type of pixels in the file.  
  // If you want the output scalar type to have a different value, set it
  // after this method is called.
  virtual void SetOutputScalarTypeToFloat(){this->SetOutputScalarType(VTK_FLOAT);}
  virtual void SetOutputScalarTypeToDouble(){this->SetOutputScalarType(VTK_DOUBLE);}
  virtual void SetOutputScalarTypeToUnsignedLong(){this->SetOutputScalarType(VTK_UNSIGNED_LONG);}
  virtual void SetOutputScalarTypeToLong(){this->SetOutputScalarType(VTK_LONG);}
  virtual void SetOutputScalarTypeToUnsignedInt(){this->SetOutputScalarType(VTK_UNSIGNED_INT);}
  virtual void SetOutputScalarTypeToInt(){this->SetOutputScalarType(VTK_INT);}
  virtual void SetOutputScalarTypeToUnsignedShort(){this->SetOutputScalarType(VTK_UNSIGNED_SHORT);}
  virtual void SetOutputScalarTypeToShort(){this->SetOutputScalarType(VTK_SHORT);}
  virtual void SetOutputScalarTypeToUnsignedChar(){this->SetOutputScalarType(VTK_UNSIGNED_CHAR);}
  virtual void SetOutputScalarTypeToChar(){this->SetOutputScalarType(VTK_CHAR);}

  // Description:
  // Get the file format.  Pixels are this type in the file.
  vtkSetMacro(OutputScalarType, int);
  vtkGetMacro(OutputScalarType, int);

protected:
  vtkITKArchetypeImageSeriesReader();
  ~vtkITKArchetypeImageSeriesReader();

  char *Archetype;
  int DataExtent[6];

  int OutputScalarType;

  double DefaultDataSpacing[3];
  double DefaultDataOrigin[3];

  int FileNameSliceOffset;
  int FileNameSliceSpacing;
  int FileNameSliceCount;
  
//BTX
  std::vector<std::string> FileNames;
//ETX
  virtual void ExecuteInformation();
  virtual void ExecuteData(vtkDataObject *data);

private:
  vtkITKArchetypeImageSeriesReader(const vtkITKArchetypeImageSeriesReader&);  // Not implemented.
  void operator=(const vtkITKArchetypeImageSeriesReader&);  // Not implemented.
};

#endif

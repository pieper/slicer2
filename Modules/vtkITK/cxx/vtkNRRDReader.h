/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkNRRDReader.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkNRRDReader - Reads Nearly Raw Raster Data files
// .SECTION Description
// vtkNRRDReader 
// Reads Nearly Raw Raster Data files using the nrrdio library as used in ITK
//

//
// .SECTION See Also
// vtkImageReader2

#ifndef __vtkNRRDReader_h
#define __vtkNRRDReader_h

#include "vtkITKConfigure.h"
#include "vtkMedicalImageReader2.h"

#include "NrrdIO/NrrdIO.h"

class VTK_ITK_EXPORT vtkNRRDReader : public vtkMedicalImageReader2
{
public:
  static vtkNRRDReader *New();
  vtkTypeRevisionMacro(vtkNRRDReader,vtkMedicalImageReader2);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description: is the given file name a NRRD file?
  virtual int CanReadFile(const char* filename);

  // Description:
  // Valid extentsions
  virtual const char* GetFileExtensions()
    {
      return ".nhdr";
    }

  // Description: 
  // A descriptive name for this format
  virtual const char* GetDescriptiveName()
    {
      return "NRRD - Nearly Raw Raster Data";
    }

  int NrrdToVTKScalarType( const int nrrdPixelType ) const
  {
  switch( nrrdPixelType )
    {
    default:
    case nrrdTypeDefault:
      return VTK_VOID;
      break;
    case nrrdTypeChar:
      return VTK_CHAR;
      break;
    case nrrdTypeUChar:
      return VTK_UNSIGNED_CHAR;
      break;
    case nrrdTypeShort:
      return VTK_SHORT;
      break;
    case nrrdTypeUShort:
      return VTK_UNSIGNED_SHORT;
      break;
      //    case nrrdTypeLLong:
      //      return LONG ;
      //      break;
      //    case nrrdTypeULong:
      //      return ULONG;
      //      break;
    case nrrdTypeInt:
      return VTK_INT;
      break;
    case nrrdTypeUInt:
      return VTK_UNSIGNED_INT;
      break;
    case nrrdTypeFloat:
      return VTK_FLOAT;
      break;
    case nrrdTypeDouble:
      return VTK_DOUBLE;
      break;
    case nrrdTypeBlock:
      return -1;
      break;
    }
  }

  int VTKToNrrdPixelType( const int vtkPixelType ) const
  {
  switch( vtkPixelType )
    {
    default:
    case VTK_VOID:
      return nrrdTypeDefault;
      break;
    case VTK_CHAR:
      return nrrdTypeChar;
      break;
    case VTK_UNSIGNED_CHAR:
      return nrrdTypeUChar;
      break;
    case VTK_SHORT:
      return nrrdTypeShort;
      break;
    case VTK_UNSIGNED_SHORT:
      return nrrdTypeUShort;
      break;
      //    case nrrdTypeLLong:
      //      return LONG ;
      //      break;
      //    case nrrdTypeULong:
      //      return ULONG;
      //      break;
    case VTK_INT:
      return nrrdTypeInt;
      break;
    case VTK_UNSIGNED_INT:
      return nrrdTypeUInt;
      break;
    case VTK_FLOAT:
      return nrrdTypeFloat;
      break;
    case VTK_DOUBLE:
      return nrrdTypeDouble;
      break;
    }
  }

protected:
  vtkNRRDReader() {};
  ~vtkNRRDReader() {};

  virtual void ExecuteInformation();
  virtual void ExecuteData(vtkDataObject *out);

private:
  vtkNRRDReader(const vtkNRRDReader&);  // Not implemented.
  void operator=(const vtkNRRDReader&);  // Not implemented.
};
#endif



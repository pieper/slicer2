/*=auto=========================================================================

(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================auto=*/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkFSSurfaceAnnotationReader.h,v $
  Language:  C++
  Date:      $Date: 2005/04/04 15:35:08 $
  Version:   $Revision: 1.3 $

=========================================================================*/
// .NAME vtkFSSurfaceAnnotationReader - read a surface annotation and
// color table file from
// Freesurfer tools
// .SECTION Description

// Reads a surface annotation file from FreeSurfer and outputs a
// vtkIntArray and has an access function to get the vtkLookupTable
// with the color values. Use the SetFileName function to specify the
// file name. The number of values in the array should be equal to the
// number of vertices/points in the surface.

#ifndef __vtkFSSurfaceAnnotationReader_h
#define __vtkFSSurfaceAnnotationReader_h

#include <vtkFreeSurferReadersConfigure.h>
#include "vtkDataReader.h"
#include "vtkLookupTable.h"
#include "vtkPolyData.h"

// tag
const int FS_COLOR_TABLE_TAG = 1;

const int FS_COLOR_TABLE_NAME_LENGTH = 1024;
const int FS_COLOR_TABLE_ENTRY_NAME_LENGTH = 1024;

const int FS_ERROR_LOADING_COLOR_TABLE = 1;
const int FS_ERROR_LOADING_ANNOTATION = 2;
const int FS_ERROR_PARSING_COLOR_TABLE = 3;
const int FS_ERROR_PARSING_ANNOTATION = 4;
const int FS_WARNING_UNASSIGNED_LABELS = 5;
const int FS_NO_COLOR_TABLE = 6;

class VTK_FREESURFERREADERS_EXPORT vtkFSSurfaceAnnotationReader : public vtkDataReader
{
public:
  static vtkFSSurfaceAnnotationReader *New();
  vtkTypeMacro(vtkFSSurfaceAnnotationReader,vtkDataReader);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkIntArray *GetOutput()
    {return this->Labels; };
  void SetOutput(vtkIntArray *output)
    {this->Labels = output; };

  vtkLookupTable *GetColorTableOutput()
    {return this->Colors; };
  void SetColorTableOutput(vtkLookupTable* colors)
    {this->Colors = colors; };

  char* GetColorTableNames();

  int ReadFSAnnotation();

  void SetColorTableFileName (char*);

    // changed these to use vtk Get/Set macros
    //void DoUseExternalColorTableFile ();
    //void DontUseExternalColorTableFile ();
    vtkGetMacro(UseExternalColorTableFile,int);
    vtkSetMacro(UseExternalColorTableFile,int);
    vtkBooleanMacro(UseExternalColorTableFile,int);
    
protected:
  vtkFSSurfaceAnnotationReader();
  ~vtkFSSurfaceAnnotationReader();

  vtkIntArray    *Labels;
  vtkLookupTable *Colors;
  char           *NamesList;
  int            NumColorTableEntries;

    // bool UseExternalColorTableFile;
    int UseExternalColorTableFile;
  char ColorTableFileName[1024];

  // Read color table information from a source, allocate the arrays
  // to hold rgb and name values, and return pointers to the
  // arrays. The caller is responsible for disposing of the memory.
  int ReadEmbeddedColorTable (FILE* annotFile, int* numEntries, 
                  int*** rgbValues, char*** names);
  int ReadExternalColorTable (char* fileName, int* numEntries, 
                  int*** rgbValues, char*** names);
  

private:
  vtkFSSurfaceAnnotationReader(const vtkFSSurfaceAnnotationReader&);  // Not implemented.
  void operator=(const vtkFSSurfaceAnnotationReader&);  // Not implemented.
};


#endif

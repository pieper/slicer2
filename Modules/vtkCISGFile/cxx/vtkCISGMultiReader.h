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
 
  Library   : vtk/objects/library/CISGImage
  Module    : $RCSfile: vtkCISGMultiReader.h,v $
  Authors   : Thomas Hartkens <thomas@hartkens.de>
              Daniel Rueckert <d.rueckert@doc.ic.ac.uk>
              Julia Schnabel  <julia.schnabel@kcl.ac.uk>
              under the supervision of Dave Hawkes and Derek Hill
  Web       : http://www.image-registration.com
  Copyright : King's College London
              Div. of Radiological Sciences, 
              Computational Imaging Science Group, 1997 - 2000
              http://www-ipg.umds.ac.uk/cisg
  Date      : $Date: 2005/04/19 14:48:26 $
  Version   : $Revision: 1.3 $

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
=========================================================================*/
// .NAME vtkCISGMultiReader - read various image file formats
// .SECTION Description
// vtkCISGMultiReader is a class for reading a number of different image
// file formats. A new file format can easily be added by deriving it
// from this class, implementing a New() and CheckHeader() method for the new
// reader, including its class header file in vtkMultiReader.h, and finally
// adjusting New() to take the new image reader into account.
//
// .SECTION See Also
// vtkStructuredPointsSource vtkCISGMultiWriter vtkCISGAnalyzeReader vtkCISGGiplReader vtkCISGInterfileReader vtkCISGRREPReader

#ifndef __vtkCISGMultiReader_h
#define __vtkCISGMultiReader_h

#include <vtkCISGFileConfigure.h>

#include "vtkPointData.h"
#include "vtkStructuredPoints.h"
#include "vtkStructuredPointsSource.h"
#include "vtkInternalFlip.h"

class VTK_CISGFILE_EXPORT vtkCISGMultiReader : public vtkStructuredPointsSource {

 public:
  vtkTypeMacro(vtkCISGMultiReader,vtkStructuredPointsSource);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Default object allocation.
  static vtkCISGMultiReader *New();

  // Description:
  // Returns correct instantiation of vtkCISGMultiReader, depending on
  // return of CheckFormat();
  static vtkCISGMultiReader *New(char *filename);
  
  /// Returns whether file has correct header.
  static int CheckHeader(char *filename){ return -1; }
  
  // Description:
  // Specify file name of data file to read.
  vtkSetStringMacro(FileName);
  
  // Description:
  // Get file name of data file to read.
  vtkGetStringMacro(FileName);
      
  // Description:
  // Get the output of this reader.
  vtkStructuredPoints *GetOutput(); 

  // Description:
  // Update this reader by calling Execute(), updating the InternalReader,
  // or updating its parent vtkStructuredPointsSource.
  void Update();
  
  // Description:
  // Returns whether file format is supported.
  int IsSupportedFileFormat();

  // Description:
  // Within tcl (e.g. for vtkview), this will scan for known supported
  // image file format extensions.
  vtkGetStringMacro(TCLFileExtentions);

  // Description:
  // Gets the pointer to the actual reader. 
  vtkGetObjectMacro(Reader, vtkCISGMultiReader);

  // Description:
  // Sets the pointer to the actual reader. 
  vtkSetObjectMacro(Reader, vtkCISGMultiReader);

 protected:

  // Description:
  // Default constructor.
  vtkCISGMultiReader();

  // Description:
  // Destructor.
  ~vtkCISGMultiReader();
  
  // Description:
  // copy constructor (empty).
  vtkCISGMultiReader(const vtkCISGMultiReader&) {};

  // Description:
  // Assigment operator (empty).
  void operator=(const vtkCISGMultiReader&) {};

  // Description:
  // Member to hold filename.
  char *FileName;

  // Description:
  // pointer to the actual reader.
  vtkCISGMultiReader* Reader;

  // Description:
  // pointer to the internal reader.
  vtkCISGMultiReader* InternalReader;
  
  // Description:
  // Calls New() to get the InternalReader and set the actual Reader.
  void Execute();

  // Description:
  // Member to hold the image origin. By default, this is (0,0,0) in mm,
  // centred in the image.
  vtkFloatingPointType Origin[3];

  // Description:
  // Member to hold the image voxel size. By default, this is 1mm x 1mm x
  // 1mm.
  vtkFloatingPointType Size[3];

  // Description:
  // Member to hold the image dimensions. By default, this is 0 x 0 x 0
  // voxels.
  int   Dim[3];

  // Description:
  // Member to hold the data type. By default, this is 0.
  char Type;

  // Description:
  // Member to hold the data byte size. By default, this is 0.
  char Bytes;

  // Description:
  // Member to hold the supported file extension for tcl interfaces.
  char *TCLFileExtentions;

  // Description:
  // Member to hold the supported file format names (not used).
  static void **FileFormats;

  short NumberOfComponents;
};



#include "vtkCISGGiplReader.h"
#include "vtkCISGAnalyzeReader.h"
#include "vtkCISGInterfileReader.h"
#include "vtkCISGRREPReader.h"



#endif








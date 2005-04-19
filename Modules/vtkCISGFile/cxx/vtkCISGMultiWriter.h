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
  Module    : $RCSfile: vtkCISGMultiWriter.h,v $
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
// .NAME vtkCISGMultiWriter - write various image file formats
// .SECTION Description
// vtkCISGMultiWriter is a class for writing a number of different image
// file formats. A new file format can easily be added by deriving it
// from this class, implementing a New() and Check() method for the new
// writer, including its class header file in vtkCISGMultiWriter.h, and
// finally adjusting New() to take the new image writer into account.
//
// .SECTION Caveats
// To find out which image format is to be written, file extensions of the
// output file name are checked. If no match with the supported image file
// format extensions can be found, GIPL image format will be written.
// 
// .SECTION See Also
// vtkDataWriter vtkCISGMultiReader vtkCISGAnalyzeWriter vtkCISGGiplWriter vtkCISGInterfileWriter

#ifndef __vtkCISGMultiWriter_h
#define __vtkCISGMultiWriter_h

#include <vtkCISGFileConfigure.h>

#include "vtkDataWriter.h"
#include "vtkStructuredPoints.h"
#include "vtkImageToStructuredPoints.h"
#include "vtkInternalFlip.h"
#include "vtkCISGMultiReader.h"

#define Writer_GIPL_Format 0
#define Writer_Interfile_Format 1

class VTK_CISGFILE_EXPORT vtkCISGMultiWriter : public vtkDataWriter {
 public:
  vtkTypeMacro(vtkCISGMultiWriter,vtkDataWriter);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Default object allocation.
  static vtkCISGMultiWriter *New();

  // Description:
  // Returns correct instantiation of vtkCISGMultiWriter, depending on
  // return of Check().
  static vtkCISGMultiWriter *New(char *filename);

  // Description:
  // Returns correct instantiation of vtkCISGMultiWriter, depending on
  // passed format which now can only be Writer_GIPL_Format.
  static vtkCISGMultiWriter *New(int Format);
  
  // Description:
  // Returns whether the filename is supported
  static int Check(char *filename){ return -1; }
  
  // Description:
  // Specify file name of data file to write.
  vtkSetStringMacro(FileName);
  
  // Description:
  // Get file name of data file to write.
  vtkGetStringMacro(FileName);
    
  // Description:
  // Update writer which calls WriteData().
  void Update();
  
  // Description:
  // Returns whether file format is supported for writing.
  int IsSupportedFileFormat();

  vtkGetStringMacro(TCLFileExtentions);

  // Description:
  // Set the input data.
  void SetInput(vtkStructuredPoints *input);

  // Description:
  // Set input filter ( casts vtkImageData to vtkStructuredPoints and calls
  // overloaded SetInput() ). 
  void SetInput(vtkImageData *cache){
    //vtkImageToStructuredPoints *tmp = cache->MakeImageToStructuredPoints();
    //this->SetInput(tmp->GetOutput()); 
    //tmp->Delete();
    //cache->Print(cout);
    this->SetInput((vtkStructuredPoints *) cache);
  }

  // Description:
  // Get the input data.
  vtkStructuredPoints *GetInput();

  // Description:
  // It might be necessary to get additional information from the 
  // reader about the file, e.g. some special patient data, which 
  // are not hold in the VTK data structure.
  vtkSetObjectMacro(Reader, vtkCISGMultiReader);

 protected:

  // Description:
  // Default constructor.
  vtkCISGMultiWriter();

  // Description:
  // Destructor.
  ~vtkCISGMultiWriter();

  // Description:
  // Copy constructor (empty).
  vtkCISGMultiWriter(const vtkCISGMultiWriter&) {};

  // Description:
  // Assignment operator (empty).
  void operator=(const vtkCISGMultiWriter&) {};
  
  // Description:
  // Method to write data. Called by Update().
  void WriteData();

  // Description:
  // Member to fold filename.
  char *FileName;
  
  // Description:
  // Member to write data.
  vtkCISGMultiWriter* InternalWriter;

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
  // Member to hold the supported file extension for tcl interfaces.  
  char *TCLFileExtentions;

  // Description:
  // Member for reading
  vtkCISGMultiReader *Reader;

};


#include "vtkCISGGiplWriter.h"
#include "vtkCISGAnalyzeWriter.h"
#include "vtkCISGInterfileWriter.h"

#endif

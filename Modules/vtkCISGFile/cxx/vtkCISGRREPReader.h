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
  Module    : $RCSfile: vtkCISGRREPReader.h,v $
  Authors   : Thomas Hartkens <thomas@hartkens.de>
              Daniel Rueckert <d.rueckert@doc.ic.ac.uk>
              Julia Schnabel  <julia.schnabel@kcl.ac.uk>
              under the supervision of Dave Hawkes and Derek Hill
  Web       : http://www.image-registration.com
  Copyright : King's College London
              Div. of Radiological Sciences, 
              Computational Imaging Science Group, 1997 - 2001
              http://www-ipg.umds.ac.uk/cisg
  Date      : $Date: 2005/04/19 14:48:26 $
  Version   : $Revision: 1.2 $

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
// .NAME vtkCISGRREPReader - read RREP image file format
// .SECTION Description
// vtkRREPReader is an instantiated class of vtkCISGMultiReader. RREP stands
// for "Retrospective Registration Evaluation Project" which is a project
// from Vanderbilt university, led by J. Michael Fitzpatrick, to compare
// CT-MR and PET-MR registration techniques used by a number of groups.
// The rigid registration in the vtkCISG package is a reimplementation
// of the technique by Colin Studholme based on normalized mutual information
// (NMI) which was evaluated in the original RREP study. An RREP image will 
// consist of two files, a header file and a data file. For more information,
// see the RREP homepage: http://www.vuse.vanderbilt.edu/~image/registration
// and http://www.vuse.vanderbilt.edu/~image/registration/online_files/format.txt
// for the image file format description.
//
// .SECTION Caveats
// The two file names of an RREP image will be always be header.ascii and
// image.bin. An additional file named image.bin.Z which is included for 
// convenience and which contains a compressed version of image.bin will be
// read instead of image.bin if present. No writer for RREP is provided. 
// 
// .SECTION See Also
// vtkCISGMultiReader

#ifndef __vtkCISGRREPReader_h
#define __vtkCISGRREPReader_h

#include <vtkCISGFileConfigure.h>

#include "vtkCISGMultiReader.h"
#include "vtkCISGCifstream.h"


class VTK_CISGFILE_EXPORT vtkCISGRREPReader : public vtkCISGMultiReader
{
public:
  vtkTypeMacro(vtkCISGRREPReader,vtkCISGMultiReader);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Construct object with NULL file prefix; file pattern "%s.%d"; image range 
  // set to (1,1); data origin (0,0,0); data spacing (1,1,1); no data mask;
  // header size 0; and byte swapping turned off.
  static vtkCISGRREPReader *New();

  // Description:
  // Checks whether data is in RREP format. The check is currently done by
  // checking whether image data header is called header.ascii .
  static int CheckHeader(char *filename);

protected:

  // Description:
  // Default constructor.
  vtkCISGRREPReader();

  // Description:
  // Destructor.
  ~vtkCISGRREPReader();

  // Description:
  // Copy constructor (empty).
  vtkCISGRREPReader(const vtkCISGRREPReader&) {};

  // Description:
  // Assignment operator (empty).
  void operator=(const vtkCISGRREPReader&) {};

  // Description:
  // This method reads the header and volume data using ReadHeader() and 
  // ReadVolume().
  void Execute();

  // Description:
  // Input file stream for reading compressed image volume data.
  vtkCISGCifstream *from;

  // Description:
  // Read header of data. The RREP header must be called header.ascii .
  // This method sets the image dimensions, voxel size, data type etc.
  void ReadHeader();

  // Description:
  // Read a volume of data. The RREP volume must be called image.bin(.Z).
  // Data is read according to header information obtained by ReadHeader().
  vtkDataArray *ReadVolume();
  
};

#endif



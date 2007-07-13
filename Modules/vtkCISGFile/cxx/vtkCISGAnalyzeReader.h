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
  Module    : $RCSfile: vtkCISGAnalyzeReader.h,v $
  Authors   : Thomas Hartkens <thomas@hartkens.de>
              Daniel Rueckert <d.rueckert@doc.ic.ac.uk>
              Julia Schnabel  <julia.schnabel@kcl.ac.uk>
              under the supervision of Dave Hawkes and Derek Hill
  Web       : http://www.image-registration.com
  Copyright : King's College London
              Div. of Radiological Sciences, 
              Computational Imaging Science Group, 1997 - 2001
              http://www-ipg.umds.ac.uk/cisg
  Date      : $Date: 2005/04/19 14:48:25 $
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
// .NAME vtkCISGAnalyzeReader - read Analyze image file format
// .SECTION Description
// vtkCISGAnalyzeReader is an instantiated class of vtkCISGMultiReader.
// It allows reading of Analyze format, the image file format of the
// Analyze package from the Biomedical Imaging Resource, Mayo Clinic 
// (http://www.mayo.edu/bir). The format supported here is called
// Analyze Image (7.5.) and is described in more detail on
// http://www.mayo.edu/bir/PDF/ANALYZE75.pdf . It consists of a header file
// and a data file. 
// 
// .SECTION Caveats
// The two file names of an Analyze image will have the extensions .hdr
// for the image header, and .bin(.Z) for the (compressed) image volume data.
// image format (extension .hdr) and a binary image file (extension .bin). 
// They files have the same name being distinguished by their extensions.
// 
// .SECTION see also
// vtkCISGMultiReader vtkCISGAnalyzeWriter

#ifndef __vtkCISGAnalyzeReader_h
#define __vtkCISGAnalyzeReader_h

#include <vtkCISGFileConfigure.h>

#include "vtkCISGMultiReader.h"
#include "vtkCISGCifstream.h"
#include "vtkFloatArray.h"


class VTK_CISGFILE_EXPORT vtkCISGAnalyzeReader : public vtkCISGMultiReader
{
public:
  vtkTypeMacro(vtkCISGAnalyzeReader,vtkCISGMultiReader);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Construct object with NULL file prefix; file pattern "%s.%d"; image range 
  // set to (1,1); data origin (0,0,0); data spacing (1,1,1); no data mask;
  // header size 0; and byte swapping turned off.
  static vtkCISGAnalyzeReader *New();

  // Checks whether data is in Analyze format. 
  static int CheckHeader(char *filename);

  // specify orientation of the images
  vtkGetStringMacro(FlippingSequence);
  vtkSetStringMacro(FlippingSequence);

protected:

  // Description:
  // Default constructor.
  vtkCISGAnalyzeReader();

  // Description:
  // Destructor.
  ~vtkCISGAnalyzeReader();

  // Description:
  // Copy constructor (empty).
  vtkCISGAnalyzeReader(const vtkCISGAnalyzeReader&) {};

  // Description:
  // Assignment operator (empty).
  void operator=(const vtkCISGAnalyzeReader&) {};

  // Description:
  // This method reads the header and volume data using ReadHeader() and 
  // ReadVolume().
  void Execute();

  // Description:
  // Input file stream for reading (compressed) image volume data.
  vtkCISGCifstream *from;

  // Description:
  // Read header of data. The Analyze header must have a .hdr extension.
  // This method sets the image dimensions, voxel size, data type etc.
  void ReadHeader();

  // Description:
  // Read a volume of data. The Analyze volume must have a .bin(.Z) extension.
  // Data is read according to header information obtained by ReadHeader().
  vtkDataArray *ReadVolume();

  int NeedSwapping;
  char *FlippingSequence; // string of characters 0, 1, 2 telling sequence of flips to perform
};

#endif



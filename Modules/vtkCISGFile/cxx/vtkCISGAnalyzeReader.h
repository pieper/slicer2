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
  Date      : $Date: 2003/08/14 17:32:33 $
  Version   : $Revision: 1.1 $

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




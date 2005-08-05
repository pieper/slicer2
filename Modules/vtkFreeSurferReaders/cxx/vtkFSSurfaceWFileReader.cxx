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
  Module:    $RCSfile: vtkFSSurfaceWFileReader.cxx,v $
  Language:  C++
  Date:      $Date: 2005/08/05 19:51:57 $
  Version:   $Revision: 1.2 $

=========================================================================*/
#include "vtkFSSurfaceWFileReader.h"
#include "vtkObjectFactory.h"
#include "vtkFSIO.h"
#include "vtkByteSwap.h"
#include "vtkFloatArray.h"

//-------------------------------------------------------------------------
vtkFSSurfaceWFileReader* vtkFSSurfaceWFileReader::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkFSSurfaceWFileReader");
  if(ret)
    {
    return (vtkFSSurfaceWFileReader*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkFSSurfaceWFileReader;
}

vtkFSSurfaceWFileReader::vtkFSSurfaceWFileReader()
{
    this->scalars = NULL;
}

vtkFSSurfaceWFileReader::~vtkFSSurfaceWFileReader()
{
}


void vtkFSSurfaceWFileReader::ReadWFile()
{
  FILE* wFile;
  int magicNumber;
  int numValues = 0;
  int vIndex;
  int vIndexFromFile;
  float fvalue;
  float *scalars;
  vtkFloatArray *output = this->scalars;

  // Do some basic sanity checks.
  if (output == NULL)
    {
      cerr << "ERROR vtkFSSurfaceWFileReader ReadWFile() : output is null" << endl;
      return;
  }
  vtkDebugMacro( << "vtkFSSurfaceWFileReader Execute() " << endl);

  if (!this->FileName) {
    vtkErrorMacro(<<"vtkFSSurfaceWFileReader Execute: FileName not specified.");
    return;
  }

  vtkDebugMacro(<<"Reading surface WFile data...");

  // Try to open the file.
  wFile = fopen(this->FileName, "rb") ;
  if (!wFile) {
    vtkErrorMacro (<< "Could not open file " << this->FileName);
    return;
  }  

  // I'm not sure what this is. In the original FreeSurfer code, there
  // is this:
  //
  //    fread2(&ilat,fp);
  //    lat = ilat/10.0;
  //
  // And then the lat variable is not used again. Maybe it was a scale
  // factor of some kind? No idea.
  vtkFSIO::ReadInt2 (wFile, magicNumber);

  // This is the number of values in the wfile.
  vtkFSIO::ReadInt3 (wFile, numValues);
  if (numValues < 0) {
    vtkErrorMacro (<< "vtkFSSurfaceWFileReader.cxx Execute: Number of vertices is 0 or negative, can't process file.");
      return;
  }

  vtkDebugMacro(<<"vtkFSSurfaceWFileReader: numValues = " << numValues);
  
  // Make our float array.
  scalars = (float*) calloc (numValues, sizeof(float));
  if (scalars == NULL)
  {
      vtkErrorMacro(<<"vtkFSSurfaceWFileReader: error allocating " << numValues << " floats!");
      return;
  }
  
  // For each value in the wfile...
  for (vIndex = 0; vIndex < numValues; vIndex ++ ) {
    
    // Check for eof.
    if (feof(wFile)) {
        vtkErrorMacro (<< "vtkFSSurfaceWFileReader.cxx Execute: Unexpected EOF after " << vIndex << " values read. Tried to read " << numValues);
      return;
    }

    // Read the 3 byte int index and float value. The wfile is weird
    // in that there is an index/value pair for every value. I guess
    // this means that the wfile could have fewer values than the
    // number of vertices in the surface, but I've never seen this
    // happen in practice. Additionally, these are usually written
    // with indices from 0->nvertices, so this index value isn't even
    // really needed.
    vtkFSIO::ReadInt3 (wFile, vIndexFromFile);
    vtkFSIO::ReadFloat (wFile, fvalue);

    // Make sure the index is in bounds. If not, print a warning and
    // try to do the next value. If this happens, there is probably a
    // mismatch between the wfile and the surface, but I think there
    // is a reason for being able to load a mismatched file. But this
    // should raise some kind of message to the user like, "This wfile
    // appears to be for a different surface; continue loading?"
    if (vIndexFromFile < 0 || vIndexFromFile >= numValues) {
        vtkErrorMacro (<< "vtkFSSurfaceWFileReader.cxx Execute: Read an index that is out of bounds (" << vIndexFromFile << " not in 0-" << numValues << ", skipping.");
      continue;
    }
    
    // Set the value in the scalars array based on the index we read
    // in, not the index in our for loop.
    scalars[vIndexFromFile] = fvalue;
  }

  // Close the file.
  fclose (wFile);

  // Set the array in our output.
  output->SetArray (scalars, numValues, 0);
}

void vtkFSSurfaceWFileReader::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkDataReader::PrintSelf(os,indent);
}


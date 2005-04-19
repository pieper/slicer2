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
  Module    : $RCSfile: vtkCISGMultiWriter.cxx,v $
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
#include "vtkObjectFactory.h"
#include "vtkShortArray.h"
#include "vtkCISGMultiWriter.h"

#include <stdio.h>




// This function must be adjusted by other users
vtkCISGMultiWriter* vtkCISGMultiWriter::New(char *filename){
  vtkCISGMultiWriter* writer;

  if (vtkCISGGiplWriter::Check(filename)){
    writer = vtkCISGGiplWriter::New();
  } else if (vtkCISGAnalyzeWriter::Check(filename)){
    writer = vtkCISGAnalyzeWriter::New();
  } else if (vtkCISGInterfileWriter::Check(filename)){
    writer = vtkCISGInterfileWriter::New();
  } else {
    cerr << "vtkCISGMultiWriter: Unknown format. Using GIPL writer ...\n";
    writer = vtkCISGGiplWriter::New();
  }

  writer->SetFileName(filename);
  return writer;
}





// ------------------------------------------------------------
// Do not change anything beyond this line 
// ------------------------------------------------------------
vtkCISGMultiWriter* vtkCISGMultiWriter::New(int format){
  vtkCISGMultiWriter* writer;

  switch (format) {
  case Writer_GIPL_Format:
    writer = vtkCISGGiplWriter::New();
  case Writer_Interfile_Format:
    // writer = vtkCISGInterfileWriter::New();
  default:
    cerr << "vtkCISGMultiWriter: Unknown format. Using GIPL writer ...\n";
    writer = vtkCISGGiplWriter::New();
  }
  return writer;
}





vtkCISGMultiWriter* vtkCISGMultiWriter::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkCISGMultiWriter")
    ;
  if(ret)
    {
      return (vtkCISGMultiWriter*)ret;
    }
  // If the factory was unable to create the object, then create it here.
return new vtkCISGMultiWriter;
}

vtkCISGMultiWriter::vtkCISGMultiWriter() {
  TCLFileExtentions = " { \"GIPL file\" {*.gipl } } { \"Mayo Analyze\" {*.hdr } } { \"Interfile 3.3\" {*.A?? } } { \"All Files\" {*} } ";
  FileName=NULL;
  this->SetFileName("default.gipl");
  InternalWriter=NULL;
  Reader=NULL;
}

vtkCISGMultiWriter::~vtkCISGMultiWriter() {
  if (InternalWriter) {
    InternalWriter->Delete();
  }
}






/* If this function is called, then the writer was not created with the 
   function New(filename), but with New().
   That means, that no appropiate writer was created. So we have to do
   this now by creating an internal writer.
   */
void vtkCISGMultiWriter::WriteData(){
  if (this->GetInput()) {
    if (InternalWriter) {
      InternalWriter->Delete();
    }
    InternalWriter = vtkCISGMultiWriter::New(FileName);
    InternalWriter->SetInput(this->GetInput());
    InternalWriter->Update();
  } else {
    vtkErrorMacro(<< "Execute: No Input specified");
  }
}

void vtkCISGMultiWriter::Update(){
  this->WriteData();
}



int vtkCISGMultiWriter::IsSupportedFileFormat(){
  if  (InternalWriter) {
    return 1;
  }  
  InternalWriter = vtkCISGMultiWriter::New(FileName);
  return (InternalWriter != NULL);
}

void vtkCISGMultiWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkDataWriter::PrintSelf(os,indent);
  
}

//----------------------------------------------------------------------------
// Specify the input data or filter.
void vtkCISGMultiWriter::SetInput(vtkStructuredPoints *input)
{
  this->vtkProcessObject::SetNthInput(0, input);
}

//----------------------------------------------------------------------------
// Specify the input data or filter.
vtkStructuredPoints *vtkCISGMultiWriter::GetInput()
{
  if (this->NumberOfInputs < 1)
    {
    return NULL;
    }
  
  return (vtkStructuredPoints *)(this->Inputs[0]);
}







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
  Module    : $RCSfile: vtkCISGMultiReader.cxx,v $
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
#include "vtkCISGMultiReader.h"


#include <stdio.h>


// This function must be adjusted by other users
vtkCISGMultiReader* vtkCISGMultiReader::New(char *filename){
  vtkCISGMultiReader* reader;

  if (vtkCISGGiplReader::CheckHeader(filename)){
    reader = vtkCISGGiplReader::New();
  } else if (vtkCISGInterfileReader::CheckHeader(filename)){
    reader = vtkCISGInterfileReader::New();
  } else if (vtkCISGAnalyzeReader::CheckHeader(filename)){
    reader = vtkCISGAnalyzeReader::New();
  } else if (vtkCISGRREPReader::CheckHeader(filename)){
    reader = vtkCISGRREPReader::New();
  } else {
    return NULL;
  }

  reader->SetFileName(filename);
  return reader;
}





// ------------------------------------------------------------
// Do not change anything beyond this line 
// ------------------------------------------------------------



vtkCISGMultiReader* vtkCISGMultiReader::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkCISGMultiReader")
;
  if(ret)
    {
    return (vtkCISGMultiReader*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkCISGMultiReader;
}

vtkCISGMultiReader::vtkCISGMultiReader() {
  TCLFileExtentions = " { \"GIPL file\" {*.gipl *.gipl.Z} } { \"Mayo Analyze\" {*.hdr *.hdr.Z} } { \"Interfile 3.3\" {*.A??} } { \"RREP\" {header.ascii} } { \"All Files\" {*} } ";
  FileName=NULL;
  InternalReader=NULL; 
  Reader=this;
  Origin[0]=0.0;
  Origin[1]=0.0;
  Origin[2]=0.0;
  Size[0]=1.0;
  Size[1]=1.0;
  Size[2]=1.0;
  Dim[0]=0;
  Dim[1]=0;
  Dim[2]=0;
  Type=0;
  Bytes=0;
}

vtkCISGMultiReader::~vtkCISGMultiReader() {
  if (InternalReader) {
    InternalReader->Delete();
  }
}






/* If this function is called, then the reader was not created with the 
   function New(filename), but with New().
   That means, that no appropiate reader was created. So we have to do
   this now by creating an internal reader.
   */
void vtkCISGMultiReader::Execute(){
  if (InternalReader) {
    InternalReader->Delete();
  }
  InternalReader = vtkCISGMultiReader::New(FileName);
  Reader=InternalReader;
  InternalReader->Update();
}

void vtkCISGMultiReader::Update(){
  if  (InternalReader) {
    this->Execute();
    InternalReader->Update();
  } else {
    this->vtkStructuredPointsSource::Update();
  }
}


vtkStructuredPoints *vtkCISGMultiReader::GetOutput() {
  if  (InternalReader) {
    InternalReader->Update();
    return InternalReader->GetOutput();
  } else {
    return this->vtkStructuredPointsSource::GetOutput();
  }
}

int vtkCISGMultiReader::IsSupportedFileFormat(){
  if  (InternalReader) {
    return 1;
  }  
  InternalReader = vtkCISGMultiReader::New(FileName);
  Reader=InternalReader;
  return (InternalReader != NULL);
}

void vtkCISGMultiReader::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkStructuredPointsSource::PrintSelf(os,indent);

  os << indent << " Dimensions: (" 
     << this->Dim[0] << ", "
     << this->Dim[1] << ", "
     << this->Dim[2] << ")\n";
  os << indent << " Spacing: (" 
     << this->Size[0] << ", "
     << this->Size[1] << ", "
     << this->Size[2] << ")\n";
  os << indent << " Origin: ("  
     << this->Origin[0] << ", "
     << this->Origin[1] << ", "
     << this->Origin[2] << ")\n";

}




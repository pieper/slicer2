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
  Date      : $Date: 2003/08/14 17:32:36 $
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




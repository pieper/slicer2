/*=========================================================================
 
  Library   : vtk/objects/library/CISGImage
  Module    : $RCSfile: vtkCISGInterfileWriter.cxx,v $
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
#include "vtkCISGInterfileWriter.h"
#include "vtkObjectFactory.h"
#include "vtkShortArray.h"
#include "vtkCharArray.h"
#include "vtkIntArray.h"
#include "vtkUnsignedShortArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUnsignedIntArray.h"
#include <ctype.h>
#include "vtkFloatArray.h"

#include "vtkCISGCofstream.h"


#define MAXEXTENTION 10

//----------------------------------------------------------------------------
vtkCISGInterfileWriter* vtkCISGInterfileWriter::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkCISGInterfileWriter");
  if(ret)
    {
    return (vtkCISGInterfileWriter*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkCISGInterfileWriter;
}



int vtkCISGInterfileWriter::Check(char *filename) {
  int i= strlen(filename)-1;
  int j=0;
  char extention[MAXEXTENTION];
  while ((i>=0) && (filename[i]!='.') && (filename[i]!='/') 
         && (j<MAXEXTENTION)) {
    extention[j]=filename[i];
    i--; j++;
  }

  // if the first letter of the extention is 'A' then 
  // I assume that this is supposed to be a Interfile
  return (j>0) && (extention[j-1]=='A');
}

// 
// Writing the header by copying it from the original header
//
int vtkCISGInterfileWriter::CopyHeaderFile(FILE *ofile, 
                                           vtkCISGInterfileReader *reader, 
                                           char *ImageFileName) {

  FILE *ifile;
  char *InputFileName = reader->GetFileName();
  if ((!InputFileName)        || 
      ((ifile = fopen(InputFileName, "rb")) == NULL)){
    vtkErrorMacro(<< "CopyHeaderFile: Cannot open input file.");
    return 0;
  }

  char line[256];
  char lowerline[256];
  char *c;
  //  int j;
  while(!feof(ifile) && fgets(line,256,ifile) ) {

    // copy line and tolower it
    strcpy(lowerline,line);
    c = lowerline;
    while(*c) { *c=tolower((int)*c); c++; }

    if (strstr(lowerline,"name of data file") != NULL) {
      fprintf(ofile, "!name of data file := %s\n",ImageFileName);

    } else if ((strstr(lowerline,"matrix size (x)") != NULL) ||
               (strstr(lowerline,"matrix size [1]")!=NULL)) {
      // write dim x
      fprintf(ofile,"!matrix size [1] :=  %d\n",this->Dim[0]);

    } else if ((strstr(lowerline,"matrix size (y)") != NULL) || 
               (strstr(lowerline,"matrix size [2]")!=NULL)) {
      // write dim y
      fprintf(ofile,"!matrix size [2] :=  %d\n",this->Dim[1]);

    } else if (strstr(lowerline,"number of slices") != NULL) {
      // write dim z
      fprintf(ofile,"!number of slices :=  %d\n",this->Dim[2]);

    } else if (strstr(lowerline,"x scaling factor") != NULL) {
      // write voxel size x
      fprintf(ofile,"!x scaling factor (mm/pixel) :=  %f\n",this->Size[0]);

    } else if (strstr(lowerline,"scaling factor (mm/pixel) [1]") != NULL) {
      // write voxel size x
     fprintf(ofile,"!scaling factor (mm/pixel) [1] :=  %f\n",this->Size[0]);
     
    } else if (strstr(lowerline,"y scaling factor") != NULL) {
      // write voxel size y
      fprintf(ofile,"!y scaling factor (mm/pixel) :=  %f\n",this->Size[1]);
      // to be sure that a scaling factor in z direction is defined we 
      // it now
      fprintf(ofile,"!scaling factor (mm/pixel) [3] :=  %f\n",
              this->Size[2]/this->Size[0]);
      fprintf(ofile,"slice thickness (pixels) :=  %g\n",
              this->Size[2]/this->Size[0]);
      
    } else if (strstr(lowerline,"scaling factor (mm/pixel) [2]") != NULL) {
      // write voxel size y
      fprintf(ofile,"!scaling factor (mm/pixel) [2] :=  %f\n",this->Size[1]);
      // to be sure that a scaling factor in z direction is defined we 
      // it now
      fprintf(ofile,"!scaling factor (mm/pixel) [3] :=  %f\n",
              this->Size[2]/this->Size[0]);
      fprintf(ofile,"slice thickness (pixels) :=  %g\n",
              this->Size[2]/this->Size[0]);
      
   } else if (strstr(lowerline,"slice thickness") != NULL) {
     // we have already written theslice thickness (see above). 
     // so we don't need to do anything here.
     
   } else if (strstr(lowerline,"scaling factor (mm/pixel) [3]") != NULL) {
     // we have already written the scaling factor (see above). 
     // so we don't need to do anything here.
     
   } else if (strstr(lowerline,"centre-centre slice separation (pixels)") != NULL) {
     // we specified the slice thickness - so we do not need the seperation
     
   } else if (strstr(lowerline,"number of bytes per pixel") != NULL) {
     // write number of bytes
     fprintf(ofile,"!number of bytes per pixel := ");
     switch (this->GetInput()->GetScalarType()){
     case VTK_CHAR: fprintf(ofile,"%d \n", sizeof(char)); break;
     case VTK_UNSIGNED_CHAR: fprintf(ofile,"%d \n", sizeof(unsigned char)); break;
     case VTK_SHORT: fprintf(ofile,"%d \n", sizeof(short)); break;
     case VTK_UNSIGNED_SHORT: fprintf(ofile,"%d \n", sizeof(unsigned short)); break;
     case VTK_INT: fprintf(ofile,"%d \n", sizeof(int)); break;
     case VTK_UNSIGNED_INT: fprintf(ofile,"%d \n", sizeof(unsigned int)); break;
     case VTK_FLOAT:fprintf(ofile,"%d \n", sizeof(float)); break;
     default:
       fprintf(ofile,"2 \n"); 
     }

   } else if (strstr(lowerline,"number format") != NULL) {
     // write number format
     switch (this->GetInput()->GetScalarType()){
     case VTK_CHAR: 
       fprintf(ofile,"!number format := SIGNED INTEGER\n");
       break;
     case VTK_UNSIGNED_CHAR:
       fprintf(ofile,"!number format := UNSIGNED INTEGER\n");
       break;
     case VTK_SHORT:
       fprintf(ofile,"!number format := SIGNED INTEGER\n");
       break;
     case VTK_UNSIGNED_SHORT:
       fprintf(ofile,"!number format := UNSIGNED INTEGER\n");
       break;
     case VTK_INT:
       fprintf(ofile,"!number format := SIGNED INTEGER\n");
       break;
     case VTK_UNSIGNED_INT:
       fprintf(ofile,"!number format := UNSIGNED INTEGER\n");
       break;
     case VTK_FLOAT:
       fprintf(ofile,"!number format := SHORT FLOAT\n");
       break;
     default:
       fprintf(ofile,"!number format := SIGNED INTEGER\n");
     }

   }  else if (strstr(lowerline,"byte order") != NULL ) {
     // We write always BIGENDIAN
     fprintf(ofile,"imagedata byte order := BIGENDIAN\n");
   } else {
     // write header line the same as input
     fprintf(ofile,"%s",line);

   }
  }

  fclose(ifile);
  return 1;
}


void vtkCISGInterfileWriter::CreateDefaultHeader(FILE *ofile, 
                                            char *ImageFileName) {
  fprintf(ofile,"!INTERFILE := \n");
  fprintf(ofile,"!imaging modality := nucmed\n");
  fprintf(ofile,"!originating system := unknown\n");
  fprintf(ofile,"!version of keys := 3.3\n");
  fprintf(ofile,"date of keys := 1992:01:01\n");
  fprintf(ofile,"conversion program := VTK CISG Registration Toolkit\n");
  fprintf(ofile,"program author := T. Hartkens <thomas@hartkens.de>\n");
  fprintf(ofile,"program version := V1.0\n");
  fprintf(ofile,"program date := 2000:02:25\n");
  fprintf(ofile,";\n");
  fprintf(ofile,"!GENERAL DATA := \n");
  fprintf(ofile,"original institution := unknown\n");
  fprintf(ofile,"data description := unknown\n");
  fprintf(ofile,"!name of data file := %s\n", ImageFileName);
  fprintf(ofile,"patient name := PATIENT NAME\n");
  fprintf(ofile,"!patient ID := 1234567\n");
  fprintf(ofile,"patient dob := 1941:05:15\n");
  fprintf(ofile,"patient sex := Unknown\n");
  fprintf(ofile,"!study ID := 0000000\n");
  fprintf(ofile,"exam type := 1.1\n");
  fprintf(ofile,"comments := default header\n");
  fprintf(ofile,"isotope := unkown\n");
  fprintf(ofile,"data compression := none\n");
  fprintf(ofile,"data encode := none\n");
  fprintf(ofile,";\n");
  fprintf(ofile,"!GENERAL IMAGE DATA := \n");
  fprintf(ofile,"!type of data := Tomographic\n");
  fprintf(ofile,"!total number of images :=  %d \n",this->Dim[2]);
  fprintf(ofile,"study date := 1999:03:31\n");
  fprintf(ofile,"study time := 09:48:24\n");
  fprintf(ofile,"imagedata byte order := BIGENDIAN\n");
  fprintf(ofile,"date last processed := 2000:00:00\n");
  fprintf(ofile,"time last processed := 16:00:00\n");
  fprintf(ofile,"process label := ACR_NEM\n");
  fprintf(ofile,"archive status := N\n");
  fprintf(ofile,"number of energy windows :=    1\n");
  fprintf(ofile,"flood corrected := N\n");
  fprintf(ofile,"decay corrected := N\n");
  fprintf(ofile,";\n");
  fprintf(ofile,"!SPECT STUDY (general) := \n");
  fprintf(ofile,"number of detector heads :=    1\n");
  fprintf(ofile,"!number of images/energy window :=   16\n");
  fprintf(ofile,"!matrix size [1] :=  %d\n",this->Dim[0]);
  fprintf(ofile,"!matrix size [2] :=  %d\n", this->Dim[1]);
  fprintf(ofile,"!scaling factor (mm/pixel) [1] :=  %f\n",this->Size[0]);
  fprintf(ofile,"!scaling factor (mm/pixel) [2] :=  %f\n",this->Size[0]);
  fprintf(ofile,"!scaling factor (mm/pixel) [3] :=  %f\n",this->Size[2]/this->Size[0]);
  fprintf(ofile,"slice thickness (pixels) :=  %f\n", this->Size[2]/this->Size[0]);
  fprintf(ofile,"!number format := ");
  switch (this->GetInput()->GetScalarType()){
  case VTK_CHAR: fprintf(ofile,"SIGNED INTEGER\n"); break;
  case VTK_UNSIGNED_CHAR: fprintf(ofile,"UNSIGNED INTEGER\n"); break;
  case VTK_SHORT: fprintf(ofile,"SIGNED INTEGER\n"); break;
  case VTK_UNSIGNED_SHORT: fprintf(ofile,"UNSIGNED INTEGER\n"); break;
  case VTK_INT:fprintf(ofile,"SIGNED INTEGER\n"); break;
  case VTK_UNSIGNED_INT: fprintf(ofile,"UNSIGNED INTEGER\n"); break;
  case VTK_FLOAT: fprintf(ofile,"SHORT FLOAT\n"); break;
  default: fprintf(ofile,"SIGNED INTEGER\n");
  }
  fprintf(ofile,"!number of bytes per pixel := ");
  switch (this->GetInput()->GetScalarType()){
  case VTK_CHAR: fprintf(ofile,"%d \n", sizeof(char)); break;
  case VTK_UNSIGNED_CHAR: fprintf(ofile,"%d \n", sizeof(unsigned char)); break;
  case VTK_SHORT: fprintf(ofile,"%d \n", sizeof(short)); break;
  case VTK_UNSIGNED_SHORT: fprintf(ofile,"%d \n", sizeof(unsigned short)); break;
  case VTK_INT: fprintf(ofile,"%d \n", sizeof(int)); break;
  case VTK_UNSIGNED_INT: fprintf(ofile,"%d \n", sizeof(unsigned int)); break;
  case VTK_FLOAT:fprintf(ofile,"%d \n", sizeof(float)); break;
  default:
    fprintf(ofile,"2 \n"); 
  }
  fprintf(ofile,"!process status := reconstructed\n");
  fprintf(ofile,";\n");
  fprintf(ofile,"!SPECT STUDY (reconstructed data) := \n");
  fprintf(ofile,"!number of slices :=  %d\n", this->Dim[2]);
  fprintf(ofile,"number of reference frame :=    0\n");
  fprintf(ofile,"slice orientation := Unkown\n");
  fprintf(ofile,"scatter corrected := N\n");
  fprintf(ofile,"oblique reconstruction := N\n");
  fprintf(ofile,"slice angle :=    0\n");
  fprintf(ofile,"!END OF INTERFILE := \n");

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void vtkCISGInterfileWriter::WriteData()
{
  int i;

  vtkDebugMacro(<<"Writing vtk structured points...");

  // Validate instance variables
  if (this->FileName == NULL) {
    vtkErrorMacro(<< "Filename is NULL");
    return;
  }

  // Set up the data for the header
  vtkStructuredPoints *input=this->GetInput();
  input->GetDimensions(Dim);
  input->GetSpacing(Size);
  input->GetOrigin(Origin);
  vtkDataArray *scalars=input->GetPointData()->GetScalars();
  int nvoxels = Dim[0]*Dim[1]*Dim[2];
  short *voxels = ((vtkShortArray *)scalars)->WritePointer(0, nvoxels);
  input->GetPointData()->GetScalars()->ComputeRange(0);
  float *range=input->GetPointData()->GetScalars()->GetRange();


  // Create the filename of the data file
  char *imagename= new char [255];
  sprintf(imagename, "%s", this->FileName);

  i= strlen(imagename)-1;
  while ((i>=0) && (imagename[i]!='.') && (imagename[i]!='/')) i--; 
  imagename[i+1] = 'I';

  char *ImageNameInHeader;
  i= strlen(imagename)-1;
  while ((i>=0) && (imagename[i]!='/')) i--; 
  if (imagename[i]=='/') {
    ImageNameInHeader=imagename+i+1;
  } else {
    ImageNameInHeader=imagename;
  }

  

  // Open file 
  FILE *fp;
  if ((fp = fopen(this->FileName, "w")) == NULL){
    vtkErrorMacro(<< "Can't open filename " << FileName);
    return;
  }
  
  // If a reader is specified and this reader was an Interfile reader
  // then the header is copied from the original file.
  if (Reader && 
      !(strcmp(Reader->GetClassName(),"vtkCISGInterfileReader"))) {
    if (!this->CopyHeaderFile(fp, 
                              (vtkCISGInterfileReader* )Reader,
                              ImageNameInHeader)) {
      this->CreateDefaultHeader(fp, ImageNameInHeader);
    }
  }  else {
    this->CreateDefaultHeader(fp, ImageNameInHeader);
  }
  fclose (fp);
  

  // Write Image Data

  vtkCISGCofstream to;
  to.Open(imagename);

  vtkDataArray *DataArray=input->GetPointData()->GetScalars();

  switch (input->GetScalarType()){
  case VTK_CHAR: 
    {
      char *data=((vtkCharArray *)DataArray)->GetPointer(0);
      InternalFlip(1,data,this->Dim);
      to.WriteAsChar(data,nvoxels,0);
      InternalFlip(1,data,this->Dim);
    }
    break;
  case VTK_UNSIGNED_CHAR:
    {
      unsigned char *data=
        ((vtkUnsignedCharArray *)DataArray)->GetPointer(0);
      InternalFlip(1,data,this->Dim);
      to.WriteAsUChar(data,nvoxels,0);
      InternalFlip(1,data,this->Dim);
    }
    break;
  case VTK_SHORT:
    {
      short *data=
        ((vtkShortArray *)DataArray)->GetPointer(0);
      InternalFlip(1,data,this->Dim);
      to.WriteAsShort(data,nvoxels,0);
      InternalFlip(1,data,this->Dim);
    }
    break;
  case VTK_UNSIGNED_SHORT:
    {
      unsigned short *data=
        ((vtkUnsignedShortArray *)DataArray)->GetPointer(0);
      InternalFlip(1,data,this->Dim);
      to.WriteAsUShort(data,nvoxels,0);
      InternalFlip(1,data,this->Dim);
    }
    break;
  case VTK_INT:
    {
      int *data=
        ((vtkIntArray *)DataArray)->GetPointer(0);
      InternalFlip(1,data,this->Dim);
      to.WriteAsInt(data,nvoxels,0);
      InternalFlip(1,data,this->Dim);
    }
    break;
  case VTK_UNSIGNED_INT:
    {
      unsigned int *data=
        ((vtkUnsignedIntArray *)DataArray)->GetPointer(0);
      InternalFlip(1,data,this->Dim);
      to.WriteAsUInt(data,nvoxels,0);
      InternalFlip(1,data,this->Dim);
    }
    break;
  case VTK_FLOAT:
    {
      float *data=
        ((vtkFloatArray *)DataArray)->GetPointer(0);
      InternalFlip(1,data,this->Dim);
      to.WriteAsFloat(data,nvoxels,0);
      InternalFlip(1,data,this->Dim);
    }
    break;
  default:
    vtkErrorMacro(<< "WriteVolume(): Unknown voxel type");
    exit(1);
  }

  to.close();

}

void vtkCISGInterfileWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkDataWriter::PrintSelf(os,indent);
}


void vtkCISGInterfileWriter::Update(){
  // prevent, that vtkCISGMultiWriter::Update() is called!
  this->vtkDataWriter::Update();
}


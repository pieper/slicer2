/*=========================================================================
 
  Library   : vtk/objects/library/CISGImage
  Module    : $RCSfile: vtkCISGAnalyzeWriter.cxx,v $
  Authors   : Thomas Hartkens <thomas.hartkens@kcl.ac.uk>
              Daniel Rueckert <d.rueckert@doc.ic.ac.uk>
              Julia Schnabel  <julia.schnabel@kcl.ac.uk>
              partly based on ideas by Colin Studholme
              under the supervision of Dave Hawkes and Derek Hill
  Copyright : King's College London
              Div. of Radiological Sciences, 
              Computational Imaging Science Group, 1997 - 2000
  Purpose   : 
  Date      : $Date: 2005/01/31 20:44:29 $
  Version   : $Revision: 1.3 $
=========================================================================*/
#include "vtkCISGAnalyzeWriter.h"
#include "vtkObjectFactory.h"
#include "vtkShortArray.h"
#include "vtkCharArray.h"
#include "vtkIntArray.h"
#include "vtkUnsignedShortArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUnsignedIntArray.h"
#include "vtkFloatArray.h"

#include "vtkAnalyze.h"
#include "vtkCISGCofstream.h"


#define MAXEXTENTION 10

//----------------------------------------------------------------------------
vtkCISGAnalyzeWriter* vtkCISGAnalyzeWriter::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkCISGAnalyzeWriter");
  if(ret)
    {
    return (vtkCISGAnalyzeWriter*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkCISGAnalyzeWriter;
}



int vtkCISGAnalyzeWriter::Check(char *filename) {
  int i= strlen(filename)-1;
  int j=0;
  char extention[MAXEXTENTION];
  while ((i>=0) && (filename[i]!='.') 
         && (filename[i]!='/')  && (filename[i]!='\\')  
         && (j<MAXEXTENTION)) {
    extention[j]=filename[i];
    i--; j++;
  }
  return !strncmp(extention,"RDH",3) || !strncmp(extention,"rdh",3);
}



//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void vtkCISGAnalyzeWriter::WriteData()
{
  int i;
  int   sizeof_hdr;
  char  padding1[28];
  int   extents;
  short int session_error;
  //  short padding2;
  char  regular;   
  char  padding3;
  short dims[5];
  short padding4[10];
  short data_type;
  short bits;    
  short padding5;    
  float pixdims[5];
  float padding6[11];
  int   glmax;   
  int   glmin;   
  char  padding7[168];
  int   padding8[8];

  vtkDebugMacro(<<"Writing vtk structured points...");

  // Validate instance variables
  if (this->FileName == NULL) {
    vtkErrorMacro(<< "Filename is NULL");
    return;
  }

  vtkCISGCofstream to;
  to.Open(this->GetFileName());
  if (to.is_open() != True) {
      vtkErrorMacro(<<"Can't open file " << this->GetFileName());
      return;
  }
  
  vtkStructuredPoints *input=this->GetInput();

  input->GetDimensions(Dim);
  input->GetSpacing(Size);
  input->GetOrigin(Origin);
  // Calculate data address
  vtkDataArray *scalars=input->GetPointData()->GetScalars();
  // calculate the number of voxels
  int nvoxels = Dim[0]*Dim[1]*Dim[2];
  // get a pointer to the scalar data
  short *voxels = ((vtkShortArray *)scalars)->WritePointer(0, nvoxels);

  input->GetPointData()->GetScalars()->ComputeRange(0);
  vtkFloatingPointType *range=input->GetPointData()->GetScalars()->GetRange();



  // Initialize header
  sizeof_hdr=348;
  extents=16384;
  session_error=(short)16384;
  regular='r';  

  for (i = 0; i < 28; i++) padding1[i] = 0;
  padding3 = 0;
  for (i = 0; i < 10; i++) padding4[i] = 0;
  padding5 = 0;
  for (i = 0; i < 11; i++) padding6[i] = 0.0;
  for (i = 0; i < 168; i++) padding7[i] = 0;
  for (i = 0; i < 8; i++) padding8[i] = 0;


  dims[0]    = 4;
  dims[1]    = Dim[0];
  dims[2]    = Dim[1];
  dims[3]    = Dim[2];
  dims[4]    = 1;

  // determine type
  int pixsize=1;
  switch(input->GetScalarType()){
  case VTK_UNSIGNED_CHAR:
    data_type  = ANALYZE_U_CHAR;
    pixsize = 1;
    break;
  case VTK_SHORT:
    data_type  = ANALYZE_SHORT;         
    pixsize = 2;
    break;
  case VTK_INT:
    data_type  = ANALYZE_INT;
    pixsize = 4;
    break;
  case VTK_FLOAT:
    data_type  = ANALYZE_FLOAT;
    pixsize = 4;
    break;
  default:
    vtkErrorMacro(<< "WriteData(): Voxel type is not supported.");
    //exit(1);
    return;
  }
  bits       = 8 * pixsize;

  pixdims[0] = 0.0;
  pixdims[1] = Size[0];
  pixdims[2] = Size[1];
  pixdims[3] = Size[2];
  pixdims[4] = 1.0;
  glmin      = (int) range[0];
  glmax      = (int) range[1];

  // Write header 
  to.WriteAsInt(sizeof_hdr, 0);
  to.WriteAsChar(padding1, 28, 4);
  to.WriteAsInt(extents, 32);
  to.WriteAsShort(session_error, 36);
  //  to.WriteAsShort(padding2, 36);
  to.WriteAsChar(regular, 38);
  to.WriteAsChar(padding3, 39);
  to.WriteAsShort(dims, 5, 40);
  to.WriteAsShort(padding4, 10, 50);
  to.WriteAsShort(data_type, 70);
  to.WriteAsShort(bits, 72);
  to.WriteAsShort(padding5, 74);
  to.WriteAsFloat(pixdims, 5, 76);
  to.WriteAsFloat(padding6, 11, 96);
  to.WriteAsInt(glmax, 140);
  to.WriteAsInt(glmin, 144);
  to.WriteAsChar(padding7, 168, 148);
  to.WriteAsInt(padding8, 8, 316);

  to.Close();

  int length;
  char imagename[255];

  // Parse directory name
  length = strlen(this->FileName);
  sprintf(imagename, "%s", this->FileName);
  imagename[length-1] = 'g';
  imagename[length-2] = 'm';
  imagename[length-3] = 'i';

  to.Open(imagename);
  if (to.is_open() != True) {
      vtkErrorMacro(<< "vtkCISGAnalyzeWriter: WriteData: Cannot open file " << imagename);
      return;
  }
 
  vtkDataArray *DataArray=input->GetPointData()->GetScalars();

  switch(input->GetScalarType()){
  case VTK_CHAR: 
    {
      char *data=
        ((vtkCharArray *)DataArray)->GetPointer(0);
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
    //exit(1);
    return;
  }

  to.close();

}

void vtkCISGAnalyzeWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkDataWriter::PrintSelf(os,indent);
}


void vtkCISGAnalyzeWriter::Update(){
  // prevent, that vtkCISGMultiWriter::Update() is called!
  this->vtkDataWriter::Update();
}


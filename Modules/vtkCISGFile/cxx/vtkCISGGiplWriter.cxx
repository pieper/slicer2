/*=========================================================================
 
  Library   : vtk/objects/library/CISGImage
  Module    : $RCSfile: vtkCISGGiplWriter.cxx,v $
  Authors   : Thomas Hartkens <thomas.hartkens@kcl.ac.uk>
              Daniel Rueckert <d.rueckert@doc.ic.ac.uk>
              Julia Schnabel  <julia.schnabel@kcl.ac.uk>
              partly based on ideas by Colin Studholme
              under the supervision of Dave Hawkes and Derek Hill
  Copyright : King's College London
              Div. of Radiological Sciences, 
              Computational Imaging Science Group, 1997 - 2000
  Purpose   : 
  Date      : $Date: 2004/09/16 18:52:03 $
  Version   : $Revision: 1.2 $
=========================================================================*/
#include "vtkCISGGiplWriter.h"
#include "vtkObjectFactory.h"
#include "vtkShortArray.h"
#include "vtkCharArray.h"
#include "vtkIntArray.h"
#include "vtkUnsignedShortArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUnsignedIntArray.h"
#include "vtkFloatArray.h"

#include "vtkGipl.h"
#include "vtkCISGCofstream.h"



//----------------------------------------------------------------------------
vtkCISGGiplWriter* vtkCISGGiplWriter::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkCISGGiplWriter");
  if(ret)
    {
    return (vtkCISGGiplWriter*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkCISGGiplWriter;
}



#define MAXEXTENTION 10

int vtkCISGGiplWriter::Check(char *filename) {
  int i= strlen(filename)-1;
  int j=0;
  char extention[MAXEXTENTION];
  while ((i>=0) && (filename[i]!='.') 
         && (filename[i]!='/') && (filename[i]!='\\') 
         && (j<MAXEXTENTION)) {
    extention[j]=filename[i];
    i--; j++;
  }
  return !strncmp(extention,"LPIG",4) || !strncmp(extention,"lpig",4);
}



//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void vtkCISGGiplWriter::WriteData()
{
  int i;

  vtkDebugMacro(<<"Writing vtk structured points...");

  // Validate instance variables
  if (this->FileName == NULL) {
    vtkErrorMacro(<< "Filename is NULL");
    return;
  }

  if (this->GetInput() == NULL) {
    vtkErrorMacro(<< "WriteData: No Input specified.");
    return;
  }

  vtkCISGCofstream to;
  to.Open(this->GetFileName());

  int Dimensions[3];
  vtkFloatingPointType PixelSize[3];
  vtkFloatingPointType DataOrigin[3], GIPLOrigin[3] ;

  vtkStructuredPoints *input= (vtkStructuredPoints *)this->GetInput();

  //  vtkIndent indent;
  //  input->PrintSelf(cerr,indent.GetNextIndent());

  input->GetDimensions(Dimensions);
  input->GetSpacing(PixelSize);
  input->GetOrigin(DataOrigin);
  // Calculate data address
  vtkDataArray *scalars=input->GetPointData()->GetScalars();
  // calculate the number of voxels
  int nvoxels = Dimensions[0]*Dimensions[1]*Dimensions[2]*
    input->GetNumberOfScalarComponents();
  GIPLOrigin[0] =  DataOrigin[0] + (PixelSize[0]*(Dimensions[0]-1)/2.0); 
  GIPLOrigin[1] =  DataOrigin[1] + (PixelSize[1]*(Dimensions[1]-1)/2.0); 
  GIPLOrigin[2] =  DataOrigin[2] + (PixelSize[2]*(Dimensions[2]-1)/2.0); 

  // Write image dimensions
  to.WriteAsShort(Dimensions[0], 0);
  to.WriteAsShort(Dimensions[1], 2);
  to.WriteAsShort(Dimensions[2], 4);
  to.WriteAsShort(1, 6);

  // Write type

  switch(input->GetScalarType()){
  case VTK_CHAR: 
    to.WriteAsShort(GIPL_CHAR, 8);
    break;
  case VTK_UNSIGNED_CHAR:
    to.WriteAsShort(GIPL_U_CHAR, 8);
    break;
  case VTK_SHORT:
    to.WriteAsShort(GIPL_SHORT, 8);
    break;
  case VTK_UNSIGNED_SHORT:
    to.WriteAsShort(GIPL_U_SHORT, 8);
    break;
  case VTK_INT:
    to.WriteAsShort(GIPL_INT, 8);
    break;
  case VTK_UNSIGNED_INT:
    to.WriteAsShort(GIPL_U_INT, 8);
    break;
  case VTK_FLOAT:
    to.WriteAsShort(GIPL_FLOAT, 8);
    break;
  default:
    vtkErrorMacro(<< "WriteVolume(): Unknown voxel type");
    exit(1);
  }


  // Write voxel dimensions
  to.WriteAsFloat(PixelSize[0], 10);
  to.WriteAsFloat(PixelSize[1], 14);
  to.WriteAsFloat(PixelSize[2], 18);
  to.WriteAsFloat(0, 22);

  // Write patient description
  for (i = 0; i < 80; i++){
    to.WriteAsChar(0, 26+i*sizeof(char));
  }

  // Write matrix
  for (i = 0; i < 12; i++){
    if (i % 5){
      to.WriteAsFloat(0, 106+i*sizeof(float));
    } else {
      to.WriteAsFloat(1, 106+i*sizeof(float));
    }
  }

  // Write identifier
  to.WriteAsInt(0, 154);

  // Write spare bytes
  for (i = 0; i < 28; i++){
    to.WriteAsChar(0, 158+i*sizeof(char));
  }

  // Write flag and orientation
  to.WriteAsChar(0, 186);
  to.WriteAsChar(0, 187);

  vtkFloatingPointType *range=input->GetScalarRange();
  to.WriteAsDouble(range[0], 188);
  to.WriteAsDouble(range[1], 196);

  // Write origin
  to.WriteAsDouble(0, 204);
  to.WriteAsDouble(0, 212);
  to.WriteAsDouble(0, 220);
  to.WriteAsDouble(0, 228);

  to.WriteAsFloat(0, 236);
  to.WriteAsFloat(0, 240);

  // User defined flag
  to.WriteAsFloat(0, 244);

  // Write No of components
  to.WriteAsShort(170, 248);
  to.WriteAsShort(input->GetNumberOfScalarComponents(), 250);

  // Write magic number 
  to.WriteAsUInt (GIPL_MAGIC, 252);


  vtkDataArray *DataArray=input->GetPointData()->GetScalars();


  switch(input->GetScalarType()){
  case VTK_CHAR: 
    {
      char *data=
        ((vtkCharArray *)DataArray)->GetPointer(0);
      to.WriteAsChar(data,nvoxels,GIPL_HEADERSIZE);
    }
    break;
  case VTK_UNSIGNED_CHAR:
    {
      unsigned char *data=
        ((vtkUnsignedCharArray *)DataArray)->GetPointer(0);
      to.WriteAsUChar(data,nvoxels,GIPL_HEADERSIZE);
    }
    break;
  case VTK_SHORT:
    {
      short *data=
        ((vtkShortArray *)DataArray)->GetPointer(0);
      to.WriteAsShort(data,nvoxels,GIPL_HEADERSIZE);
    }
    break;
  case VTK_UNSIGNED_SHORT:
    {
      unsigned short *data=
        ((vtkUnsignedShortArray *)DataArray)->GetPointer(0);
      to.WriteAsUShort(data,nvoxels,GIPL_HEADERSIZE);
    }
    break;
  case VTK_INT:
    {
      int *data=
        ((vtkIntArray *)DataArray)->GetPointer(0);
      to.WriteAsInt(data,nvoxels,GIPL_HEADERSIZE);
    }
    break;
  case VTK_UNSIGNED_INT:
    {
      unsigned int *data=
        ((vtkUnsignedIntArray *)DataArray)->GetPointer(0);
      to.WriteAsUInt(data,nvoxels,GIPL_HEADERSIZE);
    }
    break;
  case VTK_FLOAT:
    {
      float *data=
        ((vtkFloatArray *)DataArray)->GetPointer(0);
      to.WriteAsFloat(data,nvoxels,GIPL_HEADERSIZE);
    }
    break;
  default:
    vtkErrorMacro(<< "WriteVolume(): Unknown voxel type");
    exit(1);
  }

  to.close();

}

void vtkCISGGiplWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkDataWriter::PrintSelf(os,indent);
}


void vtkCISGGiplWriter::Update(){
  // prevent, that vtkCISGMultiWriter::Update() is called!
  this->vtkDataWriter::Update();
}








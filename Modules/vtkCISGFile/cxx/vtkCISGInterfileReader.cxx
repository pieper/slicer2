/*=========================================================================
 
  Library   : vtk/objects/library/CISGImage
  Module    : $RCSfile: vtkCISGInterfileReader.cxx,v $
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
#include "vtkCISGInterfileReader.h"
#include "vtkObjectFactory.h"
#include "vtkShortArray.h"
#include "vtkCharArray.h"
#include "vtkIntArray.h"
#include "vtkUnsignedShortArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUnsignedIntArray.h"
#include "vtkFloatArray.h"
#include <string.h>
#include <ctype.h>
// #include <libgen.h>

#include "vtkCISGCifstream.h"

#ifndef WIN32
#define DIRCHAR '\\'
#else 
#define DIRCHAR '/'
#endif

#define IT_BINARY               1
#define IT_CHAR                 7
#define IT_U_CHAR               8
#define IT_SHORT                15
#define IT_U_SHORT              16
#define IT_U_INT                31
#define IT_INT                  32
#define IT_FLOAT                64
#define IT_DOUBLE               65
#define IT_C_SHORT              144
#define IT_C_INT                160
#define IT_C_FLOAT              192
#define IT_C_DOUBLE             193


int vtkCISGInterfileReader::CheckHeader(char *filename){
  FILE *fp;
  if ((fp = fopen(filename, "rb")) == NULL){
    cerr << "Can't open filename " << filename;
    return 0;
  }

  // check if the keyword "interfile" is in the first 20 lines
  char vals[255], *c;
  int i=0;
  while(!feof(fp) && i<20) {
    if (fgets(vals,256,fp) == NULL) {
      perror ("fgets\n");
    }
    
    // convert one to lower case
    c = vals;
    while(*c) { *c=tolower((int)*c); c++; }

    if(strstr(vals,"interfile") != NULL){
      fclose(fp);
      return 1;
    }
    i++;
  }
    
  fclose(fp);
  return 0;
}




vtkCISGInterfileReader* vtkCISGInterfileReader::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkCISGInterfileReader")
;
  if(ret)
    {
    return (vtkCISGInterfileReader*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkCISGInterfileReader;
}


// Construct object with NULL file prefix; file pattern "%s.%d"; image range 
// set to (1,1); data origin (0,0,0); data spacing (1,1,1); no data mask;
// header size 0; and byte swapping turned off.
vtkCISGInterfileReader::vtkCISGInterfileReader(){
}

vtkCISGInterfileReader::~vtkCISGInterfileReader(){
}

void vtkCISGInterfileReader::Execute()
{
  FILE *fp;
  vtkDataArray *newScalars;

  vtkStructuredPoints *output=(vtkStructuredPoints *)this->GetOutput();

  // Validate instance variables
  if (this->FileName == NULL) {
    vtkErrorMacro(<< "Filename is NULL");
    return;
  }


  if ((fp = fopen(this->FileName, "rb")) == NULL){
    vtkErrorMacro(<< "Can't open filename " << FileName);
    return;
  }
  

  // Read header
  this->ReadHeader(fp,FileName);
  fclose(fp);


  // Read data
  newScalars = this->ReadVolume();

  // calculate dimensions of output from data dimensions
  output->SetDimensions(Dim);
  output->SetSpacing(Size);
  output->SetOrigin(Origin);
  output->SetWholeExtent(output->GetExtent());
  // output->ModifyExtentForUpdateExtent();
  output->SetUpdateExtent(output->GetWholeExtent());

  if (newScalars){
    output->GetPointData()->SetScalars(newScalars);
    newScalars->Delete();
  }

}

// Read header of data.
int vtkCISGInterfileReader::ReadHeader(FILE *fp, char *name) {
  
  char          *vals[256];
  char          *originalVals[256];
  int            i=0,j,k;
  char          *c;
  int            isInterfile;
  int            foundFileName;
  int            compression;
  int            encoded;
  int            nx,ny,nz;
  double         vx,vy,vz;
  double         centre_to_centre;
  double         z_scaling_factor;
  double         tmp_slice_thickness;
  int            bytes;
  char           imageDataFilename[256];
  char           *p1, *p2;
  //  short          byteordertest;
  //  char          *byte1, *byte2;
  //  int            needToSwap;
  // long int       totalVox;
  //  long int       vox;
  //  char           originalvalue[2];
  //  char          *imageData;
  //  int            ok;
  int            length;

  // Read header
  // if the file open operation succeeds, then look at header
  if(fp!=NULL) {

    // read up to 256 lines in header
    // each line can be a maximum of 256 characters long
    while(!feof(fp) && i<256) {


      // vals array will be converted to lower case
      // originalVals will be left as it is.
      vals[i]=new char[256];
      originalVals[i]=new char[256];
      
      // get string
      if (fgets(vals[i],256,fp) == NULL) {
                       break;
      }
      
      // copy 
      strcpy(originalVals[i],vals[i]);
      
      // convert one to lower case
      c = vals[i];
      while(*c) { *c=tolower((int)*c); c++; }
      i++;
    }
    
    // close header file
    fclose(fp);

    
    // check that one line has INTERFILE if not return
    isInterfile = 0;
    for(j=0;j<i;j++){
      if(strstr(vals[j],"interfile") != NULL){
               isInterfile = 1;
      }
    }
    if (isInterfile == 0 ) {
      return -1;
    }
    
    // check that we can find image data
    foundFileName = 0;
    for(j=0;j<i;j++){
      if(strstr(vals[j],"!name of data file") != NULL){

        // if image data filename in header
        // is an absolute pathname, then we can use it
        // directly. 
        p1 = strstr(originalVals[j],":=")+3;
        p2 = strrchr(p1,DIRCHAR);
        
        if (p2 != p1 && p2 != NULL) {
        // the string does contain a /
          strcpy(imageDataFilename,strstr(originalVals[j],":=")+3);          
        } else {
          
          // If image data filename is just the filename
          // ie. no directory specified, then we should add
          // onto it the basename from the header file.
          
          // Need to check if we have a basename in header file name
          // This is a bit of a mess as, I found that on some
          // really old systems the basename() function
          // didnt work, or wasnt available
          
          p1 = p2 = name;
          for(k=0;k < (int) strlen(name);k++){
            if((name[k]) == DIRCHAR) {
              p2 = (name+k);
            }
          }
          if(p1!=p2) {
            // p2 must have found a /
            p1 = name;
            length = p2-p1+1;
            strncpy(imageDataFilename,p1,length);
            imageDataFilename[length]=0;
            strcat(imageDataFilename,strstr(originalVals[j],":=")+3);
          } else {
            strcpy(imageDataFilename,strstr(originalVals[j],":=")+3);
          }
        }
        if(strstr(imageDataFilename,"\n")!=NULL){
          *strstr(imageDataFilename,"\n") = '\0';
        }
        vtkDebugMacro(<< "Read image data filename " << imageDataFilename);
        foundFileName = 1;
        
      }
    }
    if (!foundFileName){
      vtkErrorMacro(<< "ERROR: Couldnt find image data file name: I was searching for !name of data file");
      return -2;
    }
  }
    
    // check image is not compressed
    // Modified 16/06/2000
    // If `data compression' field is not
    // specified, then we presume image is not
    // compressed.
    compression = -1;
    for(j=0;j<i;j++){
      if(strstr(vals[j],"data compression") != NULL) {
        if(strstr((strstr(vals[j],":=") + 2),"none") != NULL ) {
          compression = 0;
        } else {
          compression = 1;
        }
      }
    }
    if(compression == 1){
      vtkErrorMacro(<< "ERROR: Can't read compressed Interfiles \n"
                    << "I was looking for data compression := none");
      return -3;
    } else if (compression ==-1){
      vtkDebugMacro(<< "WARNING: Couldnt find data compression flag, I will assume the data is not compressed\n");
    }

    
    // check image for bigendianness
    bigendian = -1;
    for(j=0;j<i;j++){
      if(strstr(vals[j],"byte order") != NULL) {
        if(strstr((strstr(vals[j],":=") + 2),"littleendian") != NULL ) {
          bigendian = 0;
        } else {
          bigendian = 1;
        }
      }
    }
    if (bigendian == -1){
      vtkDebugMacro(<< "WARNING: Can't find byte order, assuming big endian\n");
      bigendian = 1;
    }
    
    // check image is not encoded
    encoded = -1;
    for (j=0;j<i;j++){
      if (strstr(vals[j],"data encode") != NULL) {
        if (strstr((strstr(vals[j],":=") + 2),"none") != NULL ) {
                  encoded = 0;
        } else {
          encoded = 1;
        }
      }
    }
    
    if(encoded == 1){
      vtkErrorMacro(<< "ERROR: Can't read encoded Interfiles\n");
      return -4;
    } else if (encoded == -1) {
      vtkDebugMacro(<< "WARNING: Couldnt find `data encode' flag, I will assume that the image is not encoded\n");
    }
    
    // get number of x pixels
    nx = -1;
    for(j=0;j<i;j++){
      if((strstr(vals[j],"matrix size (x)") != NULL) || (strstr(vals[j],"matrix size [1]")!=NULL))
        nx = atoi(strstr(vals[j],":=")+2);
    }
    if(nx == -1){
      vtkErrorMacro(<< "ERROR: Can't find Interfile number of x pixels\n");
      return -5;
    }
    
    // get number of y pixels
    ny = -1;
    for(j=0;j<i;j++){
      if((strstr(vals[j],"matrix size (y)") != NULL) || (strstr(vals[j],"matrix size [2]")!=NULL))
        ny = atoi(strstr(vals[j],":=")+2);
    }
    if(ny == -1){
      vtkErrorMacro(<< "ERROR: Can't find Interfile number of y pixels\n");
      return -6;
    }
    
    // get number of slices
    nz = -1;
    for(j=0;j<i;j++){
      if(strstr(vals[j],"number of slices") != NULL)
        nz = atoi(strstr(vals[j],":=")+2);
    }
    if(nz == -1){
      vtkErrorMacro(<< "ERROR: Can't find Interfile number of slices - defaulting to 1\n");
      nz = 1;
    }
    
    // get x scale factor - default is 1
    vx = -1;
    for(j=0;j<i;j++){
      if(strstr(vals[j],"x scaling factor") != NULL) {
        vx = (double)atof(strstr(vals[j],":=")+2);
        vtkDebugMacro(<< "Set pixel x size to " << vx);
      }
      
    }
    if (vx == -1) {
      vtkDebugMacro(<< "WARNING: I was looking for \"x scaling factor\" but didnt find it. I'll try looking for \"scaling factor (mm/pixel) [1]\"\n");
      for(j=0;j<i;j++){
        if(strstr(vals[j],"scaling factor (mm/pixel) [1]") != NULL) {
          vx = (double)atof(strstr(vals[j],":=")+2);
          vtkDebugMacro(<< "Set pixel x size to " << vx);
        }
      }
    }
    
    if (vx == -1) {
      vtkDebugMacro(<< "WARNING: I was looking for \"scaling factor (mm/pixel) [1]\" but didnt find it, setting the x pixel size to 1mm\n");
      vx = 1;
    }
    
    // get y scale factor - default is 1

    vy = -1;
    for(j=0;j<i;j++){
      if(strstr(vals[j],"y scaling factor") != NULL) {
        vy = (double)atof(strstr(vals[j],":=")+2);
        vtkDebugMacro(<< "Set pixel y size to " << vy);
      }
    }
    if (vy == -1) {
      vtkDebugMacro(<< "WARNING: I was looking for \"y scaling factor\" but didnt find it. I'll try looking for \"scaling factor (mm/pixel) [2]\"\n");
      for(j=0;j<i;j++){
        if(strstr(vals[j],"scaling factor (mm/pixel) [2]") != NULL) {
          vy = (double)atof(strstr(vals[j],":=")+2);
          vtkDebugMacro(<< "Set pixel y size to " << vy);
        }
      }
    }
    if (vy == -1) {
      vtkDebugMacro(<< "WARNING: I was looking for \"scaling factor (mm/pixel) [2]\" but didnt find it, setting the y pixel size to 1mm\n");
      vy = 1;
    }
    
    // get slice thickness 
    vz = -1;
    for(j=0;j<i;j++){
      if(strstr(vals[j],"slice thickness") != NULL) {
        vz = (double)atof(strstr(vals[j],":=")+2);
        vtkDebugMacro(<< "Set slice thickness to " << vz);
      }
    }
    if (vz == -1 ) {
      vtkDebugMacro(<< "WARNING: Couldnt find \"slice thickness\"\n");
    }
    
    // get z scale factor - default is 1
    z_scaling_factor = -1;
    for(j=0;j<i;j++){
      if(strstr(vals[j],"scaling factor (mm/pixel) [3]") != NULL) {
        z_scaling_factor = (double)atof(strstr(vals[j],":=")+2);
        vtkDebugMacro(<< "Set z scaling factor to " << z_scaling_factor);
      }
    }
    if (z_scaling_factor == -1 ) {
      vtkDebugMacro(<< "WARNING: Couldnt find \"scaling factor (mm/pixel) [3]\"\n");
    }
    
    // get centre to centre ratio

    centre_to_centre = -1;
    for(j=0;j<i;j++){
      if(strstr(vals[j],"centre-centre slice separation (pixels)") != NULL) {
        centre_to_centre = (double)atof(strstr(vals[j],":=")+2);
        vtkDebugMacro(<< "Set centre-centre slice separation to " << centre_to_centre);
        vz=-1;
      }
      
    }
    if (centre_to_centre == -1) {
      vtkDebugMacro(<< "WARNING: I was looking for \"centre-centre slice separation (pixels)\" but didnt find it. I'll have to set it to a default of 1\n");
      centre_to_centre = 1;
    }
    
    
    // check number of bytes
    bytes = 0;
    for(j=0;j<i;j++){
      if(strstr(vals[j],"number of bytes per pixel") != NULL) {
        bytes = atoi(strstr(vals[j],":=")+2);
      }
    }
    /*
      if(bytes != 2){
      vtkErrorMacro(<< "ERROR: wrong number of bytes, should definitely be 2 bytes\n");
      return -7;
    }
    */

    
    // check data type
    for (j=0;j<i;j++) {
      if (strstr(vals[j],"number format") != NULL) {
        
        if (strstr((strstr(vals[j],":=") + 2),"unsigned integer") != NULL ) {
          switch (bytes) {
          case sizeof(unsigned char): this->Type=VTK_UNSIGNED_CHAR; break;
          case sizeof(unsigned short): this->Type=VTK_UNSIGNED_SHORT; break;
          case sizeof(unsigned int): this->Type=VTK_UNSIGNED_INT; break;
          default: 
            vtkErrorMacro(<< "Cannot find an appropiate voxel type");
            exit(0);
          }
        } else if (strstr((strstr(vals[j],":=") + 2),"signed integer") != NULL ) {
          switch (bytes) {
          case sizeof(char): this->Type=VTK_CHAR; break;
          case sizeof(short): this->Type=VTK_SHORT; break;
          case sizeof(int): this->Type=VTK_INT; break;
          default: 
            vtkErrorMacro(<< "Cannot find an appropiate voxel type");
            exit(0);
          }
        } else if (strstr((strstr(vals[j],":=") + 2),"long float") != NULL ) {
          switch (bytes) {
          case sizeof(float): this->Type=VTK_FLOAT; break;
          case sizeof(double): this->Type=VTK_DOUBLE; break;
          default: 
            vtkErrorMacro(<< "Cannot find an appropiate voxel type");
            exit(0);
          }
        } else if (strstr((strstr(vals[j],":=") + 2),"short float") != NULL ) {
          switch (bytes) {
          case sizeof(float): this->Type=VTK_FLOAT; break;
          case sizeof(double): this->Type=VTK_DOUBLE; break;
          default: 
            vtkErrorMacro(<< "Cannot find an appropiate voxel type");
            exit(0);
          }
        } else if (strstr((strstr(vals[j],":=") + 2),"bit") != NULL ) {
          switch (bytes) {
          case sizeof(char): this->Type=VTK_CHAR; break;
          case sizeof(short): this->Type=VTK_SHORT; break;
          case sizeof(int): this->Type=VTK_INT; break;
          default: 
            vtkErrorMacro(<< "Cannot find an appropiate voxel type");
            exit(0);
          }
        } else if (strstr((strstr(vals[j],":=") + 2),"ASCII") != NULL ) {
          switch (bytes) {
          case sizeof(char): this->Type=VTK_CHAR; break;
          case sizeof(short): this->Type=VTK_SHORT; break;
          case sizeof(int): this->Type=VTK_INT; break;
          default: 
            vtkErrorMacro(<< "Cannot find an appropiate voxel type");
            exit(0);
          }
        } else {
          vtkErrorMacro(<< "ERROR: data type unknown.\n");
          return -8;
        }
        
      }
    }
    
    if((nx!=-1)&&(ny!=-1)&&(nz!=-1)) {
      // if we found a scaling factor for the z direction, thats
      // what we use.
      // Otherwise we take the x pixel size, and multiply by the slice
      // thickness
      if (z_scaling_factor != -1) {
        tmp_slice_thickness = vx;
        if (vz != -1){
          tmp_slice_thickness *= vz;
        } else {
          tmp_slice_thickness = 1;
        }
      } else {
        tmp_slice_thickness = vx;
        if (vz != -1){
          tmp_slice_thickness *= vz;
          vtkDebugMacro(<< "vz found to be " << vz);
        } else if (centre_to_centre!=-1) {
          vtkDebugMacro(<< "Using centre_to_centre separation of "
          << centre_to_centre << " pixels and vx of "
          << vx << " mm");
          tmp_slice_thickness *= centre_to_centre;
        }
      }
      vtkDebugMacro(<< "Setting the final z voxel dimension to " << tmp_slice_thickness);
      
    } else {
      vtkErrorMacro(<< "ERROR in Pixel Sizes:" << nx << ny << nz);
    }
    
    // Copy header information to VTK structured points
    Dim[0] = nx;
    Dim[1] = ny;
    Dim[2] = nz;
    Size[0]    = vx;
    Size[1]    = vy;
    Size[2]    = tmp_slice_thickness;
    Origin[0]    = -(vx*(nx-1)/2.0);
    Origin[1]    = -(vy*(ny-1)/2.0);
    Origin[2]    = -(tmp_slice_thickness*(nz-1)/2.0);
    ImageDataFile    = imageDataFilename;
    return 1;
  }





// Read a volume of data.
vtkDataArray *vtkCISGInterfileReader::ReadVolume()
{
  // Create file stream
  vtkCISGCifstream from;

  // Open new file for reading
  from.Open(this->ImageDataFile);
  if (!bigendian) {
    from.FlipSwapping();
  }

  // calculate the number of voxels
  int nvoxels = this->Dim[0] * this->Dim[1] * this->Dim[2];

  // create the short scalars for data
  vtkDataArray *Scalars = vtkDataArray::CreateDataArray(this->Type);
  Scalars->Allocate(1,nvoxels);

  vtkDataArray *DataArray=Scalars;
  switch(this->Type){
  case VTK_CHAR: 
    {
      char *data=
        ((vtkCharArray *)DataArray)->WritePointer(0,nvoxels);
      from.CReadAsChar(data, nvoxels, 0);
      InternalFlip(1,data,this->Dim);
    }
  break;
  case VTK_UNSIGNED_CHAR:
    {
      unsigned char *data=
        ((vtkUnsignedCharArray *)DataArray)->WritePointer(0,nvoxels);
      from.CReadAsUChar(data, nvoxels, 0);
      InternalFlip(1,data,this->Dim);
    }
  break;
  case VTK_SHORT:
    {
      short *data =
        ((vtkShortArray *) DataArray)->WritePointer(0,nvoxels);
      from.CReadAsShort(data, nvoxels, 0);
      InternalFlip(1,data,this->Dim);
    }
  break;
  case VTK_UNSIGNED_SHORT:
    {
      unsigned short *data=
        ((vtkUnsignedShortArray *)DataArray)->WritePointer(0,nvoxels);
      from.CReadAsUShort(data, nvoxels, 0);
      InternalFlip(1,data,this->Dim);
    }
  break;
  case VTK_INT:
    {
      int *data=
      ((vtkIntArray *)DataArray)->WritePointer(0,nvoxels);
      from.CReadAsInt(data, nvoxels, 0);
      InternalFlip(1,data,this->Dim);
    }
  break;
  case VTK_UNSIGNED_INT:
    {
      unsigned int *data=
        ((vtkUnsignedIntArray *)DataArray)->WritePointer(0,nvoxels);
      from.CReadAsUInt(data, nvoxels, 0);
      InternalFlip(1,data,this->Dim);
    }
    break;
  case VTK_FLOAT:
    {
      float *data=
        ((vtkFloatArray *)DataArray)->WritePointer(0,nvoxels);
      from.CReadAsFloat(data, nvoxels, 0);
      InternalFlip(1,data,this->Dim);
    }
    break;
  default:
    vtkErrorMacro(<< "ReadVolume(): Unknown voxel type");
    exit(1);
  }

  from.Close();

  return Scalars;
}


  

void vtkCISGInterfileReader::PrintSelf(ostream& os, vtkIndent indent){
  vtkCISGMultiReader::PrintSelf(os,indent);
}



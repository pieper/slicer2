#include "vtkVolumeTextureMapper3D.h"
#include "vtkCamera.h"
#include "vtkGraphicsFactoryAddition.h"
#include "vtkImageData.h"
#include "vtkLargeInteger.h"
#include "vtkMatrix4x4.h"
#include "vtkPointData.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkTransform.h"
#include "vtkVolumeProperty.h"
#include "math.h"
#include <GL/gl.h>

FILE* fd = fopen("error_vtkVolumeTextureMapper3D.txt", "w");
int currentCounter= 0;
template <class T>

//-----------------------------------------------------
//Name: vtkVolumeTextureMapper3D_TextureOrganization
//Description: Organization of textures
//-----------------------------------------------------
void vtkVolumeTextureMapper3D_TextureOrganization( T *data_ptr,                     
                           int size[3],
                           int volume, 
                           vtkVolumeTextureMapper3D *me )
{

  int              i, j, k;
  int              kstart, kend, kinc;
  unsigned char    *tptr;
  T                *dptr;
  unsigned char    *rgbaArray = me->GetRGBAArray();
  vtkRenderWindow  *renWin = me->GetRenderWindow();
  float            spacing[3];
  unsigned char    *texture;
  int              *zAxis=0, *yAxis=0, *xAxis=0;
  int              loc, inc=0;
  int              textureOffset=0;  
  int           a0=0, a1=1, a2=2;
  int           dimension[3];
  int           factor[2];
  vtkFloatingPointType           tempFactor =0;
  xAxis = &i;
  yAxis = &j;
  zAxis = &k;
  inc = 1;
  
  // Create space for the texture

  texture = new unsigned char[4*size[0]*size[1]];

  me->GetDataSpacing( spacing );

  kstart = 0;
  kend = size[2];
  kinc = me->GetInternalSkipFactor();
  for ( k = kstart; k != kend; k+=kinc )
  {
    for ( j = 0; j < size[a1]; j++ )
    {
      i = 0;
      //tptr is the pointer where to store the texture
      //start at texture and add addresses according to j
      tptr = texture+4*j*size[0];
      loc = (*zAxis)*size[0]*size[1]+(*yAxis)*size[0]+(*xAxis);
      dptr = data_ptr + loc;
      
      for ( i = 0; i < size[a0]; i++ )
      {
    //copy information from (rgbaArray + (*dptr)*4) with the length 4 to tptr
    memcpy( tptr, rgbaArray + (*dptr)*4, 4 );
    tptr += 4;
    dptr += inc;
      }
    }
    
    if ( renWin->CheckAbortStatus() )
    {
      break;
    }
    
    //check if the volume must be rescaled
    me->GetDimension(volume, dimension);
    tempFactor = (vtkFloatingPointType)size[0]/(vtkFloatingPointType)dimension[0];
    if (tempFactor == 1)
    {
      factor[0] = (int)tempFactor;
    }
    else if (tempFactor < 1)
    {
      factor[0] = -(int)(1/tempFactor);
    }
    else
    {
      factor[0]= (int)tempFactor;            
    }
    tempFactor = (vtkFloatingPointType)size[1]/(vtkFloatingPointType)dimension[1];
    if (tempFactor == 1)
    {
      factor[1] = (int)tempFactor;
    }
    else if (tempFactor < 1)
    {
      factor[1] = -(int)(1/tempFactor);
    }
    else
    {
      factor[1]= tempFactor;        
    }
    //scale data to wanted power of two
    me->RescaleData(texture, size, spacing, factor, volume);       
  }
  delete [] texture;
}


//-----------------------------------------------------
//Name: RescaleData  - are going to be rewritten!
//Description: Scaling of the volumes to a different power of two
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::RescaleData(unsigned char* texture,  int size[3], float spacing[3], int scaleFactor[2], int volume)
{
  
  int dim[3];
  this->GetDimension(volume, dim);

  //rescale to wanted power of two and call CreateSubImage in vtkOpenGLVolumeTextureMapper3D
  int texPtr = 0;
  for (int y = 0; y < size[1]; y++)
  {
    for (int x = 0; x < size[0]; x++) 
    {
      tempData[x][y][0] = (int)texture[texPtr];
      histArray[volume][tempData[x][y][0]]++;
      if ((histArray[volume][tempData[x][y][0]] > histMax[volume]) && (tempData[x][y][0] != 0))
      {
    histMax[volume] = histArray[volume][tempData[x][y][0]];
      }
      texPtr=texPtr+4;    
    }
  }
  if ((scaleFactor[0] == 1) && (scaleFactor[1] == 1))
  {
    texPtr = 0;
    for (int y = 0; y < dim[1]; y++)
    {
      for (int x = 0; x < dim[0]; x++) 
      {
    texture[texPtr] = (unsigned char) tempData[x][y][0];
    texPtr++;
      }
    }
    this->CreateSubImages(texture, size, spacing);
  }
  else
  {
    double tempValue = 0;
    int tx = 0;
    int ty = 0;
    //increas the volume
    if ((scaleFactor[0] < 0) && (scaleFactor[1] <0))
    {
      texPtr= 0;
      for (int y = 0; y < size[1]; y++) 
      {
    if (y < (size[1]+scaleFactor[1]))
    {    
      for (int scaleY=0; scaleY < -scaleFactor[1]; scaleY++)
      {
        for (int x = 0; x < size[0]; x++)
        {
          if (x < (size[0]+scaleFactor[0]))
          {
        for (int scaleX=0; scaleX < -scaleFactor[0]; scaleX++)
        {
          tx = scaleX/(-scaleFactor[0]-1);
          ty = scaleY/(-scaleFactor[1]-1);
          //interpolation
          tempValue = (1-tx)*(1-ty)*(vtkFloatingPointType)tempData[x][y][0]+tx
                     *(1-ty)*(vtkFloatingPointType)tempData[x+1][y][0]+ty*(1-tx)
                     *(vtkFloatingPointType)tempData[x][y+1][0]+tx*ty* (vtkFloatingPointType)tempData[x+1][y+1][0];    
          texture[texPtr] = (unsigned char)(ceil(tempValue));
          texPtr++;
        }
          }                        
          else //high  x-values
          {   
        for (int scaleX=0; scaleX < -scaleFactor[0]; scaleX++)
        {
          texture[texPtr]=(unsigned char)(tempData[x][y][0]);    
          texPtr++;
        }
          }
        }
      }
    }
    else //high y-values
    {
      for (int scaleY=0; scaleY < -scaleFactor[1]; scaleY++)
      {
        for (int x = 0; x < size[0]; x++)
        {
          for (int scaleX=0; scaleX < -scaleFactor[0]; scaleX++)
          {
        texture[texPtr]=(unsigned char)(tempData[x][y][0]);
        texPtr++;
          }
        }
      }
    }
      }
    }
    //decrease the volume
    if ((scaleFactor[0]>= 0) && (scaleFactor[1]>=0))
    {  
      texPtr= 0;
      tempValue = 0;
      for (int y = 0; y < size[1]; y+=scaleFactor[1]) 
      {
    for (int x = 0; x < size[0]; x+=scaleFactor[0])
    {
      for (int scaleY=0; scaleY < scaleFactor[1]; scaleY++)
      {
        for (int scaleX=0; scaleX < scaleFactor[0]; scaleX++)
        {
          tempValue = tempValue+tempData[x+scaleX][y+scaleY][0];
        }
      }
      texture[texPtr] = (unsigned char)(ceil(tempValue/(scaleFactor[0]*scaleFactor[1])));
      texPtr++;
      tempValue = 0;
    }        
      }
    }
    //call CreateSubImages in vtkOpenGLVolumeTextureMapping with the rescaled texture
    this->CreateSubImages(texture, size, spacing);
    texPtr =0;
  }
}


vtkCxxRevisionMacro(vtkVolumeTextureMapper3D, "$Revision: 1.3 $");

//----------------------------------------------------------------------------
// Needed when we don't use the vtkStandardNewMacro.
vtkInstantiatorNewMacro(vtkVolumeTextureMapper3D);
//----------------------------------------------------------------------------

//-----------------------------------------------------
//Name: vtkVolumeTextureMapper3D 
//Description: Constructor
//-----------------------------------------------------
vtkVolumeTextureMapper3D::vtkVolumeTextureMapper3D()
{
  this->Texture               = NULL;
  diffX = 0;
  diffY = 0;
  clipPlaneNum = 0;
  numberOfPlanes = 0;
  tMatrixChanged[0] = 1;
  tMatrixChanged[1] = 0;
  tMatrixChanged[2] = 0;
    
   for (int i = 0; i < 3; i++)
   {
     for (int j = 0; j < 4; j++)
       {
     for (int k = 0; k < 4; k++)
       {
         currentTransformation[i][j][k] = 0;
       }
       }    
     currentTransformation[i][0][0] = 1;
     currentTransformation[i][1][1] = 1;
     currentTransformation[i][2][2] = 1;
     currentTransformation[i][3][3] = 1;
   }
}

//-----------------------------------------------------
//Name: ~vtkVolumeTextureMapper3D
//Description: Destructor
//-----------------------------------------------------
vtkVolumeTextureMapper3D::~vtkVolumeTextureMapper3D()
{
  if ( this->Texture )
  {
    delete [] this->Texture;
  }
}


//-----------------------------------------------------
//Name: New
//Description: 
//-----------------------------------------------------
vtkVolumeTextureMapper3D *vtkVolumeTextureMapper3D::New()
{
  // First try to create the object from the vtkObjectFactoryAddition
  vtkObject* ret =  vtkGraphicsFactoryAddition::CreateInstance("vtkVolumeTextureMapper3D");
  return (vtkVolumeTextureMapper3D*)ret;
}

//-----------------------------------------------------
//Name: GenerateTextures
//Description: Generation of textures
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::GenerateTextures( vtkRenderer *ren, vtkVolume *vol, int volume )
{

  vtkImageData           *input = this->GetInput();
  int                    size[3];
  void                   *inputPointer;
  int                    inputType;
  
  inputPointer = input->GetPointData()->GetScalars()->GetVoidPointer(0);
  inputType    = input->GetPointData()->GetScalars()->GetDataType();

  
  histMax[volume] = 0;
  for (int j = 0; j < 256; j++)
  {
    histArray[volume][j] = 0;
  }

  if ( this->Texture )
  {
    delete [] this->Texture;
    this->Texture = NULL;
  }
  
  //is not always updated - very strange!  
  input->GetDimensions(size);
  
  switch ( inputType )
  {
    case VTK_UNSIGNED_CHAR:
      vtkVolumeTextureMapper3D_TextureOrganization( (unsigned char *)inputPointer, size, volume, this );

      break;
    case VTK_UNSIGNED_SHORT:
      vtkVolumeTextureMapper3D_TextureOrganization( (unsigned short *)inputPointer,  size, volume, this );
  
      break;
    default:
      vtkErrorMacro("vtkVolumeTextureMapper3D only works with unsigned short and unsigned char data.\n" << 
          "Input type: " << inputType << " given.");
  }
}

//-----------------------------------------------------
//Name: InitializeRender 
//Description: Initialization
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::InitializeRender( vtkRenderer *ren,
                                                 vtkVolume *vol,
                                                 int majorDirection )
{
  double spacing[3];
  boxSize = 128;
  this->InternalSkipFactor = 1;
  this->GetInput()->GetSpacing(spacing);
  this->DataSpacing[0] = (float) spacing[0];
  this->DataSpacing[1] = (float) spacing[1];
  this->DataSpacing[2] = (float) spacing[2];
  this->vtkVolumeTextureMapper::InitializeRender( ren, vol );





}


void vtkVolumeTextureMapper3D::DefaultValues()
{
  for (int i = 0; i < 3; i++)
    {
      dimension[i][0] = 256;
      dimension[i][1] = 256;
      dimension[i][2] = 256;    
    }    
}

//-----------------------------------------------------
//Name: MultiplyMatrix (internal function)
//Description: Multiplication of a 1x3 matrix and a 3x3 matrix 
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::MultiplyMatrix(double result[3], double rotMatrix[3][3], double original[3])
{
  result[0] = rotMatrix[0][0]*original[0]+rotMatrix[0][1]*original[1]+rotMatrix[0][2]*original[2];
  result[1] = rotMatrix[1][0]*original[0]+rotMatrix[1][1]*original[1]+rotMatrix[1][2]*original[2];
  result[2] = rotMatrix[2][0]*original[0]+rotMatrix[2][1]*original[1]+rotMatrix[2][2]*original[2];
}

//-----------------------------------------------------
//Name: ComputePlaneEquation  (internal function)
//Description: Compute a clip plane equation
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::ComputePlaneEquation(double plane[4], double point[3], double normal[3])
{
  plane[0]=normal[0];
  plane[1]=normal[1];
  plane[2]=normal[2];
  plane[3]=normal[0]*point[0]+normal[1]*point[1]+normal[2]*point[2];
}

//-----------------------------------------------------
//Name: Rotate (internal function)
//Description: Rotation with the result in rot[3][3]
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::Rotate(double rot[3][3], double angle[3])
{
  double rotx[3][3];
  double roty[3][3];
  double rotz[3][3];
  double rotTemp[3][3];
  
  rotx[0][0]= 1;
  rotx[0][1]= 0;
  rotx[0][2]= 0; 
  rotx[1][0]= 0;
  rotx[1][1]= cos(angle[0]);
  rotx[1][2]= sin(angle[0]); 
  rotx[2][0]= 0;
  rotx[2][1]= -sin(angle[0]);
  rotx[2][2]= cos(angle[0]);
  
  roty[0][0]= cos(angle[1]);
  roty[0][1]= 0;
  roty[0][2]= -sin(angle[1]); 
  roty[1][0]= 0;
  roty[1][1]= 1;
  roty[1][2]= 0; 
  roty[2][0]= sin(angle[1]);
  roty[2][1]= 0;
  roty[2][2]= cos(angle[1]);
  
  rotz[0][0]= cos(angle[2]);
  rotz[0][1]= sin(angle[2]);
  rotz[0][2]= 0; 
  rotz[1][0]= -sin(angle[2]);
  rotz[1][1]= cos(angle[2]);
  rotz[1][2]= 0; 
  rotz[2][0]= 0;
  rotz[2][1]= 0;
  rotz[2][2]= 1;
  
  rotTemp[0][0] = rotx[0][0]*roty[0][0]+rotx[0][1]*roty[1][0]+rotx[0][2]*roty[2][0];
  rotTemp[0][1] = rotx[0][0]*roty[0][1]+rotx[0][1]*roty[1][1]+rotx[0][2]*roty[2][1];
  rotTemp[0][2] = rotx[0][0]*roty[0][2]+rotx[0][1]*roty[1][2]+rotx[0][2]*roty[2][2];
  
  rotTemp[1][0] = rotx[1][0]*roty[0][0]+rotx[1][1]*roty[1][0]+rotx[1][2]*roty[2][0];
  rotTemp[1][1] = rotx[1][0]*roty[0][1]+rotx[1][1]*roty[1][1]+rotx[1][2]*roty[2][1];
  rotTemp[1][2] = rotx[1][0]*roty[0][2]+rotx[1][1]*roty[1][2]+rotx[1][2]*roty[2][2];
  
  rotTemp[2][0] = rotx[2][0]*roty[0][0]+rotx[2][1]*roty[1][0]+rotx[2][2]*roty[2][0];    
  rotTemp[2][1] = rotx[2][0]*roty[0][1]+rotx[2][1]*roty[1][1]+rotx[2][2]*roty[2][1];
  rotTemp[2][2] = rotx[2][0]*roty[0][2]+rotx[2][1]*roty[1][2]+rotx[2][2]*roty[2][2];
  
  
  rot[0][0] = rotTemp[0][0]*rotz[0][0]+rotTemp[0][1]*rotz[1][0]+rotTemp[0][2]*rotz[2][0];
  rot[0][1] = rotTemp[0][0]*rotz[0][1]+rotTemp[0][1]*rotz[1][1]+rotTemp[0][2]*rotz[2][1];
  rot[0][2] = rotTemp[0][0]*rotz[0][2]+rotTemp[0][1]*rotz[1][2]+rotTemp[0][2]*rotz[2][2];
  
  rot[1][0] = rotTemp[1][0]*rotz[0][0]+rotTemp[1][1]*rotz[1][0]+rotTemp[1][2]*rotz[2][0];
  rot[1][1] = rotTemp[1][0]*rotz[0][1]+rotTemp[1][1]*rotz[1][1]+rotTemp[1][2]*rotz[2][1];
  rot[1][2] = rotTemp[1][0]*rotz[0][2]+rotTemp[1][1]*rotz[1][2]+rotTemp[1][2]*rotz[2][2];
  
  rot[2][0] = rotTemp[2][0]*rotz[0][0]+rotTemp[2][1]*rotz[1][0]+rotTemp[2][2]*rotz[2][0];    
  rot[2][1] = rotTemp[2][0]*rotz[0][1]+rotTemp[2][1]*rotz[1][1]+rotTemp[2][2]*rotz[2][1];
  rot[2][2] = rotTemp[2][0]*rotz[0][2]+rotTemp[2][1]*rotz[1][2]+rotTemp[2][2]*rotz[2][2];    
  
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// Functions below can be called by users
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


//-o-o-o-o-o-o-o-o-o-o-
//General functions
//-o-o-o-o-o-o-o-o-o-o-

//-----------------------------------------------------
//Name: SetEnableVolume
//Description: vtkVolumeTextureMapper3D supports 3 volumes.
//With this function the volumes are set to enable or disable.
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::SetEnableVolume(int volume, int type)
{
  enableVolume[volume] = type;
}

//-----------------------------------------------------
//Name: GetEnableVolume
//Description: vtkVolumeTextureMapper3D supports 3 volumes.
//This function returns an array with information of which volumes
//that are enabled.
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::GetEnableVolume(int enVol[3]) 
{
  for (int i = 0; i < 3; i++)
  {
    enVol[i] = enableVolume[i];
  }
}

//-----------------------------------------------------
//Name: SetNumberOfVolumes
//Description: Set the number of volumes wanted (max=3)
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::SetNumberOfVolumes(int num)
{
  volNum = num;
}

//-----------------------------------------------------
//Name: SetBoxSize
//Description: Set the size of the box surronding the volumes
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::SetBoxSize(vtkFloatingPointType size)
{
  boxSize = (int)size;
}

//-----------------------------------------------------
//Name: GetBoxSize
//Description: Return the size of the box surrounding the volumes
//-----------------------------------------------------
int vtkVolumeTextureMapper3D::GetBoxSize()
{
  return boxSize;
}

//-----------------------------------------------------
//Name: SetOrigin
//Description: Set the size of the box surronding the volumes
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::SetOrigin(vtkFloatingPointType o_x, vtkFloatingPointType o_y, vtkFloatingPointType o_z)
{
  origin[0]=o_x;
  origin[1]=o_y;
  origin[2]=o_z;
}

//-----------------------------------------------------
//Name: GetOrigin
//Description: Return the size of the box surrounding the volumes
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::GetOrigin(vtkFloatingPointType o[3])
{
  for(int i = 0; i < 3; i++)
  {
    o[i]= origin[i];
  }
}

//-----------------------------------------------------
//Name: SetCounter
//Description: A counter 
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::SetCounter(int counter)
{
  currentCounter = counter;
}

//-----------------------------------------------------
//Name: GetCounter
//Description: Get the current counter
//-----------------------------------------------------
int vtkVolumeTextureMapper3D::GetCounter()
{
  return currentCounter;
}

//-----------------------------------------------------
//Name: SetNumberOfPlanes
//Description: Set the amount of planes to be used
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::SetNumberOfPlanes(int planes)
{
  numberOfPlanes = planes;
  for (int vol = 0; vol < 3; vol++ )
  {
    changedTable[vol] = true;
  }
}

//-----------------------------------------------------
//Name: GetNumberOfPlanes
//Description: Get the number of planes that are used
//-----------------------------------------------------
int vtkVolumeTextureMapper3D::GetNumberOfPlanes()
{
  return numberOfPlanes;
}


//-----------------------------------------------------
//Name: SetDimension
//Description: Set the dimension of a volume (must be powers of two)
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::SetDimension(int volume, int dir, int dims)
{
  dimension[volume][dir] = dims;
}

//-----------------------------------------------------
//Name: GetDimension 
//Description: Get the dimension of a volume
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::GetDimension(int volume, int dim[3])
{
  for (int i = 0; i < 3; i++)
  {
    dim[i]= dimension[volume][i];
  }    
}

//-----------------------------------------------------
//Name: GetTextureDimension
//Description: Get the texture dimension
//-----------------------------------------------------
int vtkVolumeTextureMapper3D::GetTextureDimension(int volume, int dir)
{
  return dimension[volume][dir];
}

//-o-o-o-o-o-o-o-o-o-o-
//Clip plane functions
//-o-o-o-o-o-o-o-o-o-o-

//-----------------------------------------------------
//Name: EnableClipLines
//Description: If the user wants to see clip plane lines
//then this funtion is set to enable 
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::EnableClipLines(int value)
{
  clipLines = value;
}


//-----------------------------------------------------
//Name: IsClipLinesEnable
//Description: Check if the clip plane line is enabled
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::IsClipLinesEnable(int value[1])
{
  value[0] = clipLines;
}

//-----------------------------------------------------
//Name: VolumesToClip
//Description: Set which volumes that are going to be clipped
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::VolumesToClip(int vol, int value)
{
  if (value == 1) 
  {
    volToClip[vol] = true;
  }
  else 
  {
    volToClip[vol] = false;
  }
}

//-----------------------------------------------------
//Name: GetVolumesToClip
//Description: Get information of which volumes that are 
//going to be clipped
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::GetVolumesToClip(bool vToClip[3])
{
  for(int i = 0; i < 3; i++)
  {
    vToClip[i] = volToClip[i];
  }
}

//-----------------------------------------------------
//Name: SetNumberOfClipPlanes
//Description: Set the number of clip planes to use
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::SetNumberOfClipPlanes(int planeNum)
{    
  clipPlaneNum = planeNum;
}


//-----------------------------------------------------
//Name: GetNumberOfClipPlanes
//Description: Get the number of clip planes used
//-----------------------------------------------------
int vtkVolumeTextureMapper3D::GetNumberOfClipPlanes()
{
  return clipPlaneNum;
}



//-----------------------------------------------------
//Name: InitializeClipPlanes
//Description: Initialization of clip planes
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::InitializeClipPlanes()
{
  currentType = 0;
  for (int i = 0; i < 3; i++)
  {
    volToClip[i] = false;
  }
  this->ResetClipPlanes(0);
  this->SetEnableClipPlanes(0, 1);
  for (int c = 1; c < 6; c++)
  {
    this->SetEnableClipPlanes(c, 0);
  }
  this->ChangeType(0);
}

//-----------------------------------------------------
//Name: ChangeDist
//Description: Change the clip plane distance from origin
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::ChangeDist(int plane, int dist)
{
  currentDistance[plane] = dist;
  this->UpdateClipPlaneEquation(plane);
}

//-----------------------------------------------------
//Name: ChangeSpacing
//Description: Change the spacing between two clip planes
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::ChangeSpacing(int spacing)
{
  currentSpacing = spacing;
  this->UpdateClipPlaneEquation(0);    
}

//-----------------------------------------------------
//Name: ChangeType
//Description: Change the current type of clipping
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::ChangeType(int type)
{
  currentType = type;
  this->ResetClipPlanes(currentType);    
}

//-----------------------------------------------------
//Name: GetClipNum
//Description: Get the number of clip planes required for 
//the current type of clipping 
//-----------------------------------------------------
int vtkVolumeTextureMapper3D::GetClipNum()
{
  if (currentType == 2)
  {
    return 6;
  }
  else
  {
    return currentType+1;
  }
}

//-----------------------------------------------------
//Name: ResetClipPlanes
//Description: Reset the current clip planes
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::ResetClipPlanes(int type)
{
  for (int k = 0; k < 6; k++)
  {
    for (int p = 0; p<3; p++)
    {
      currentPlane[k][p]=0;
      currentPlane[k][p]=0;
      currentPlane[k][p]=0;
    }
    if (currentType != 2)
    {
      SetEnableClipPlanes(k, 0);
    }
  }
  currentPlane[0][0]= 1.0;
  currentPlane[1][0]= -1.0;
  currentPlane[2][1]= 1.0;
  currentPlane[3][1]= -1.0;
  currentPlane[4][2]= 1.0;
  currentPlane[5][2]= -1.0;
          
  if (currentType == 0)
  {
    SetEnableClipPlanes(0, 1);    
  }
  else if (currentType == 1)
  {
    SetEnableClipPlanes(0, 1);
    SetEnableClipPlanes(1, 1);    
  }
  else 
  {
    for (int i = 0; i < 6; i++)
      {
    SetEnableClipPlanes(i, 1);
      }
  }
  for (int i = 0; i < 3; i++)
  {
    currentAngle[i] = 0;
  }
}

//-----------------------------------------------------
//Name: ChangeClipPlaneDir
//Description: Change the direction of a clip plane
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::ChangeClipPlaneDir(int plane, int dir, vtkFloatingPointType angle)
{
  double rot[3][3];
  double tempPlaneEquation[4];
  double currPlane[3];
  currPlane[0] = 1;
  currPlane[1] = 0;
  currPlane[2] = 0;
  angle = angle*3.14/180;
  currentAngle[dir]= angle;
  this->Rotate(rot, currentAngle);
  this->MultiplyMatrix(tempPlaneEquation, rot, currPlane);
  currentPlane[plane][0] = tempPlaneEquation[0];
  currentPlane[plane][1] = tempPlaneEquation[1];
  currentPlane[plane][2] = tempPlaneEquation[2];
  this->UpdateClipPlaneEquation(plane);
}




//-----------------------------------------------------
//Name: UpdateClipPlaneEquation
//Description: Updates a clip plane equation
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::UpdateClipPlaneEquation(int plane)
{
  double normDirectionVector[3];
  double planePoint[3];
  double planePointRot[3];
  double newPlane[4];
  double newPlanePoint[3];
  double normPlaneVector[3];
  
  //normal vector
  normDirectionVector[0] = currentPlane[plane][0]/sqrt(currentPlane[plane][0]*currentPlane[plane][0]
                              +currentPlane[plane][1]*currentPlane[plane][1]
                              +currentPlane[plane][2]*currentPlane[plane][2]);
  normDirectionVector[1] = currentPlane[plane][1]/sqrt(currentPlane[plane][0]*currentPlane[plane][0]
                              +currentPlane[plane][1]*currentPlane[plane][1]
                              +currentPlane[plane][2]*currentPlane[plane][2]);
  normDirectionVector[2] = currentPlane[plane][2]/sqrt(currentPlane[plane][0]*currentPlane[plane][0]
                              +currentPlane[plane][1]*currentPlane[plane][1]
                              +currentPlane[plane][2]*currentPlane[plane][2]);

  //plane point
  if (currentType == 0)
  {
    planePoint[0] = currentDistance[plane]*normDirectionVector[0];
    planePoint[1] = currentDistance[plane]*normDirectionVector[1];
    planePoint[2] = currentDistance[plane]*normDirectionVector[2];
    
    this->ComputePlaneEquation(newPlane, planePoint, normDirectionVector);
    planeEq[plane][0]=newPlane[0];
    planeEq[plane][1]=newPlane[1];
    planeEq[plane][2]=newPlane[2];
    planeEq[plane][3]=newPlane[3];
  }

  //if double
  else if (currentType == 1)
  {
    planePoint[0] = (currentDistance[plane]+currentSpacing)*normDirectionVector[0];
    planePoint[1] = (currentDistance[plane]+currentSpacing)*normDirectionVector[1];
    planePoint[2] = (currentDistance[plane]+currentSpacing)*normDirectionVector[2];
    
    this->ComputePlaneEquation(newPlane, planePoint, normDirectionVector);
    planeEq[0][0]=newPlane[0];
    planeEq[0][1]=newPlane[1];
    planeEq[0][2]=newPlane[2];
    planeEq[0][3]=newPlane[3];
    
    planePoint[0] = (currentDistance[plane]-currentSpacing)*normDirectionVector[0];
    planePoint[1] = (currentDistance[plane]-currentSpacing)*normDirectionVector[1];
    planePoint[2] = (currentDistance[plane]-currentSpacing)*normDirectionVector[2];
    
    normDirectionVector[0] = -normDirectionVector[0];
    normDirectionVector[1] = -normDirectionVector[1];
    normDirectionVector[2] = -normDirectionVector[2];
    
    this->ComputePlaneEquation(newPlane, planePoint, normDirectionVector);
    planeEq[1][0]=newPlane[0];
    planeEq[1][1]=newPlane[1];
    planeEq[1][2]=newPlane[2];
    planeEq[1][3]=newPlane[3];
  }
  else if (currentType == 2) 
  {
    planePoint[0] = (currentDistance[plane]+currentSpacing)*normDirectionVector[0];
    planePoint[1] = (currentDistance[plane]+currentSpacing)*normDirectionVector[1];
    planePoint[2] = (currentDistance[plane]+currentSpacing)*normDirectionVector[2];
    
    planePointCube[0][0] = 1;
    planePointCube[0][1] = 0;
    planePointCube[0][2] = 0;
    planePointCube[1][0] = -1;
    planePointCube[1][1] = 0;
    planePointCube[1][2] = 0;
    planePointCube[2][0] = 0;
    planePointCube[2][1] = 1;
    planePointCube[2][2] = 0;
    planePointCube[3][0] = 0; 
    planePointCube[3][1] = -1;
    planePointCube[3][2] = 0;
    planePointCube[4][0] = 0;
    planePointCube[4][1] = 0;
    planePointCube[4][2] = 1; 
    planePointCube[5][0] = 0;
    planePointCube[5][1] = 0;
    planePointCube[5][2] = -1;
    
    double rot[3][3];
    this->Rotate(rot, currentAngle);
    
    for (int j = 0; j < 6; j++)
    {
      planePoint[0] = planePointCube[j][0];
      planePoint[1] = planePointCube[j][1];
      planePoint[2] = planePointCube[j][2];
      
      this->MultiplyMatrix(planePointRot, rot, planePoint);
      
      normPlaneVector[0] = planePointRot[0]/sqrt(planePointRot[0]*planePointRot[0]
                        +planePointRot[1]*planePointRot[1]
                        +planePointRot[2]*planePointRot[2]);
      normPlaneVector[1] = planePointRot[1]/sqrt(planePointRot[0]*planePointRot[0]
                        +planePointRot[1]*planePointRot[1]
                        +planePointRot[2]*planePointRot[2]);
      normPlaneVector[2] = planePointRot[2]/sqrt(planePointRot[0]*planePointRot[0]
                         +planePointRot[1]*planePointRot[1]
                         +planePointRot[2]*planePointRot[2]);

      newPlanePoint[0] = currentDistance[plane]*normDirectionVector[0]+currentSpacing*normPlaneVector[0];
      newPlanePoint[1] = currentDistance[plane]*normDirectionVector[1]+currentSpacing*normPlaneVector[1];
      newPlanePoint[2] = currentDistance[plane]*normDirectionVector[2]+currentSpacing*normPlaneVector[2];
            
      this->ComputePlaneEquation(newPlane, newPlanePoint, normPlaneVector);
      
      planeEq[j][0]=newPlane[0];
      planeEq[j][1]=newPlane[1];
      planeEq[j][2]=newPlane[2];
      planeEq[j][3]=newPlane[3];
    }
  }
}

//-----------------------------------------------------
//Name: SetEnableClipPlanes
//Description: Set a clip plane to enable
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::SetEnableClipPlanes(int plane, int type)
{
  clipPlaneEnable[plane] = type;
}

//-----------------------------------------------------
//Name: GetEnableClipPlanes
//Description: Find out if a clip plane is enable
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::GetEnableClipPlanes(int enableClip[6])
{
  for (int i = 0; i < 6; i++)
  {
    enableClip[i] = clipPlaneEnable[i];
  }
}

//-----------------------------------------------------
//Name: GetClipPlaneEquation
//Description: Get a clip plane equation 
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::GetClipPlaneEquation(double planeEquation[4], int planeNum)
{
  for(int i = 0; i < 4; i++)
  {
    planeEquation[i] = planeEq[planeNum][i];
  }
}


//-o-o-o-o-o-o-o-o-o-o-
//Color functions
//-o-o-o-o-o-o-o-o-o-o-




//-----------------------------------------------------
//Name: GetHistValue
//-----------------------------------------------------
int vtkVolumeTextureMapper3D::GetHistValue(int volume, int index)
{
  return histArray[volume][index]*255/histMax[volume];
}


//-----------------------------------------------------
//Name: ClearTF
//Description: Clear the transfer functions
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::ClearTF()
{
  for (int i = 0; i < 3; i++)
  {
    for (int rgba = 0; rgba < 4; rgba++ )
    {
      TFnum[i][rgba] = 0; 
    }
  }
}

//-----------------------------------------------------
//Name: GetNumPoint
//Description: Get number of points in the transfer function
//for a specific volume and type
//-----------------------------------------------------
int vtkVolumeTextureMapper3D::GetNumPoint(int currentVolume, int type)
{
  return (TFnum[currentVolume][type]-1);
}

//-----------------------------------------------------
//Name: GetPoint
//Description: Get the value of a specific transfer function point
//-----------------------------------------------------
int vtkVolumeTextureMapper3D::GetPoint(int currentVolume, int type, int num, int xORy)
{
  return TFdata[type][num][currentVolume][xORy];
}

//-----------------------------------------------------
//Name: AddTFPoint
//Description: Add a transfer function point
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::AddTFPoint(int volume, int type, int point, int value)
{
  bool last = true;
  bool grey = false;
  
  if (point < 0)
  {
    point =0;
  }
  if (value <0)
  {
    value =0;
  }
  if (type == 4)
  {
    grey = true;
    type = 0;
  }
  
  if (TFnum[volume][type] == 0)
  {
    if (grey)
    {
      for (int i = 0; i < 3; i++)
      {
    TFdata[i][0][volume][0] = point; 
    TFdata[i][0][volume][1] = value;
    TFnum[volume][i]++;
      }
    }
    else
    {
      TFdata[type][0][volume][0] = point; 
      TFdata[type][0][volume][1] = value;
      TFnum[volume][type]++;
    }
    last = false;
  }
  else
  {
    for(int i = 0; i < TFnum[volume][type]; i++ )
    {
      if (TFdata[type][i][volume][0] >= point)
      {
    //sortera sà att de kommer i rätt ordning!!!
    for (int j = TFnum[volume][type]; j > i; j--) 
    {
      if (grey)
      {
        for (int m = 0; m < 3; m++)
        {
          TFdata[m][j][volume][0] = TFdata[m][j-1][volume][0]; 
          TFdata[m][j][volume][1] = TFdata[m][j-1][volume][1];
        }
      }
      else
      {
        TFdata[type][j][volume][0] = TFdata[type][j-1][volume][0]; 
        TFdata[type][j][volume][1] = TFdata[type][j-1][volume][1];                        
      }
    }
    if (grey)
    {
      for(int m = 0; m< 3; m++)
      {
        TFdata[m][i][volume][0] = point;
        TFdata[m][i][volume][1] = value;
        TFnum[volume][m]++;
      }
    }
    else
    {
      TFdata[type][i][volume][0] = point;
      TFdata[type][i][volume][1] = value;
      TFnum[volume][type]++;
    }
    last = false;
    break;
      }
    }
  }        
  if (last == true) 
  {
    if (grey)
    {
      for (int m = 0; m < 3; m++)
      {
    TFdata[m][TFnum[volume][m]][volume][0] = point;
    TFdata[m][TFnum[volume][m]][volume][1] = value;
    TFnum[volume][m]++;
      }
    }
    else
    {
      TFdata[type][TFnum[volume][type]][volume][0] = point;
      TFdata[type][TFnum[volume][type]][volume][1] = value;
      TFnum[volume][type]++;
    }
  }
  grey =false;
  changedTable[volume] = true;
}

//-----------------------------------------------------
//Name: RemoveTFPoint
//Description: Remove a transfer function point
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::RemoveTFPoint(int volume, int type, int pointPos)
{
  for (int j = pointPos; j < TFnum[volume][type]; j++) 
  {
    if (type == 4)
    {
      for (int m = 0; m < 3; m++)
      {
    TFdata[m][j][volume][0] = TFdata[m][j+1][volume][0]; 
    TFdata[m][j][volume][1] = TFdata[m][j+1][volume][1];
      }
    }
    else
    {
      TFdata[type][j][volume][0] = TFdata[type][j+1][volume][0]; 
      TFdata[type][j][volume][1] = TFdata[type][j+1][volume][1];                
    }
  }
  if (type == 4)
  {
    for (int t = 0; t < 3; t++) 
    {
      TFnum[volume][t]--;
    }
  }
  else 
  {
    TFnum[volume][type]--;
  }        
}


//-----------------------------------------------------
//Name: InitializeColors
//Description: Initialization of colors
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::IniatializeColors()
{
  for (int i = 0; i < 3; i++) 
  {
    for (int j = 0; j < 4; j++)
    {
      colorMinMax[i][j][0]= 0;
      colorMinMax[i][j][1]= 255;
    }
  }
}

//-----------------------------------------------------
//Name: SetColorMinMax
//Description: Set values to vary between
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::SetColorMinMax(int volume, int minelmax, int r, int g, int b){
  colorMinMax[volume][0][minelmax] = r;
  colorMinMax[volume][1][minelmax] = g;
  colorMinMax[volume][2][minelmax] = b;
  changedTable[volume] = true;
}
    
//-----------------------------------------------------
//Name: GetColorMinMax
//Description: Get values to vary between
//-----------------------------------------------------
int vtkVolumeTextureMapper3D::GetColorMinMax(int volume, int minelmax, int rgb)
{
  return colorMinMax[volume][rgb][minelmax];
}

//-----------------------------------------------------
//Name: UpdateColorTable
//Description: Update the color table
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::UpdateColorTable(int colorTable[256][4], int volume)
{
  vtkFloatingPointType quote = 0.0;
  vtkFloatingPointType diff1 =0.0;
  vtkFloatingPointType diff2 =0.0;
  int least1 =0;
  int least2 = 0;
  
  for(int type = 0; type < 4; type++)
  {
    for (int num = 0; num < (TFnum[volume][type]-1); num++ )
    {
      for (int i = TFdata[type][num][volume][0]; i <= TFdata[type][num+1][volume][0]; i++)
      { 
    if (TFdata[type][num+1][volume][1] == TFdata[type][num][volume][1] )
    {
      if (type == 3)
      {
        colorTable[i][type] = TFdata[type][num][volume][1]*256/numberOfPlanes/**128/numberOfPlanes*/;    
        if (colorTable[i][type]<0)
        {
          colorTable[i][type]=0;
        }
      }
      else
      {
        colorTable[i][type] = ceil((TFdata[type][num][volume][1]
                    *((colorMinMax[volume][type][1]-colorMinMax[volume][type][0]))
                    /255+colorMinMax[volume][type][0]));
        if (colorMinMax[volume][type][1] < colorMinMax[volume][type][0])
        {
          if (colorTable[i][type] < colorMinMax[volume][type][1])
          {
        colorTable[i][type] = colorMinMax[volume][type][1];
          }
          else if (colorTable[i][type] > colorMinMax[volume][type][0])
          {
        colorTable[i][type] = colorMinMax[volume][type][0];
          }
        }
        else
        {
          if (colorTable[i][type] > colorMinMax[volume][type][1])
          {
        colorTable[i][type] = colorMinMax[volume][type][1];
          }
          else if (colorTable[i][type] < colorMinMax[volume][type][0])
          {
        colorTable[i][type] = colorMinMax[volume][type][0];
          }
        }
      }
    }
    else
    {
      diff1=(vtkFloatingPointType)TFdata[type][num+1][volume][1]-(vtkFloatingPointType)TFdata[type][num][volume][1];
      diff2=(vtkFloatingPointType)TFdata[type][num+1][volume][0]-(vtkFloatingPointType)TFdata[type][num][volume][0];
      quote = sqrt(diff1*diff1)/sqrt(diff2*diff2);
      if (TFdata[type][num+1][volume][1]<TFdata[type][num][volume][1])
      {
        least2=(vtkFloatingPointType)TFdata[type][num+1][volume][1];
      }
      else
      {
        least2=(vtkFloatingPointType)TFdata[type][num][volume][1];
      }
      if (TFdata[type][num+1][volume][0]<TFdata[type][num][volume][0])
      {
        least1=(vtkFloatingPointType)TFdata[type][num+1][volume][0];
      }
      else
      {
        least1=(vtkFloatingPointType)TFdata[type][num][volume][0];
      }
      if (type == 3)
      {
        colorTable[i][type]= ceil((((i-least1)*quote+least2))*256/numberOfPlanes);
        if (colorTable[i][type]<0)
        {
          colorTable[i][type]=0;
        }
      }
      else
      {
        colorTable[i][type] = ceil(((((i-least1)*quote+least2)*
                     ((colorMinMax[volume][type][1]-colorMinMax[volume][type][0]))
                     /255+colorMinMax[volume][type][0])));
      }
    }
      }     
    }
  }
}


//-----------------------------------------------------
//Name: GetArrayPos
//Description: Get the array position for a point in the 
//transfer function
//-----------------------------------------------------
int vtkVolumeTextureMapper3D::GetArrayPos(int volume, int type, int point, int value, int boundX, int boundY) 
{
  if (type == 4)
  {
    type = 0;
  }

  for (int i = 0; i < TFnum[volume][type]; i++ )
  {
    if ((TFdata[type][i][volume][0] < (point+boundX) 
      && TFdata[type][i][volume][0] > (point-boundX)) 
      && (TFdata[type][i][volume][1] < (value+boundY) 
      && TFdata[type][i][volume][1] > (value-boundY)))
    {
      diffX = TFdata[type][i][volume][0] - point;
      diffY = TFdata[type][i][volume][1] - value;
      return i;
    }
  }
  return -1;
}


//-----------------------------------------------------
//Name: ChangeTFPoint
//Description: Change an existing transfer function point
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::ChangeTFPoint(int volume, int type, int pos, int point, int value)
{
  if (type == 4)
  {
    for (int m = 0; m < 3; m++)
    {
      TFdata[m][pos][volume][0] = point+diffX;
      TFdata[m][pos][volume][1] = value+diffY;
    }
  }
  else
  {
    TFdata[type][pos][volume][0] = point+diffX;
    TFdata[type][pos][volume][1] = value+diffY;
  }
  changedTable[volume] = true;
}

//-----------------------------------------------------
//Name: IsColorTableChanged
//Description: Is the color table changed since update?
//-----------------------------------------------------
int vtkVolumeTextureMapper3D::IsColorTableChanged(int volume)
{
  if (changedTable[volume] == true)
  {
    changedTable[volume] = false;
    return 1;
  }
  else
  {
    return 0;
  }     
}

//-o-o-o-o-o-o-o-o-o-o-
//Transform functions
//-o-o-o-o-o-o-o-o-o-o-


//-----------------------------------------------------
//Name: UpdateTransformMatrix -  Not necessary???
//Description: Updates the transformation matrix with new values
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::UpdateTransformMatrix(int volume, vtkFloatingPointType t00, vtkFloatingPointType t01, vtkFloatingPointType t02, vtkFloatingPointType t03, vtkFloatingPointType t10, vtkFloatingPointType t11, vtkFloatingPointType t12, vtkFloatingPointType t13, vtkFloatingPointType t20, vtkFloatingPointType t21, vtkFloatingPointType t22, vtkFloatingPointType t23, vtkFloatingPointType t30, vtkFloatingPointType t31, vtkFloatingPointType t32, vtkFloatingPointType t33 )
{
  currentTransformation[volume][0][0] = t00;
  currentTransformation[volume][0][1] = t01;
  currentTransformation[volume][0][2] = t02;
  currentTransformation[volume][0][3] = t03;
  currentTransformation[volume][1][0] = t10;
  currentTransformation[volume][1][1] = t11;
  currentTransformation[volume][1][2] = t12;
  currentTransformation[volume][1][3] = t13;
  currentTransformation[volume][2][0] = t20;
  currentTransformation[volume][2][1] = t21;
  currentTransformation[volume][2][2] = t22;
  currentTransformation[volume][2][3] = t23;
  currentTransformation[volume][3][0] = t30;
  currentTransformation[volume][3][1] = t31;
  currentTransformation[volume][3][2] = t32;
  currentTransformation[volume][3][3] = t33;
  
  tMatrixChanged[volume] = 1;
}

//-----------------------------------------------------
//Name: UpdateTransformMatrix with vtkMatrix4x4 input
//Description: Updates the transformation matrix with new values
//--------------------------------------------------

void vtkVolumeTextureMapper3D::UpdateTransformMatrix(int volume, vtkMatrix4x4 *transMatrix )
{
  for(int i = 0; i < 4; i++)
  {
    for(int j = 0; j < 4; j++)
    {
      currentTransformation[volume][i][j] =(vtkFloatingPointType) transMatrix->GetElement(i, j);
    }
  }

  
  tMatrixChanged[volume] = 1;
}

//-----------------------------------------------------
//Name: IsTMatrixChanged
//Description: Check if the transformation matrix is changed
//-----------------------------------------------------
int vtkVolumeTextureMapper3D::IsTMatrixChanged(int volume)
{
  if (tMatrixChanged[volume] == 1)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

//-----------------------------------------------------
//Name: GetTransformMatrix
//Description: Get the current transformation matrix
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::GetTransformMatrix(vtkFloatingPointType transfMatrix[4][4], int volume)
{

  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      transfMatrix[i][j] = currentTransformation[volume][i][j];
    }
  }
  tMatrixChanged[volume] = 0;
}

//-----------------------------------------------------
//Name: SetTransformMatrixElement
//Description: Set an element in the transformation matrix
//-----------------------------------------------------
void vtkVolumeTextureMapper3D::SetTransformMatrixElement(int volume, int row, int column, vtkFloatingPointType value)
{
  currentTransformation[volume][row][column] = value;
  tMatrixChanged[volume] = 1;
}


//-----------------------------------------------------
//Name: GetTransformMatrixElement
//Description: Get the value of an element in the 
//transformation matrix
//-----------------------------------------------------
vtkFloatingPointType vtkVolumeTextureMapper3D::GetTransformMatrixElement(int volume, int row, int column)
{
  return currentTransformation[volume][row][column];
}



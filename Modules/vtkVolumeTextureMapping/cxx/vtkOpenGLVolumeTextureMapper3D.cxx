#include "vtkOpenGLVolumeTextureMapper3D.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkGraphicsFactoryAddition.h"
#include "vtkVolume.h"
#include "stdio.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"
#include "vtkImageData.h"
#include "math.h"
#include <GL/gl.h>

#define volumeBox 3

#ifndef VTK_IMPLEMENT_MESA_CXX
vtkCxxRevisionMacro(vtkOpenGLVolumeTextureMapper3D, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkOpenGLVolumeTextureMapper3D);
#endif

#ifndef WIN32
# include <unistd.h>
#endif

#ifdef WIN32
    PFNGLTEXIMAGE3DEXTPROC glTexImage3DEXT;
    PFNGLTEXSUBIMAGE3DEXTPROC glTexSubImage3DEXT;
    PFNGLCOLORTABLEEXTPROC glColorTableEXT;
#endif

int intersectionPlanes[12][4] ={0, 1, 0, 1,
                2, 6, 0, 3, 
                7, 4, 1, 2,
                5, 3, 2, 3,
                1, 6, 0, 5,
                0, 2, 0, 4,
                4, 3, 2, 5,
                7, 5, 2, 4,
                    1, 4, 1, 5,
                6, 3, 3, 5,
                0, 7, 1, 4,
                2, 5, 3, 4};


FILE* fm = fopen("error_vtkOpenGLVolumeTextureMapper3D.txt", "w");

//-----------------------------------------------------
//Name: isExtensionSupported
//Description: Check if a specific extension is supported
//-----------------------------------------------------
int isExtensionSupported(const char *extension)
{
  const GLubyte *extensions = NULL;
  const GLubyte *start;
  GLubyte *where, *terminator;

  // Extension names should not have spaces. 
  where = (GLubyte *) strchr(extension, ' ');
  if (where || *extension == '\0')
    return 0;
  extensions = glGetString(GL_EXTENSIONS);
  start = extensions;
  for (;;) {
    where = (GLubyte *) strstr((const char *) start, extension);
    if (!where)
      break;
    terminator = where + strlen(extension);
    if (where == start || *(where - 1) == ' ')
      if (*terminator == ' ' || *terminator == '\0')
        return 1;
    start = terminator;
  }
  return 0;
}

//-----------------------------------------------------
//Name: vtkOpenGLVolumeTextureMapper3D
//Description: Constructor
//-----------------------------------------------------
vtkOpenGLVolumeTextureMapper3D::vtkOpenGLVolumeTextureMapper3D()
{
  num = 0;
  init = 0;
  zVal = 0;
  counter = 0;
  maxVolumes = 3;
}

//-----------------------------------------------------
//Name: ~vtkOpenGLVolumeTextureMapper3D
//Description: Destructor
//-----------------------------------------------------
vtkOpenGLVolumeTextureMapper3D::~vtkOpenGLVolumeTextureMapper3D()
{
}
                    

//-----------------------------------------------------
//Name: Render
//Description: Main function that control the rendering process
//-----------------------------------------------------
void vtkOpenGLVolumeTextureMapper3D::Render(vtkRenderer *ren, vtkVolume *vol)
{    
    
  int dim[3];
  counter = this->GetCounter();
  
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glShadeModel(GL_FLAT);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_COLOR);
  int size[3];





  //init == 0 - only the first time that a dataset are saved in texture memory
  if (init == 0)
  {
    using_palette = isExtensionSupported("GL_EXT_paletted_texture");        
    #ifdef WIN32
       glTexImage3DEXT = (PFNGLTEXIMAGE3DEXTPROC) wglGetProcAddress("glTexImage3DEXT");
       glTexSubImage3DEXT = (PFNGLTEXSUBIMAGE3DEXTPROC) wglGetProcAddress("glTexSubImage3DEXT");
       glColorTableEXT = (PFNGLCOLORTABLEEXTPROC) wglGetProcAddress("glColorTableEXT");
       //#define glColorTable glColorTableEXT
    #else
       #define GL_INTENSITY_EXT GL_INTENSITY
       #define GL_SHARED_TEXTURE_PALETTE_EXT GL_TEXTURE_COLOR_TABLE_SGI
       #define GL_COLOR_INDEX8_EXT GL_INTENSITY
       #define GL_TEXTURE_3D_EXT GL_TEXTURE_3D
       //#define glTexImage3DEXT glTexImage3D
       //#define glColorTableEXT glColorTable            
    #endif

    this->DefaultValues();
    boxSize = this->GetBoxSize();
    
    for (int n = 0; n < 3; n++)
    {    
      this->GetDimension(0, dim);
      textureSizeX[n]=dim[0];
      textureSizeY[n]=dim[1];
      textureSizeZ[n]=dim[2];
    }
    CreateEmptyTexture();
    InitializeVolRend();
  }
    
  //every time the volume is refreshed - i.e. resave volumes in texture memory
  if (counter == 0 )
  {
    this->GetEnableVolume(enableVol);
    
    if (init != 0)
    {
      InitializeVolRend();
      for (int n = 0; n < 3; n++)
      {    
    this->GetDimension(n, dim);
    textureSizeX[n]=dim[0];
    textureSizeY[n]=dim[1];
    textureSizeZ[n]=dim[2];            
      }    
      
      CreateEmptyTexture();    
    }    
  }

  if (counter < maxVolumes)
  {
    //testing---------------
  /*  vtkMatrix4x4       *trans = vtkMatrix4x4::New();
    trans =  vol->GetUserMatrix();
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
    transformMatrix[counter][i][j]=trans->GetElement(i, j);
      }
    }
*/

    ///---------------------

    if (enableVol[counter] == 1)
    {
      //superclass initialization
      this->vtkVolumeTextureMapper3D::InitializeRender( ren, vol );
      //generate texture to save for the specified dataset
      this->GenerateTextures( ren, vol, counter );      
      //render the polygons
      RenderQuads(ren, vol);            
    }
  }
  else
  {
    //render the polygons
    RenderQuads(ren, vol);
  }
  //update counter
  counter++;
  this->SetCounter(counter);
  
  if (init == 0)
  {
    init ++;
  }
}


//-----------------------------------------------------
//Name: CreateEmptyTexture
//Description: Save empty texture in texture memory
//-----------------------------------------------------
void vtkOpenGLVolumeTextureMapper3D::CreateEmptyTexture()
{
  if (init != 0)
  {
    glDeleteTextures(3, &tempIndex3d[0]);
  }
    
  glGenTextures(3, &tempIndex3d[0]);  
  for (int m = 0; m < 3; m++)
  {
    glBindTexture(GL_TEXTURE_3D_EXT, tempIndex3d[m]);
    glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_WRAP_R, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }
  
  for (int i = 0; i < 3; i++)
  {    
    //create an empty texture
    for (int s = 0; s < textureSizeZ[i]; s++)
    {
      for (int t = 0; t < textureSizeY[i]; t++)
      {
    for (int r = 0; r < textureSizeX[i]; r++) 
    {
      emptyImage[r][t][s][0] = (GLubyte) (0);
      emptyImage[r][t][s][1] = (GLubyte) (0);
      emptyImage[r][t][s][2] = (GLubyte) (0);
      emptyImage[r][t][s][3] = (GLubyte) (0);
    }
      }
    }
    
    //store empty textures for each dataset
    glBindTexture(GL_TEXTURE_3D_EXT, tempIndex3d[i]);
    
    //store the data in the texture memory
    if (using_palette != 1)
    {
      glTexImage3DEXT(GL_TEXTURE_3D_EXT, 
              0, 
              GL_INTENSITY_EXT, 
              textureSizeX[i], 
              textureSizeY[i], 
              textureSizeZ[i], 
              0, GL_RGBA, 
              GL_UNSIGNED_BYTE, 
              emptyImage);        

    }
    else
    {
      glTexImage3DEXT(GL_TEXTURE_3D_EXT, 
              0, 
              GL_COLOR_INDEX8_EXT, 
              textureSizeX[i], 
              textureSizeY[i], 
              textureSizeZ[i], 
              0, GL_COLOR_INDEX, 
              GL_UNSIGNED_BYTE, 
              emptyImage);    
    }
  }
}


//-----------------------------------------------------
//Name: InsertVertex
//Description: 
//-----------------------------------------------------
void vtkOpenGLVolumeTextureMapper3D::InsertVertex(float vertex[12][3], int* vertexnums, float result[4])
{
  if (result[3]== 1)
  {
    vertex[*vertexnums][0] = result[0];
    vertex[*vertexnums][1] = result[1];
    vertex[*vertexnums][2] = result[2];
    *vertexnums= *vertexnums+1;
  }
}

//-----------------------------------------------------
//Name: ClipPlane
//Description: Calculation of clip planes
//-----------------------------------------------------
void vtkOpenGLVolumeTextureMapper3D::ClipPlane(int plane, float viewUp[3])
{
  double planeEquation[4];
  int planeNum = 6;
  float vertex[12][3];
  float result[4];
  int vertexnums= 0;
  int cLines[1];

  //get the clip plane equation for a specific plane
  this->GetClipPlaneEquation(planeEquation, plane);
  float a = planeEquation[0];
  float b = planeEquation[1];
  float c = planeEquation[2];
  float d = planeEquation[3];

  //set the clip plane equation to a specific openGL clip plane
  glClipPlane((GLenum)(GL_CLIP_PLANE0+plane),planeEquation);
  this->IsClipLinesEnable(cLines);
  //calculate intersection points of the clipplane and the volumebox
  if (cLines[0] == 1)
  {  
    for (int i = 0; i < 12; i++) 
    {    
  
      this->IntersectionPoint(result, 
                  intersectionPlanes[i][0], 
                  intersectionPlanes[i][1], 
                  intersectionPlanes[i][2], 
                  intersectionPlanes[i][3], 
                  a, b, c, -d, volumeBox);
      this->InsertVertex(vertex, &vertexnums, result);
    }
    
    int vertexOrder[12];
    float normal[3];
    normal[0] = planeEquation[0];
    normal[1] = planeEquation[1];
    normal[2] = planeEquation[2];
    //sort vertexes with respect to angle around the polygon normal
    this->SortVertex(vertexOrder, vertex, &vertexnums, viewUp, normal);
    glEnable(GL_COLOR);
    glEnable(GL_LIGHTING);
    glColor3f(1.0, 1.0, 1.0);
 
    if (vertexnums>=3)
    {
      //render the clip plane line
      glBegin(GL_LINE_LOOP);
      for (int j = 0; j<vertexnums; j++)
      {
    if ((vertexOrder[j] < vertexnums)  && (vertexOrder[j] >= 0 ))
      {
        glVertex3f(vertex[vertexOrder[j]][0], vertex[vertexOrder[j]][1], vertex[vertexOrder[j]][2]);
      }
      }
      glEnd();
      glDisable(GL_COLOR);
      glDisable(GL_LIGHTING);
    }
  }
}




//-----------------------------------------------------
//Name: SortVertex
//Description: Sorts the vertexs according to angle
//-----------------------------------------------------
void vtkOpenGLVolumeTextureMapper3D::SortVertex(int vertexOrder[12], float vertex[12][3], int *vertexnum, float viewUp[3], float normal[3])
{
  float vertexAngle[12];
  float r[3];
  float rLength;
  float rAngleViewUp;
  float rAngleNx;
  float pi = 3.14;
  float rAngle;
  int antV;
  float nx[3];
  antV = 0;

  //calculate crossproduct normal x viewUp
  nx[0] =(normal[1]*viewUp[2]-normal[2]*viewUp[1]); 
  nx[1] =(normal[2]*viewUp[0]-normal[0]*viewUp[2]);
  nx[2] =(normal[0]*viewUp[1]-normal[1]*viewUp[0]);
  
  
  for (int vNum = 0; vNum < *vertexnum; vNum++)
  {
    //calculate the direction from the point to the origin
    r[0] = vertex[vNum][0];
    r[1] = vertex[vNum][1];
    r[2] = vertex[vNum][2];
      
    //normalize the r vector
    this ->NormalizeVector(r);
      
    //calculate the angle between the direction and viewUp
    rAngleViewUp = acos(r[0]*viewUp[0]+r[1]*viewUp[1]+r[2]*viewUp[2]);
    //calculate the angle between the direction and the normal x -direction
    rAngleNx = acos(r[0]*nx[0]+r[1]*nx[1]+r[2]*nx[2]);
      
    if (rAngleNx <= pi/2)
    {
      rAngle = rAngleViewUp;
    }
    else
    {
      rAngle = 2*pi-rAngleViewUp;
    }
      
    //find out where in the array vertexOrder that the vertex is going to be placed
    if (vNum == 0)
    {
      vertexAngle[vNum]=rAngle;
      vertexOrder[vNum]=0;
      antV++;
    }
    else 
    {
      for (int i = 0; i < antV; i++)
      {
    //is the current angle equal to any other angle in vertexAngle?
    if (vertexAngle[i] == rAngle)
    {
      i = antV;            
    }
    //is the current angle in vertexAngle larger than the angle?
    else if (vertexAngle[i] > rAngle)
    {
      //move the angles in vertexAngle and the vertexes in vertexOrder one step forward in the arrays
      for (int j = antV; j > i; j--)
      {
        vertexAngle[j] = vertexAngle[j-1];
        vertexOrder[j] = vertexOrder[j-1];
      }
      //insert the current angle and vertex
      vertexAngle[i]= rAngle;
      vertexOrder[i]= vNum;
      antV++;
      i = antV;
    }
    //insert the angle and vertex last in the arrays
    else if (i == (antV-1))
    {
      vertexAngle[antV] = rAngle;
      vertexOrder[antV]= vNum;
      antV++;
      i = antV;
    }
      }
    }
  }
}


//-----------------------------------------------------
//Name: RenderQuads
//Description: Render the calculated quads with correct texture
//-----------------------------------------------------
void vtkOpenGLVolumeTextureMapper3D::RenderQuads(vtkRenderer *ren, vtkVolume *vol)
{
    
  float focalPoint[3];
  int   size[3];
  float spacing[3];
  float viewAngle;
  float viewUp[3];
  bool  volumesToClip[3];
  int   thisSize[3];
  float normal[3];
  float cameraPosition[3];
  float nx[3];
  float ny[3];
  float nz[3];
  float startpos[3];

  this->GetVolumesToClip(volumesToClip);
  ren->GetActiveCamera()->GetPosition(cameraPosition);
  viewAngle = ren->GetActiveCamera()->GetViewAngle();
  ren->GetActiveCamera()->GetFocalPoint(focalPoint);
  ren->GetActiveCamera()->GetViewUp(viewUp);
  this->GetInput()->GetDimensions( size );
  this->GetInput()->GetSpacing(spacing);
    
  //is the tranformation changed?
  for (int v = 0; v < maxVolumes; v++)
  {
    if ((enableVol[v] == 1) && (this->IsTMatrixChanged(v) == 1))
    {        
      this->Transformation();            
      break;
    }
  }
    
  //clipplanes
  for (int i = 0; i < (this->GetClipNum()); i++)
  {
    ClipPlane(i, viewUp);
  }
  
  //the amount of planes used to build the volume
  int numQuads = this->GetNumberOfPlanes();
  float diagonal = sqrt(boxSize*boxSize+boxSize*boxSize+boxSize*boxSize);
  
  //translate so that the focalPoint becomes origin
  glTranslatef(focalPoint[0], focalPoint[1], focalPoint[2]);

  //calculate the normalvector
  normal[0] = (cameraPosition[0]-focalPoint[0]); 
  normal[1] = (cameraPosition[1]-focalPoint[1]);
  normal[2] = (cameraPosition[2]-focalPoint[2]);

  //normalize vector
  this->NormalizeVector(normal);
  
  for(int axis = 0; axis < 3; axis++)
  {
    startpos[axis] = -normal[axis]*diagonal/2;
  }

  for(int num = 0; num < numQuads; num++)
  {
    for (int vols = 0; vols < maxVolumes; vols++)
    {
      if (enableVol[vols] == 1)
      {
    if (vols > 0) 
    {
      num++;
    }        
    float planePoint[3];
    
    for(int axis = 0; axis < 3; axis++)
    {
      //vector in the normal z direction
      nz[axis] = normal[axis]*(diagonal/numQuads)*num;
      planePoint[axis]= startpos[axis]+nz[axis];
    }
    
    float vertex[12][3];
    float result[4];
    int   vertexnums = 0;
    float a = normal[0];
    float b = normal[1];
    float c = normal[2];
    float d = normal[0]*planePoint[0]
             +normal[1]*planePoint[1]
             +normal[2]*planePoint[2];
    
    for (int i = 0; i < 12; i++) 
    {
      this->IntersectionPoint(result, 
                  intersectionPlanes[i][0], 
                  intersectionPlanes[i][1], 
                  intersectionPlanes[i][2], 
                  intersectionPlanes[i][3], 
                  a, b, c, d, vols);
      this->InsertVertex(vertex, &vertexnums, result);
    }
    
    //sort the vertexes
    int vertexOrder[12];
    this->SortVertex(vertexOrder, vertex, &vertexnums, viewUp, normal);
    int planeNum = 1;
    
    if (vols == 0)
    {
      ChangeColorTable(vols, colorTable0);
    }
    else if(vols == 1)
    {
      ChangeColorTable(vols, colorTable1);
    }
    else 
    {
      ChangeColorTable(vols, colorTable2);
    }
    
    if (volumesToClip[vols] == true)
    {
      this->GetEnableClipPlanes(enableClip);
      for (int i = 0; i < 6; i++)
      {
        if (enableClip[i] == 1)
        {
          glEnable((GLenum)(GL_CLIP_PLANE0+i));
        }
        else 
        {
          glDisable((GLenum)(GL_CLIP_PLANE0+i));
        }
      }
    }
    
    if (using_palette != 1)
    {
       glEnable(GL_TEXTURE_COLOR_TABLE_SGI);
    }
    else
    {
      glEnable(GL_SHARED_TEXTURE_PALETTE_EXT);
    }
    glEnable(GL_TEXTURE_3D_EXT);                
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);    
    glBindTexture(GL_TEXTURE_3D_EXT, tempIndex3d[vols]);
    
    if (vertexnums >= 3)
    {
      glBegin(GL_POLYGON);
      for (int v = 0; v < vertexnums; v++)
      {        
        if ((vertexOrder[v] < vertexnums) && (vertexOrder[v] >= 0)) 
        {
          float bx = vertex[vertexOrder[v]][0] + boxSize/2;
          float by = vertex[vertexOrder[v]][1] + boxSize/2;
          float bz = vertex[vertexOrder[v]][2] + boxSize/2;
             
          //transform texture coordinates with the inverse of the transformation matrix
          float tx = (transformInvMatrix[vols][0][0]*bx+
              transformInvMatrix[vols][0][1]*by+
              transformInvMatrix[vols][0][2]*bz+
              transformInvMatrix[vols][0][3]);    
          float ty = (transformInvMatrix[vols][1][0]*bx+
              transformInvMatrix[vols][1][1]*by+
              transformInvMatrix[vols][1][2]*bz+
              transformInvMatrix[vols][1][3]);
          float tz = (transformInvMatrix[vols][2][0]*bx+
              transformInvMatrix[vols][2][1]*by+
              transformInvMatrix[vols][2][2]*bz+
              transformInvMatrix[vols][2][3]);

          /*tx = tx*textureSizeX[vols]/texSize[vols][0]*1/textureSizeX[vols];
          ty = ty*textureSizeY[vols]/texSize[vols][1]*1/textureSizeY[vols];
          tz = tz*1/textureSizeZ[vols];*/

          tx = tx/texSize[vols][0];
          ty = ty/texSize[vols][1];
          tz = tz/textureSizeZ[vols];
                         
          //set texture coordinate
          glTexCoord3f(tx, ty, tz);
          
          float vx = vertex[vertexOrder[v]][0];
          float vy = vertex[vertexOrder[v]][1];
          float vz = vertex[vertexOrder[v]][2];
          
          //set vertex coordinates
          glVertex3f(vx, vy, vz);    
        }
      }  
      glEnd();
    }
      }
      
      for (int i = 0; i < 6; i++)
      {
    glDisable((GLenum)(GL_CLIP_PLANE0+i));                        
      }
    }
  }
  glTranslatef(-focalPoint[0], -focalPoint[1], -focalPoint[2]);
  zVal = 0;
  if (using_palette != 1)
  {
    glDisable(GL_TEXTURE_COLOR_TABLE_SGI);
  }
  else
  {
    glDisable(GL_SHARED_TEXTURE_PALETTE_EXT);
  }
  
  glDisable(GL_TEXTURE_3D_EXT);
  glDisable(GL_BLEND);
}

//-----------------------------------------------------
//Name: ChangeColorTable
//Description: If the transfer function is changed then
//a new color table is needed.
//-----------------------------------------------------
void vtkOpenGLVolumeTextureMapper3D::ChangeColorTable(int volume, int colorTable[256][4])
{    
  //update colortable if the transfer function is changed
  if (IsColorTableChanged(volume))
  {
    this->UpdateColorTable(colorTable, volume);             
  }
  
  GLubyte newColorTable[256][4];
  for (int i = 0; i < 256; i++)
  {
    for (int j=0; j<4; j++)
    {
      newColorTable[i][j] = (GLubyte) colorTable[i][j];
    }
  }
  //set new color tables
  if (using_palette != 1)
  {
    glColorTable(GL_TEXTURE_COLOR_TABLE_SGI,
         GL_RGBA ,
         256 ,
         GL_RGBA ,
         GL_UNSIGNED_BYTE ,
         &newColorTable);
  }
  else
  {
    glColorTable(GL_SHARED_TEXTURE_PALETTE_EXT,
         GL_RGBA ,
         256 ,
         GL_RGBA ,
         GL_UNSIGNED_BYTE ,
         &newColorTable);
  }
}

//-----------------------------------------------------
//Name: CalcMaxMinValue
//Description: Find out which values to resize between
//-----------------------------------------------------
void vtkOpenGLVolumeTextureMapper3D::CalcMaxMinValue()
{
  for (int volume = 0; volume < maxVolumes; volume++)
  {
    if (enableVol[volume] == 1)
    {
      float cornerInDatasetBox[8][3] = {1, 1, 1,
                    1, texSize[volume][1], 1,
                    1, 1, texSize[volume][2],
                    texSize[volume][0], texSize[volume][1], texSize[volume][2],
                    texSize[volume][0], texSize[volume][1], 1,
                    texSize[volume][0], 1, texSize[volume][2],
                    1, texSize[volume][1], texSize[volume][2], 
                    texSize[volume][0], 1, 1};
      
      for (int i = 0; i < 8; i++) 
      {
    for (int j = 0; j < 3; j++)
    {
      volumeCornerPoint[volume][i][j]= transformMatrix[volume][j][0]*cornerInDatasetBox[i][0]+
                                       transformMatrix[volume][j][1]*cornerInDatasetBox[i][1]+
                                           transformMatrix[volume][j][2]*cornerInDatasetBox[i][2]+
                                   transformMatrix[volume][j][3];
    }
      }        
    
    }
  }
}

//-----------------------------------------------------
//Name: Transformation
//Description: Transform the volume according to a 
//transformation matrix.
//-----------------------------------------------------
void vtkOpenGLVolumeTextureMapper3D::Transformation() 
{
  float tMatrix[4][4];
  vtkMatrix4x4       *tfmatrix = vtkMatrix4x4::New();
  vtkMatrix4x4       *tfInvmatrix = vtkMatrix4x4::New();
    
  for (int v = 0; v < maxVolumes; v++)
  {
    if (enableVol[v] == 1)
    {
      this->GetTransformMatrix(tMatrix, v);
      for (int i = 0; i < 4; i++)
      {
    for (int j = 0; j < 4; j++)
    {
      transformMatrix[v][i][j] = tMatrix[i][j];
    }
      }
    }
  }
  
  this->CalcMaxMinValue();
 
  for (int volume = 0; volume < maxVolumes; volume++)
  {
    if(enableVol[volume] == 1)
    {
      for (int m = 0; m < 6; m++)
      {
    CalculatePlaneEquation(/*a1*/volumeCornerPoint[volume][cornersInDatasetPlane[m][0]][0],
                   /*a2*/volumeCornerPoint[volume][cornersInDatasetPlane[m][0]][1],
                   /*a3*/volumeCornerPoint[volume][cornersInDatasetPlane[m][0]][2],
                   /*b1*/volumeCornerPoint[volume][cornersInDatasetPlane[m][1]][0],
                   /*b2*/volumeCornerPoint[volume][cornersInDatasetPlane[m][1]][1],
                   /*b3*/volumeCornerPoint[volume][cornersInDatasetPlane[m][1]][2],
                   /*c1*/volumeCornerPoint[volume][cornersInDatasetPlane[m][2]][0],
                   /*c2*/volumeCornerPoint[volume][cornersInDatasetPlane[m][2]][1],
                   /*c3*/volumeCornerPoint[volume][cornersInDatasetPlane[m][2]][2],
                   volume ,m);
      }
            
      for (int n = 0; n < 4; n++)
      {
    for (int u = 0; u < 4; u++)
    {
      tfmatrix->SetElement(n,u, transformMatrix[volume][n][u]);
    }
      }
      
      tfmatrix->Invert(tfmatrix, tfInvmatrix);
      
      for (int f = 0; f < 4; f++)
      {
    for (int e = 0; e < 4; e++)
    {
      transformInvMatrix[volume][f][e]= tfInvmatrix->GetElement(f,e);
    }
      }
    }
  }
  tfmatrix->Delete();
  tfInvmatrix->Delete();
}


//-----------------------------------------------------
//Name: CreateSubImages
//Description: Store sub images in texture memory
//-----------------------------------------------------
void vtkOpenGLVolumeTextureMapper3D::CreateSubImages( unsigned char* texture,                                                      int size[3], float spacing[3])
{
  if (counter  < maxVolumes)
  {
    if (enableVol[counter] == 1)
    {
      for(int i = 0; i < 3; i++)
      {
    volSize[counter][i]= spacing[i];
    texSize[counter][i]= size[i];                
      }
      
      this->SetTransformMatrixElement(counter, 0, 0, spacing[0]);
      this->SetTransformMatrixElement(counter, 1, 1, spacing[1]);
      this->SetTransformMatrixElement(counter, 2, 2, spacing[2]);
      //this->Transformation(counter, size, spacing);
    }
    int texPtr = 0;    
    
    if (using_palette != 1)
    {
      pix =  new unsigned char[4*textureSizeX[counter]*textureSizeY[counter]];
      int pixPtr = 0;

      for (int y = 0; y < textureSizeY[counter]; y++)
      {
    for (int x = 0; x < textureSizeX[counter]; x++) 
    {
      pix[pixPtr]=texture[texPtr];
      pixPtr++;
      pix[pixPtr]=texture[texPtr];
      pixPtr++;
      pix[pixPtr]=texture[texPtr];
      pixPtr++;
      pix[pixPtr]=texture[texPtr];
      pixPtr++;
      texPtr++;
    }
      }
      texPtr = 0;
      
      //store the subimage
      glBindTexture(GL_TEXTURE_3D_EXT, tempIndex3d[counter]);    
      glTexSubImage3DEXT( GL_TEXTURE_3D_EXT,    
              0,                    //level of detail
              0,                //xoffset 
              0,                //yoffset 
              zVal,                //zoffset
              //0,
              textureSizeX[counter],
              textureSizeY[counter], 
              1,
              GL_RGBA,
              GL_UNSIGNED_BYTE, 
              pix);                 //data
    }
    else
    {
      //store the subimage
      glBindTexture(GL_TEXTURE_3D_EXT, tempIndex3d[counter]);    
      glTexSubImage3DEXT( GL_TEXTURE_3D_EXT, //GL_TEXTURE_3D_EXT
              0,            //level of detail
              0,            //xoffset 
              0,            //yoffset 
              zVal,            //zoffset
              textureSizeX[counter],
              textureSizeY[counter], 
              1,
              GL_COLOR_INDEX,
              GL_UNSIGNED_BYTE, 
              texture);              //data    
    }
    
    if (size[3]  == zVal)
    {
      this->Transformation();
    }
    zVal++;
  }    
  if (using_palette != 1)
  {
    delete [] pix;    
  }
}

//-----------------------------------------------------
//Name: CalculatePlaneEquation
//Description: Calculation of plane equations
//-----------------------------------------------------
void vtkOpenGLVolumeTextureMapper3D::CalculatePlaneEquation(float a1,float a2, float a3, float b1, float b2, float b3, float c1, float c2, float c3, int volume,int num)
{
  //crossproduct to get the polygon normal
  float A = (b2-a2)*(c3-a3)-(c2-a2)*(b3-a3);
  float B = ((b3-a3)*(c1-a1)-(c3-a3)*(b1-a1));
  float C = (b1-a1)*(c2-a2)-(c1-a1)*(b2-a2);

  if (volume != volumeBox)
  {
    //translate

 a1 = a1  - boxSize/2;
a2 =a2  - boxSize/2;
    a3 =a3 - boxSize/2;
  }

  //plane equation
  volumePlaneEquation[volume][num][0] = A/(float)sqrt(A*A+B*B+C*C);
  volumePlaneEquation[volume][num][1] = B/(float)sqrt(A*A+B*B+C*C);
  volumePlaneEquation[volume][num][2] = C/(float)sqrt(A*A+B*B+C*C);
  volumePlaneEquation[volume][num][3] = (a1*volumePlaneEquation[volume][num][0]
                    +a2*volumePlaneEquation[volume][num][1]
                        +a3*volumePlaneEquation[volume][num][2]);
}

//-----------------------------------------------------
//Name: NormalizeVector
//Description: Vector normalization
//-----------------------------------------------------
void vtkOpenGLVolumeTextureMapper3D::NormalizeVector(float vect[3])
{
  float vectDiv = sqrt(vect[0]*vect[0]+vect[1]*vect[1]+vect[2]*vect[2]);
  vect[0]= vect[0]/vectDiv;
  vect[1]= vect[1]/vectDiv;
  vect[2]= vect[2]/vectDiv;
}



//-----------------------------------------------------
//Name: IntersectionPoint
//Description: Find out where the planes intersect with 
//the volume box
//-----------------------------------------------------
void vtkOpenGLVolumeTextureMapper3D::IntersectionPoint(float result[4], int corner1, int corner2, int plane1, int plane2, float a, float b, float c, float d, int vols)
{
  float intersectionDisplacement[3];
  float intersectionMatrix[3][3];
  float intersectionInvMatrix[3][3];   
  float determinant ;
  float r1 = 0;
  float r2 = 0;
  float r3 = 0;
  float t1 = 0;
  float t2 = 0;
  float t3 = 0;
  float x = 0.0;
  float y = 0.0;
  float z = 0.0;
  float margin = 0.1;
  result[3] = 0;

  intersectionMatrix[0][0]= volumePlaneEquation[vols][plane1][0];
  intersectionMatrix[0][1]= volumePlaneEquation[vols][plane1][1];
  intersectionMatrix[0][2]= volumePlaneEquation[vols][plane1][2];
  
  intersectionMatrix[1][0]= volumePlaneEquation[vols][plane2][0];
  intersectionMatrix[1][1]= volumePlaneEquation[vols][plane2][1];
  intersectionMatrix[1][2]= volumePlaneEquation[vols][plane2][2];
  
  intersectionMatrix[2][0] = a;
  intersectionMatrix[2][1] = b;
  intersectionMatrix[2][2] = c;
  
  intersectionDisplacement[0] = volumePlaneEquation[vols][plane1][3];
  intersectionDisplacement[1] = volumePlaneEquation[vols][plane2][3];
  intersectionDisplacement[2] = d;
  
  determinant = intersectionMatrix[0][0]*intersectionMatrix[1][1]*intersectionMatrix[2][2]
               +intersectionMatrix[0][1]*intersectionMatrix[1][2]*intersectionMatrix[2][0]
               +intersectionMatrix[0][2]*intersectionMatrix[1][0]*intersectionMatrix[2][1]
               -intersectionMatrix[2][0]*intersectionMatrix[1][1]*intersectionMatrix[0][2]
               -intersectionMatrix[1][0]*intersectionMatrix[0][1]*intersectionMatrix[2][2]
               -intersectionMatrix[2][1]*intersectionMatrix[1][2]*intersectionMatrix[0][0];
  
  if (determinant != 0)
  {
    intersectionInvMatrix[0][0] = (intersectionMatrix[1][1]*intersectionMatrix[2][2]
                  -intersectionMatrix[2][1]*intersectionMatrix[1][2])/determinant;
    intersectionInvMatrix[1][0] = -(intersectionMatrix[1][0]*intersectionMatrix[2][2]
                  -intersectionMatrix[2][0]*intersectionMatrix[1][2])/determinant;
    intersectionInvMatrix[2][0] = (intersectionMatrix[1][0]*intersectionMatrix[2][1]
                  -intersectionMatrix[2][0]*intersectionMatrix[1][1])/determinant;

    intersectionInvMatrix[0][1] = -(intersectionMatrix[0][1]*intersectionMatrix[2][2]
                  -intersectionMatrix[2][1]*intersectionMatrix[0][2])/determinant;
    intersectionInvMatrix[1][1] = (intersectionMatrix[0][0]*intersectionMatrix[2][2]
                  -intersectionMatrix[2][0]*intersectionMatrix[0][2])/determinant;
    intersectionInvMatrix[2][1] = -(intersectionMatrix[0][0]*intersectionMatrix[2][1]
                  -intersectionMatrix[2][0]*intersectionMatrix[0][1])/determinant;

    intersectionInvMatrix[0][2] = (intersectionMatrix[0][1]*intersectionMatrix[1][2]
                  -intersectionMatrix[1][1]*intersectionMatrix[0][2])/determinant;
    intersectionInvMatrix[1][2] = -(intersectionMatrix[0][0]*intersectionMatrix[1][2]
                  -intersectionMatrix[1][0]*intersectionMatrix[0][2])/determinant;
    intersectionInvMatrix[2][2] = (intersectionMatrix[0][0]*intersectionMatrix[1][1]
                  -intersectionMatrix[1][0]*intersectionMatrix[0][1])/determinant;

    x = intersectionInvMatrix[0][0]*intersectionDisplacement[0]
       +intersectionInvMatrix[0][1]*intersectionDisplacement[1]
       +intersectionInvMatrix[0][2]*intersectionDisplacement[2];
    y = intersectionInvMatrix[1][0]*intersectionDisplacement[0]
       +intersectionInvMatrix[1][1]*intersectionDisplacement[1]
       +intersectionInvMatrix[1][2]*intersectionDisplacement[2];
    z = intersectionInvMatrix[2][0]*intersectionDisplacement[0]
       +intersectionInvMatrix[2][1]*intersectionDisplacement[1]
       +intersectionInvMatrix[2][2]*intersectionDisplacement[2];

    float xc1 = (volumeCornerPoint[vols][corner1][0]);
    float xc2 = (volumeCornerPoint[vols][corner2][0]);
    float yc1 = (volumeCornerPoint[vols][corner1][1]);
    float yc2 = (volumeCornerPoint[vols][corner2][1]);
    float zc1 = (volumeCornerPoint[vols][corner1][2]);
    float zc2 = (volumeCornerPoint[vols][corner2][2]);

    //translate the coordinates to the correct position
    if (vols != volumeBox)
    {
      xc1 = xc1 - boxSize/2;
      xc2 = xc2  - boxSize/2;
      yc1 = yc1  - boxSize/2; 
      yc2 = yc2 - boxSize/2;
      zc1 = zc1 - boxSize/2;
      zc2 = zc2 - boxSize/2;
    }

    //check if the found intersection point lies within the corner points on the volume box
    if ((((x >= (xc1-margin)) && (x <= (xc2+margin))) ||
    ((x <= (xc1+margin)) && (x >= (xc2-margin))))
    &&
    (((y >= (yc1-margin)) && (y <= (yc2+margin))) ||
    ((y <= (yc1+margin)) && (y >= (yc2-margin))))
    &&
    (((z >= (zc1-margin)) && (z <= (zc2+margin))) ||
    ((z <= (zc1+margin)) && (z >= (zc2-margin)))))
      
    {
      r1 =  xc2-xc1;
      r2 =  yc2-yc1;
      r3 =  zc2-zc1;
      t1 =  x-xc1;
      t2 =  y-yc1;
      t3 =  z-zc1;
      r1 =  r1/sqrt(r1*r1+r2*r2+r3*r3);
      r2 =  r2/sqrt(r1*r1+r2*r2+r3*r3);
      r3 =  r3/sqrt(r1*r1+r2*r2+r3*r3);
      t1 =  t1/sqrt(t1*t1+t2*t2+t3*t3);
      t2 =  t2/sqrt(t1*t1+t2*t2+t3*t3);
      t3 =  t3/sqrt(t1*t1+t2*t2+t3*t3);
      
      if ((sqrt((r1 - t1)*(r1 - t1)) < 1.0) && (sqrt((r2 - t2)*(r2 - t2)) < 1.0) && (sqrt((r3 - t3)*(r3 - t3)) < 1.0))
      {
    result[0] = x;
    result[1] = y;
    result[2] = z;
    result[3] = 1;
      }
    }    
  }
}


//-----------------------------------------------------
//Name: InitalizeVolRend
//Description: Initialization of the volume renderer
//-----------------------------------------------------
void vtkOpenGLVolumeTextureMapper3D::InitializeVolRend()
{
  if (init == 0)
  {
    for (int t = 0; t < maxVolumes; t++)
    {
      transformMatrix[t][0][0] = 1;
      transformMatrix[t][0][1] = 0;
      transformMatrix[t][0][2] = 0;
      transformMatrix[t][0][3] = 0;
      
      transformMatrix[t][1][0] = 0;
      transformMatrix[t][1][1] = 1;
      transformMatrix[t][1][2] = 0;
      transformMatrix[t][1][3] = 0;

      transformMatrix[t][2][0] = 0;
      transformMatrix[t][2][1] = 0;
      transformMatrix[t][2][2] = 1;
      transformMatrix[t][2][3] = 0;

      transformMatrix[t][3][0] = 0;
      transformMatrix[t][3][1] = 0;
      transformMatrix[t][3][2] = 0;
      transformMatrix[t][3][3] = 1;
    }
    
    cornersInDatasetPlane[0][0] = 0;
    cornersInDatasetPlane[0][1] = 1;
    cornersInDatasetPlane[0][2] = 2;
    
    cornersInDatasetPlane[1][0] = 1;
    cornersInDatasetPlane[1][1] = 4;
    cornersInDatasetPlane[1][2] = 0;

    cornersInDatasetPlane[2][0] = 3;
    cornersInDatasetPlane[2][1] = 4;
    cornersInDatasetPlane[2][2] = 5;

    cornersInDatasetPlane[3][0] = 3;
    cornersInDatasetPlane[3][1] = 5;
    cornersInDatasetPlane[3][2] = 6;

    cornersInDatasetPlane[4][0] = 0;
    cornersInDatasetPlane[4][1] = 2;
    cornersInDatasetPlane[4][2] = 7;

    cornersInDatasetPlane[5][0] = 4;
    cornersInDatasetPlane[5][1] = 1;
    cornersInDatasetPlane[5][2] = 3;
  }
  
  boxSize = this->GetBoxSize();
  volumeCornerPoint[volumeBox][0][0] = -boxSize/2;
  volumeCornerPoint[volumeBox][0][1] = -boxSize/2;
  volumeCornerPoint[volumeBox][0][2] = -boxSize/2;
  
  volumeCornerPoint[volumeBox][1][0] = -boxSize/2;
  volumeCornerPoint[volumeBox][1][1] = boxSize/2;
  volumeCornerPoint[volumeBox][1][2] = -boxSize/2;
  
  volumeCornerPoint[volumeBox][2][0] = -boxSize/2;
  volumeCornerPoint[volumeBox][2][1] = -boxSize/2;
  volumeCornerPoint[volumeBox][2][2] = boxSize/2;

  volumeCornerPoint[volumeBox][3][0] = boxSize/2;
  volumeCornerPoint[volumeBox][3][1] = boxSize/2;
  volumeCornerPoint[volumeBox][3][2] = boxSize/2;

  volumeCornerPoint[volumeBox][4][0] = boxSize/2;
  volumeCornerPoint[volumeBox][4][1] = boxSize/2;
  volumeCornerPoint[volumeBox][4][2] = -boxSize/2;

  volumeCornerPoint[volumeBox][5][0] = boxSize/2;
  volumeCornerPoint[volumeBox][5][1] = -boxSize/2;
  volumeCornerPoint[volumeBox][5][2] = boxSize/2;
  
  volumeCornerPoint[volumeBox][6][0] = -boxSize/2;
  volumeCornerPoint[volumeBox][6][1] = boxSize/2;
  volumeCornerPoint[volumeBox][6][2] = boxSize/2;

  volumeCornerPoint[volumeBox][7][0] = boxSize/2;
  volumeCornerPoint[volumeBox][7][1] = -boxSize/2;
  volumeCornerPoint[volumeBox][7][2] = -boxSize/2;

  for (int m = 0; m < 6; m++)
  {
    CalculatePlaneEquation(/*a1*/volumeCornerPoint[volumeBox][cornersInDatasetPlane[m][0]][0],
               /*a2*/volumeCornerPoint[volumeBox][cornersInDatasetPlane[m][0]][1],
               /*a3*/volumeCornerPoint[volumeBox][cornersInDatasetPlane[m][0]][2],
               /*b1*/volumeCornerPoint[volumeBox][cornersInDatasetPlane[m][1]][0],
               /*b2*/volumeCornerPoint[volumeBox][cornersInDatasetPlane[m][1]][1],
               /*b3*/volumeCornerPoint[volumeBox][cornersInDatasetPlane[m][1]][2],
               /*c1*/volumeCornerPoint[volumeBox][cornersInDatasetPlane[m][2]][0],
               /*c2*/volumeCornerPoint[volumeBox][cornersInDatasetPlane[m][2]][1],
               /*c3*/volumeCornerPoint[volumeBox][cornersInDatasetPlane[m][2]][2],
               volumeBox ,m);
  }
}




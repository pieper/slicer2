#ifndef __vtkOpenGLVolumeTextureMapper3D_h
#define __vtkOpenGLVolumeTextureMapper3D_h

#include "vtkVolumeTextureMapper3D.h"
#include <GL/gl.h>

//------------------
#include <math.h>
#include <stdlib.h>
#define GL_GLEXT_LEGACY
#include <stdio.h>

//WINDOWS
#ifndef WIN32
#  include <unistd.h>
#else
#  include <io.h>
#  include <GL/glext.h>
#endif


#if !defined(HAVE_GLCOLORTABLE) && defined(HAVE_GLCOLORTABLEEXT)
#  define glColorTable glColorTableEXT
#else
#  if !defined(HAVE_GLCOLORTABLE) && defined(HAVE_GLCOLORTABLESGI)
#     define glColorTable glColorTableSGI
#  endif
#endif

#if !defined(GL_TEXTURE_COLOR_TABLE) && defined(GL_TEXTURE_COLOR_TABLE_EXT)
#  define GL_TEXTURE_COLOR_TABLE GL_TEXTURE_COLOR_TABLE_EXT
#else
#  if !defined(GL_TEXTURE_COLOR_TABLE) && defined(GL_TEXTURE_COLOR_TABLE_SGI)
#    define GL_TEXTURE_COLOR_TABLE GL_TEXTURE_COLOR_TABLE_SGI
#  endif
#endif

#include <vtkVolumeTextureMappingConfigure.h>


class VTK_VOLUMETEXTUREMAPPING_EXPORT vtkOpenGLVolumeTextureMapper3D : public vtkVolumeTextureMapper3D
{
public:
 vtkTypeRevisionMacro(vtkOpenGLVolumeTextureMapper3D,vtkVolumeTextureMapper3D);
 static vtkOpenGLVolumeTextureMapper3D *New();
  
 virtual void Render(vtkRenderer *ren, vtkVolume *vol);
 void CreateSubImages(unsigned char* texture, int size[2], float spacing[3]);
 void RenderQuads(vtkRenderer *ren, vtkVolume *vol);
 void CreateEmptyTexture(void);
 void ClipPlane(int plane, float viewUp[3]);
 void InitializeVolRend();
 void NormalizeVector(float vect[3]);
 void ChangeColorTable(int volume, int colorTable[256][4]);
 void CalculatePlaneEquation(float a1, float a2, float a3, float b1, float b2,float  b3, float c1, float c2, float c3, int volume,int num);
 void IntersectionPoint(float result[4], int corner1, int corner2, int plane1, int plane2, float a, float b, float c, float d, int vols);
 void SortVertex(int vertexOrder[12], float vertex[12][3], int* vertexnum, float viewUp[3], float normal[3]);
 void Transformation(); 
 void CalcMaxMinValue();
 void InsertVertex(float vertex[12][3], int *vertexnums, float result[4]);

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//variables
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

int colorTable0[256][4];
int colorTable1[256][4];
int colorTable2[256][4];
int num;
float volumeCornerPoint[4][8][3];
 unsigned char* pix;
int init;
int zVal;
int counter;
int maxVolumes ;
int    using_palette;
float volSize[3][3];
int texSize[3][3];
int textureSizeX[3];
int textureSizeY[3];
int textureSizeZ[3];
int boxSize;
float transformMatrix[3][4][4];
float transformInvMatrix[3][4][4];
float volumePlaneEquation[4][6][4]; 
int cornersInDatasetPlane[6][3];
float maxminCoord[2];
int enableClip[6];
int enableVol[3];
int currentTable[256][4];
GLubyte emptyImage[256][256][128][4];
GLubyte colorImage[256][256][1][4];
GLuint tempIndex3d[3];
GLubyte pixels[256][256][1][1];    
GLubyte pixels4[256][256][1][4];    
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

protected:
  vtkOpenGLVolumeTextureMapper3D();
  ~vtkOpenGLVolumeTextureMapper3D();


private:
  vtkOpenGLVolumeTextureMapper3D(const vtkOpenGLVolumeTextureMapper3D&);  // Not implemented.
  void operator=(const vtkOpenGLVolumeTextureMapper3D&);  // Not implemented.
};


#endif



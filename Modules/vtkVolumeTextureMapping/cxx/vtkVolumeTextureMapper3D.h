

#ifndef __vtkVolumeTextureMapper3D_h
#define __vtkVolumeTextureMapper3D_h

#include "vtkVolumeTextureMapper.h"
//#include <GL/gl.h>
#include <vtkVolumeTextureMappingConfigure.h>
#include "vtkMatrix4x4.h"
class VTK_VOLUMETEXTUREMAPPING_EXPORT vtkVolumeTextureMapper3D : public vtkVolumeTextureMapper
{
public:
    vtkTypeRevisionMacro(vtkVolumeTextureMapper3D,vtkVolumeTextureMapper);
  //vtkTypeMacro(vtkVolumeTextureMapper3D,vtkVolumeTextureMapper);
  // void PrintSelf( ostream& os, vtkIndent indent );

  static vtkVolumeTextureMapper3D *New();

  

void SetNumberOfVolumes( int num );
void AddTFPoint(int volume, int type, int point, int value);
void ClearTF();
int IsColorTableChanged(int volume);
int GetArrayPos(int volume, int type, int point, int value, int boundX, int boundY) ;
void ChangeTFPoint(int volume, int type, int pos, int point, int value);
void SetNumberOfClipPlanes(int planeNum);
int GetNumberOfClipPlanes();
void ChangeDist(int plane, int dist);
void ChangeClipPlaneDir(int plane, int dir, float angle);
void GetClipPlaneEquation(double planeEquation[4], int planeNum);
void MultiplyMatrix(double result[3], double rotMatrix[3][3], double original[3]);
void UpdateClipPlaneEquation(int plane);
void InitializeClipPlanes();
void SetCounter(int counter);
int GetCounter();
void SetNumberOfPlanes(int planes);
int GetNumberOfPlanes();
void VolumesToClip(int vol, int value);
void GetVolumesToClip(bool vToClip[3]);
void IniatializeColors();
void SetColorMinMax(int volume, int minelmax, int r, int g, int b);
int GetNumPoint(int currentVolume, int type);
int GetPoint(int currentVolume, int type, int num, int xORy);
int GetColorMinMax(int volume, int minelmax, int rgb);
void RemoveTFPoint(int volume, int type, int pointPos);

void SetEnableClipPlanes(int plane, int type);
void GetEnableClipPlanes(int enableClip[6]);
void ChangeType(int type);
void ResetClipPlanes(int type);
void ChangeSpacing(int spacing);
void ComputePlaneEquation(double plane[4], double point[4], double normal[3]);
void Rotate(double rot[3][3], double angle[3]);
void EnableClipLines(int value);
void IsClipLinesEnable(int value[1]);
void UpdateTransformMatrix(int volume, float t00, float t01, float t02, float t03, float t10, float t11, float t12, float t13, float t20, float t21, float t22, float t23, float t30, float t31, float t32, float t33 );
 
void GetTransformMatrix(float transfMatrix[4][4], int volume);
int IsTMatrixChanged(int volume);
float GetTransformMatrixElement(int volume, int row, int column);
void SetEnableVolume(int volume, int type);
void GetEnableVolume(int enVol[3]) ;
int GetClipNum();
 void SetTransformMatrixElement(int volume, int row, int column, float value);
 void SetDimension(int volume, int dir, int dims);
void GetDimension(int volume, int dim[3]);
int GetTextureDimension(int volume, int dir);
void DefaultValues();
void SetBoxSize(float size);
int GetBoxSize();
int GetHistValue(int volume, int index);
void UpdateTransformMatrix(int volume, vtkMatrix4x4 *transMatrix );
 void SetOrigin(float o_x, float o_y, float o_z);
void  GetOrigin(float o[3]);
//BTX

  // Description:
  // WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
  // DO NOT USE THIS METHOD OUTSIDE OF THE RENDERING PROCESS
  // Render the volume
  virtual void Render(vtkRenderer *, vtkVolume *) {};

  virtual void CreateSubImages(unsigned char* vtkNotUsed(texture),
                            int vtkNotUsed(size)[3],
                            float vtkNotUsed(spacing)[3]) {};

  

  // Description:
  // Made public only for access from the templated method. Not a vtkGetMacro
  // to avoid the PrintSelf defect.
  int GetInternalSkipFactor() {return this->InternalSkipFactor;};
  
  int *GetAxisTextureSize() {return &(this->AxisTextureSize[0][0]);};

  int GetSaveTextures() {return this->SaveTextures;};

  unsigned char *GetTexture() {return this->Texture;};

  int GetNumberOfVolumes() {return this->volNum;};
  void RescaleData(unsigned char* texture,  int size[3], float spacing[3], int scaleFactor[2], int volume);


//ETX


protected:
  vtkVolumeTextureMapper3D();
  ~vtkVolumeTextureMapper3D();

  void InitializeRender( vtkRenderer *ren, vtkVolume *vol )
    {this->InitializeRender( ren, vol, -1 );}
  
  void InitializeRender( vtkRenderer *ren, vtkVolume *vol, int majorDirection );

  void GenerateTextures( vtkRenderer *ren, vtkVolume *vol, int volume );

 void UpdateColorTable(int colorTable[256][4], int volume);

 

  int  MajorDirection;
  int  TargetTextureSize[2];

 int  MaximumNumberOfPlanes;
  int  InternalSkipFactor;
  int  MaximumStorageSize;
  int  MaximumNumberOfVolumes;
  int  volNum;
  
  unsigned char  *Texture;
  unsigned char *rescaledTexture;
  int             TextureSize;
  int             SaveTextures;
  vtkTimeStamp    TextureMTime;
  
  int             AxisTextureSize[3][3];
  
  int xySize[3][2];
    int TFdata[4][256][3][2];
    bool changedTable[3];
    int colorTable[256][4];
    int colorMinMax[3][4][2];
    int TFnum[3][4];
    int dimension[3][3];
    int tempData[256][256][4];
    int histArray[3][256];
    int histMax[3];
    int diffX ;
    int diffY ;
    int boxSize;
    int clipPlaneNum ;
    int numberOfPlanes ;
    double planeEq[6][4];
    double currentPlane[6][3]; 
    int currentDistance[6];
    int currentSpacing;
    int currentType;
    double currentAngle[3];
    unsigned char* textureToMemory;
    bool volToClip[3];
    double planePointCube[6][3];
    int clipPlaneEnable[6];
    int clipLines;
    float currentTransformation[3][4][4];
    int tMatrixChanged[3];
    int enableVolume [3];
    float origin[3];

  
private:
  vtkVolumeTextureMapper3D(const vtkVolumeTextureMapper3D&);  // Not implemented.
  void operator=(const vtkVolumeTextureMapper3D&);  // Not implemented.
};


#endif



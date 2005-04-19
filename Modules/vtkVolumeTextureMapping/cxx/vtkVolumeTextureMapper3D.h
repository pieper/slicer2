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


#ifndef __vtkVolumeTextureMapper3D_h
#define __vtkVolumeTextureMapper3D_h

#include "vtkVolumeTextureMapper.h"
#include <vtkVolumeTextureMappingConfigure.h>
#include "vtkMatrix4x4.h"
#include "vtkLookupTable.h"
#include "vtkImageData.h"
#include "vtkLookupTable.h"
class VTK_VOLUMETEXTUREMAPPING_EXPORT vtkVolumeTextureMapper3D : public vtkVolumeTextureMapper
{
public:
  vtkTypeRevisionMacro(vtkVolumeTextureMapper3D,vtkVolumeTextureMapper);
  static vtkVolumeTextureMapper3D *New();

  

void SetNumberOfVolumes( int num );
void AddTFPoint(int volume, int point, int value);
void ClearTF();
int IsColorTableChanged(int volume);
int GetArrayPos(int volume, int point, int value, int boundX, int boundY) ;
void ChangeTFPoint(int volume, int pos, int point, int value);
void SetNumberOfClipPlanes(int planeNum);
int GetNumberOfClipPlanes();
void ChangeDist(int plane, int dist);
void ChangeClipPlaneDir(int plane, int dir, vtkFloatingPointType angle);
void GetClipPlaneEquation(vtkFloatingPointType planeEquation[4], int planeNum);
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
int GetNumPoint(int currentVolume);
int GetPoint(int currentVolume, int num, int xORy);
int GetColorMinMax(int volume, int minelmax, int rgb);
void RemoveTFPoint(int volume,int pointPos);
void SetEnableClipPlanes(int plane, int type);
void GetEnableClipPlanes(int enableClip[6]);
void ChangeType(int type);
void ResetClipPlanes(int type);
void ChangeSpacing(int spacing);
void ComputePlaneEquation(vtkFloatingPointType plane[4], vtkFloatingPointType point[4], vtkFloatingPointType normal[3]);
void Rotate(int dir, vtkFloatingPointType angle);
void EnableClipLines(int value);
int IsClipLinesEnable();
void UpdateTransformMatrix(int volume, vtkFloatingPointType t00, vtkFloatingPointType t01, vtkFloatingPointType t02, vtkFloatingPointType t03, vtkFloatingPointType t10, vtkFloatingPointType t11, vtkFloatingPointType t12, vtkFloatingPointType t13, vtkFloatingPointType t20, vtkFloatingPointType t21, vtkFloatingPointType t22, vtkFloatingPointType t23, vtkFloatingPointType t30, vtkFloatingPointType t31, vtkFloatingPointType t32, vtkFloatingPointType t33 ); 
void GetTransformMatrix(vtkFloatingPointType transfMatrix[4][4], int volume);
int IsTMatrixChanged(int volume);
vtkFloatingPointType GetTransformMatrixElement(int volume, int row, int column);
void SetEnableVolume(int volume, int type);
void GetEnableVolume(int enVol[3]) ;
int GetClipNum();
 void SetTransformMatrixElement(int volume, int row, int column, vtkFloatingPointType value);
 void SetDimension(int volume, int dir, int dims);
int GetTextureDimension(int volume, int dir);
void SetBoxSize(vtkFloatingPointType size);
int GetBoxSize();
int GetHistValue(int volume, int index);
void UpdateTransformMatrix(int volume, vtkMatrix4x4 *transMatrix );
 void SetOrigin(vtkFloatingPointType o_x, vtkFloatingPointType o_y, vtkFloatingPointType o_z);
void  GetOrigin(vtkFloatingPointType o[3]);
void SetHistMax(int volume, int value);
int GetHistMax(int volume);
void SetHistValue(int volume, int index);
void SetColorTable(vtkLookupTable *lookupTable, int volume);
void GetColorTable(int colorTable[256][4], int volume);
//BTX

  // Description:
  // WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
  // DO NOT USE THIS METHOD OUTSIDE OF THE RENDERING PROCESS
  // Render the volume
  virtual void Render(vtkRenderer *, vtkVolume *) {};

  virtual void CreateSubImages(unsigned char* vtkNotUsed(texture),
                            int vtkNotUsed(size)[3],
                            vtkFloatingPointType vtkNotUsed(spacing)[3]) {};

  

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
    int TFdata[256][3][2];
    bool changedTable[3];
    int colorTable[256][4];
    int colorMinMax[3][4][2];
    int TFnum[3];
    int dimension[3][3];
    int histArray[3][256];
    int histMax[3];
    int diffX ;
    int diffY ;
    int boxSize;
    int clipPlaneNum ;
    int numberOfPlanes ;
    vtkFloatingPointType planeEq[6][4];
    vtkFloatingPointType currentPlane[6][3]; 
    int currentDistance[6];
    int currentSpacing;
    int currentType;
    vtkFloatingPointType currentAngle[3];
    unsigned char* textureToMemory;
    bool volToClip[3];
    vtkFloatingPointType planePointCube[6][3];
    int clipPlaneEnable[6];
    int clipLines;
    vtkFloatingPointType currentTransformation[3][4][4];
    int tMatrixChanged[3];
    int enableVolume [3];
    vtkMatrix4x4  *rotate;
    vtkFloatingPointType origin[3];
    vtkImageData *imageDataResampled;
    vtkLookupTable *LUT0;
    vtkLookupTable *LUT1;
    vtkLookupTable *LUT2;
  
private:
  vtkVolumeTextureMapper3D(const vtkVolumeTextureMapper3D&);  // Not implemented.
  void operator=(const vtkVolumeTextureMapper3D&);  // Not implemented.
};


#endif



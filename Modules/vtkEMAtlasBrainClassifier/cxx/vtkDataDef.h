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
#ifndef __vtkDataDef_h
#define __vtkDataDef_h


#include <vtkEMAtlasBrainClassifierConfigure.h>
#include "vtkObject.h"

// ----------------------------------------------------
// Stucture and function needed for convolution
// ----------------------------------------------------

//
//BTX - begin tcl exclude
//
// From Simon convolution.cxx

typedef struct {
  float *input;
  int nrow;
  int ncol;
  int nslice;
  float *filter;
  int *indexes;
  int numindexes;
  int M1;
  int M2;
  int N1;
  int N2;
  int O1;
  int O2;
  float *output;
  int startindex;
  int endindex;   /* Process voxels in the range [startindex, endindex) */
} convolution_filter_work;

// ----------------------------------------------------------------------------------------------
// Definitions for 3D float array EMVolume
// ----------------------------------------------------------------------------------------------/ 

// Kilian turn around dimension so it is y,x,z like in matlab ! 
class VTK_EMATLASBRAINCLASSIFIER_EXPORT EMVolume {
public:
  //static EMVolume *New() {return (new vtkDataDef);}
  EMVolume(){this->Data  = NULL;this->MaxX = this->MaxY = this->MaxZ = this->MaxXY = this->MaxXYZ = 0;}
  EMVolume(int initZ,int initY, int initX) {this->allocate(initZ,initY, initX);}
  ~EMVolume() {this->deallocate();}

  // Convolution in all three direction 
  // Can be made using less memory but then it will be probably be slower
  void Conv(float *v,int vLen) {
    this->ConvY(v,vLen);
    this->ConvX(v,vLen);
    this->ConvZ(v,vLen);
  }
  void Resize(int DimZ,int DimY,int DimX) {
    if ((this->MaxX == DimX) && (this->MaxY == DimY) && (this->MaxZ == DimZ)) return;
    this->deallocate();this->allocate(DimZ,DimY,DimX);
  }

  float& operator () (int z,int y, int x) {return this->Data[x+this->MaxX*y + this->MaxXY*z];}
  const float& operator () (int z,int y, int x) const {return this->Data[x+this->MaxX*y + this->MaxXY*z];}

  EMVolume & operator = (const EMVolume &trg) {
    if ( this->Data == trg.Data ) return *this;
    if ((this->MaxX != trg.MaxX) || (this->MaxY != trg.MaxY) ||(this->MaxZ != trg.MaxZ)) {
    }
    for (int i = 0; i < this->MaxXYZ; i++ ) this->Data[i] = trg.Data[i];
    return *this;
  }
  // Kilian : Just to be compatible with older version
  void Conv(double *v,int vLen);
  // End -  Multi Thread Function
  void ConvY(float *v, int vLen);
  void ConvX(float *v, int vLen);
  // Same as above only sorce and target Volume are different => Slower 
  void ConvX(EMVolume &src,float v[], int vLen);
  void ConvZ(float *v, int vLen);
  // Same as above only sorce and target Volume are different => Slower 
  void ConvZ(EMVolume &src,float v[], int vLen);
  void Print(char name[]);
  void Test(int Vdim);
  void SetValue(float val) {
    if (val) {for (int i = 0; i < this->MaxXYZ; i++ ) this->Data[i] = val;}
    else { memset(this->Data, 0,sizeof(float)*this->MaxXYZ);}
  }
  float* GetData() {return this->Data;}
 
protected :
  float *Data;
  int MaxX, MaxY, MaxZ, MaxXY, MaxXYZ;

  void allocate (int initZ,int initY, int initX) {
    this->MaxX  = initX;this->MaxY  = initY;this->MaxZ  = initZ;
    this->MaxXY = initX*initY; this->MaxXYZ = this->MaxXY*this->MaxZ;
    this->Data = new float[this->MaxXYZ];
  } 

  void deallocate () {
    if (this->Data) delete[] this->Data;
    this->Data  = NULL;this->MaxX = this->MaxY = this->MaxZ  =  this->MaxXY = this->MaxXYZ = 0;
  }


}; 

// ----------------------------------------------------------------------------------------------
// Definitions for 5D float array EMTriVolume (lower triangle)
// ----------------------------------------------------------------------------------------------/ 
// It is a 5 dimensional Volume where m[t1][t2][z][y][x] t1>= t2 is only defined 
// Lower Traingular matrix - or a symmetric matrix where you only save the lower triangle
class VTK_EMATLASBRAINCLASSIFIER_EXPORT EMTriVolume {
protected :
  EMVolume **TriVolume;
  int Dim;

  void allocate (int initDim, int initZ,int initY, int initX) {
    this->Dim = initDim;
    int x,y;
    this->TriVolume = new EMVolume*[Dim];
    for (y=0; y < initDim; y++) {
      this->TriVolume[y] = new EMVolume[y+1];
      for (x = 0; x <= y ; x++) this->TriVolume[y][x].Resize(initZ,initY,initX);
    }
  }
  void deallocate () {
    if (this->TriVolume) {
      for (int y = 0; y < this->Dim; y++) delete[] this->TriVolume[y];
      delete[] this->TriVolume;
    }
    this->TriVolume  = NULL;this->Dim  = 0;
  }
public:
  EMTriVolume(){this->TriVolume  = NULL;this->Dim  = 0;}
  EMTriVolume(int initDim,int initZ,int initY, int initX) {this->allocate(initDim,initZ,initY, initX);}
  ~EMTriVolume() {this->deallocate();}

  void Resize(int initDim, int DimZ,int DimY,int DimX) {
    this->deallocate();this->allocate(initDim,DimZ,DimY,DimX);
  }

  float& operator () (int t1, int t2, int z,int y, int x) {return this->TriVolume[t1][t2](z,y,x);}
  const float& operator () (int t1, int t2, int z,int y, int x) const {return this->TriVolume[t1][t2](z,y,x);}

  // Kilian : Just to be complient with old version
  void Conv(double *v,int vLen) {
    float *v_f = new float[vLen];
    for (int i = 0; i < vLen; i++) v_f[i] = float(v[i]);
    this->Conv(v_f,vLen);  
    delete[] v_f;
  }

  void Conv(float *v,int vLen) {
    int x,y;
    for (y=0 ; y < this->Dim; y++) { 
      for (x = 0; x <= y; x++) this->TriVolume[y][x].Conv(v,vLen);
    }
  }
  void SetValue(float val) {
    int x,y;
    for (y=0 ; y < this->Dim; y++) { 
      for (x = 0; x <= y; x++) this->TriVolume[y][x].SetValue(val);
    }
  }
};



//
//ETX - end tcl exclude
//

// ----------------------------------------------------------------------------------------------
// Dummy class 
// ----------------------------------------------------------------------------------------------/ 
class VTK_EMATLASBRAINCLASSIFIER_EXPORT vtkDataDef { 
public:
  static vtkDataDef *New() {return (new vtkDataDef);}
protected:

};

#endif

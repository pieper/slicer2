/*=auto=========================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy (but NOT distribute) this
software and its documentation for any NON-COMMERCIAL purpose, provided
that existing copyright notices are retained verbatim in all copies.
The authors grant permission to modify this software and its documentation 
for any NON-COMMERCIAL purpose, provided that such modifications are not 
distributed without the explicit consent of the authors and that existing
copyright notices are retained in all copies. Some of the algorithms
implemented by this software are patented, observe all applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
=========================================================================auto=*/
#ifndef __vtkMrmlSlicer_h
#define __vtkMrmlSlicer_h

#include <fstream.h>
#include <stdlib.h>
#include <iostream.h>

#include "vtkCamera.h"
#include "vtkImageReformatIJK.h"
#include "vtkImageReformat.h"
#include "vtkImageOverlay.h"
#include "vtkImageMapToColors.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkPoints.h"
#include "vtkLookupTable.h"
#include "vtkMrmlVolume.h"
#include "vtkMrmlVolumeNode.h"
#include "vtkImageLabelOutline.h"
#include "vtkImageCrossHair2D.h"
#include "vtkImageZoom2D.h"
#include "vtkImageDouble2D.h"
#include "vtkIndirectLookupTable.h"
#include "vtkImageDrawROI.h"

#define NUM_SLICES 3

// Orient
#define MRML_SLICER_ORIENT_AXIAL        0
#define MRML_SLICER_ORIENT_SAGITTAL     1
#define MRML_SLICER_ORIENT_CORONAL      2
#define MRML_SLICER_ORIENT_INPLANE      3
#define MRML_SLICER_ORIENT_INPLANE90    4
#define MRML_SLICER_ORIENT_INPLANENEG90 5
#define MRML_SLICER_ORIENT_PERP         6
#define MRML_SLICER_ORIENT_ORIGSLICE    7
#define MRML_SLICER_ORIENT_AXISLICE     8
#define MRML_SLICER_ORIENT_SAGSLICE     9
#define MRML_SLICER_ORIENT_CORSLICE    10
#define MRML_SLICER_ORIENT_AXISAGCOR   11
#define MRML_SLICER_ORIENT_ORTHO       12
#define MRML_SLICER_ORIENT_SLICES      13
#define MRML_SLICER_NUM_ORIENT         14

class VTK_EXPORT vtkMrmlSlicer : public vtkObject 
{
public:

  // The Usual vtk class functions
  static vtkMrmlSlicer *New();
  const char *GetClassName() {return "vtkMrmlSlicer";};
  void PrintSelf(ostream& os, vtkIndent indent);
    
  // Description:
  // GetOutput slice images
  vtkImageData *GetOutput(int s) {
    this->Update(); return this->Overlay[s]->GetOutput();};
  vtkImageData *GetCursor(int s) {
    this->Update(); return this->Cursor[s]->GetOutput();};
  vtkImageData *GetActiveOutput(int s) {
	  this->Update();
    if (this->ActiveSlice == s) 
      return this->PolyDraw->GetOutput();
	  else 
      return this->Overlay[s]->GetOutput();
  };

  // Active Slice
  void SetActiveSlice(int s);
  vtkGetMacro(ActiveSlice, int);

  // Zoom factor
  void SetZoom(int s, float mag);
  void SetZoom(float mag);
  float GetZoom(int s) {return this->Zoom[s]->GetMagnification();};
  // Zoom center
  void SetZoomCenter(int s, float x, float y);
  void GetZoomCenter();
  vtkGetVector2Macro(ZoomCenter0, float);
  vtkGetVector2Macro(ZoomCenter1, float);
  vtkGetVector2Macro(ZoomCenter2, float);
  // Zoom auto center
  void SetZoomAutoCenter(int s, int yes);
  int GetZoomAutoCenter(int s) {return this->Zoom[s]->GetAutoCenter();};

  // Double
  void DoubleOn() {
    this->DoubleSliceSize = 1; this->BuildLowerTime.Modified();};
  void DoubleOff() {
    this->DoubleSliceSize = 0; this->BuildLowerTime.Modified();};

  // Cursor
  void SetShowCursor(int vis);
  void SetNumHashes(int hashes);
  void SetCursorColor(float red, float green, float blue);
  void SetCursorPosition(int s, int x, int y) {
    this->Cursor[s]->SetCursor(x, y);};

  vtkGetMacro(FieldOfView, float);
  void SetFieldOfView(float x);

  // Volumes
  void SetNoneVolume(vtkMrmlVolume *vol);
  vtkGetObjectMacro(NoneVolume, vtkMrmlVolume);
  void SetBackVolume(vtkMrmlVolume *vol);
  void SetForeVolume(vtkMrmlVolume *vol);
  void SetLabelVolume(vtkMrmlVolume *vol);
  void SetBackVolume( int s, vtkMrmlVolume *vol);
  void SetForeVolume( int s, vtkMrmlVolume *vol);
  void SetLabelVolume(int s, vtkMrmlVolume *vol);
  vtkMrmlVolume* GetBackVolume( int s) {return this->BackVolume[s];};
  vtkMrmlVolume* GetForeVolume( int s) {return this->ForeVolume[s];};
  vtkMrmlVolume* GetLabelVolume(int s) {return this->LabelVolume[s];};

  // Cursor Annotation
  int GetForePixel(int s, int x, int y);
  int GetBackPixel(int s, int x, int y);

  // Opacity
  void SetForeOpacity(float opacity);
  vtkGetMacro(ForeOpacity, float);

  // Coloring label maps
  void SetLabelIndirectLUT(vtkIndirectLookupTable *lut);

  // Slice Orient
  void SetOrient(int orient);
  void SetOrient(int s, int orient);
  void SetOrientString(char *str);
  void SetOrientString(int s, char *str);
  int GetOrient(int s) {return this->Orient[s];};
  char *GetOrientString(int s);
  char *GetOrientList() {return
"Axial Sagittal Coronal InPlane InPlane90 InPlaneNeg90 Perp OrigSlice AxiSlice SagSlice CorSlice";};

  // Slice Offset
  float GetOffsetRangeLow(int s) {
    return this->OffsetRange[s][this->Orient[s]][0];};
  float GetOffsetRangeHigh(int s) {
    return this->OffsetRange[s][this->Orient[s]][1];};
  void SetOffset(int s, float offset);
  void InitOffset(int s, char *str, float offset);
  float GetOffset(int s) {return this->Offset[s][this->Orient[s]];};
  float GetOffset(int s, char *str) {return 
	  this->Offset[s][ConvertStringToOrient(str)];};

  // Matrix
  vtkMatrix4x4 *GetReformatMatrix(int s) {return this->ReformatMatrix[s];};
  void ComputeNTPFromCamera(vtkCamera *camera);
  void SetDirectNTP(float nx, float ny, float nz,
    float tx, float ty, float tz, float px, float py, float pz);
  void SetDriver(int s, int d) {this->Driver[s] = d; this->Modified();};
  int GetDriver(int s) {return this->Driver[s];};
  double *GetP(int s);
  double *GetT(int s);
  double *GetN(int s);
  vtkGetVector3Macro(DirP, double);
  vtkGetVector3Macro(CamP, double);

  // Points
  void SetReformatPoint(int s, int x, int y);
  vtkGetVectorMacro(WldPoint, float, 3);
  vtkGetVectorMacro(IjkPoint, float, 3);
  void SetScreenPoint(int s, int x, int y);
  vtkGetVectorMacro(ReformatPoint, int, 2);
  vtkGetVectorMacro(Seed, int, 3);
  vtkGetVectorMacro(Seed2D, int, 3);

  // Description:
  //
  vtkGetMacro(BackFilter, int);
  vtkSetMacro(BackFilter, int);
  vtkBooleanMacro(BackFilter, int);
  vtkGetMacro(ForeFilter, int);
  vtkSetMacro(ForeFilter, int);
  vtkBooleanMacro(ForeFilter, int);
  vtkGetMacro(FilterActive, int);
  vtkSetMacro(FilterActive, int);
  vtkBooleanMacro(FilterActive, int);
  void SetFirstFilter(int s, vtkImageToImageFilter *filter);
  void SetLastFilter(int s, vtkImageToImageFilter *filter);
  vtkImageToImageFilter* GetFirstFilter(int s) {return this->FirstFilter[s];};
  vtkImageToImageFilter* GetLastFilter(int s) {return this->LastFilter[s];};

  // Draw
  void DrawSetColor(float r, float g, float b) {
	  this->PolyDraw->SetPointColor(r, g, b);
	  this->PolyDraw->SetLineColor(r, g, b);};
  void DrawSelectAll() {
    this->PolyDraw->SelectAllPoints();};
  void DrawDeselectAll() {
    this->PolyDraw->DeselectAllPoints();};
  void DrawDeleteSelected() {
    this->PolyDraw->DeleteSelectedPoints();};
  void DrawDeleteAll() {
    this->PolyDraw->DeleteAllPoints();};
  void DrawShowPoints(int s) {
    if (s) this->PolyDraw->SetPointRadius(1); 
	else this->PolyDraw->SetPointRadius(0);};
  void DrawSetRadius(int r) {
    this->PolyDraw->SetPointRadius(r); };
  int DrawGetRadius() {
    return this->PolyDraw->GetPointRadius(); };
  void DrawInsertPoint(int x, int y) {
    this->PolyDraw->InsertAfterSelectedPoint(x, y);};
  void DrawMoveInit(int x, int y) {
	  this->DrawX = x; this->DrawY = y;};
  void DrawMove(int x, int y) {
	this->PolyDraw->MoveSelectedPoints(x-this->DrawX, y-this->DrawY);
    this->DrawX = x; this->DrawY = y;};
  int DrawGetNumPoints() {
    return this->PolyDraw->GetNumPoints();};
  int DrawGetNumSelectedPoints() {
    return this->PolyDraw->GetNumSelectedPoints();};
  void DrawStartSelectBox(int x, int y) {
    this->PolyDraw->StartSelectBox(x, y);};
  void DrawDragSelectBox(int x, int y) {
    this->PolyDraw->DragSelectBox(x, y);};
  void DrawEndSelectBox(int x, int y) {
    this->PolyDraw->EndSelectBox(x, y);};
  vtkPoints* DrawGetPoints() {
    return this->PolyDraw->GetPoints();}
  void DrawComputeIjkPoints();
  vtkGetObjectMacro(DrawIjkPoints, vtkPoints);
  void DrawSetShapeToPolygon() {this->PolyDraw->SetShapeToPolygon();};
  void DrawSetShapeToLines() {this->PolyDraw->SetShapeToLines();};
  void DrawSetShapeToPoints() {this->PolyDraw->SetShapeToPoints();};
  char* GetShapeString() {return this->PolyDraw->GetShapeString();};

  // Description:
  //
  void Update();

  // Description:
  //
  void ReformatModified() {this->BuildUpperTime.Modified();};

protected:
  vtkMrmlSlicer();
  ~vtkMrmlSlicer();
  vtkMrmlSlicer(const vtkMrmlSlicer&) {};
  void operator=(const vtkMrmlSlicer&) {};

  void ComputeOffsetRange();
  void ComputeOffsetRangeIJK(int s);
  vtkMrmlVolume* GetIJKVolume(int s);
  vtkImageReformat* GetIJKReformat(int s);
  int IsOrientIJK(int s);
  void BuildLower(int s);
  void BuildUpper(int s);
  int ConvertStringToOrient(char *str);
  char* ConvertOrientToString(int orient);
  void ComputeReformatMatrix(int s);
  void ComputeReformatMatrixIJK(int s, float offset, vtkMatrix4x4 *ref);
  float GetOffsetForComputation(int s);

  int ActiveSlice;
  int DoubleSliceSize;
  float FieldOfView;
  float ForeOpacity;

  double CamN[3];
  double CamT[3];
  double CamP[3];
  double DirN[3];
  double DirT[3];
  double DirP[3];

  int Driver[NUM_SLICES];
  float OffsetRange[NUM_SLICES][MRML_SLICER_NUM_ORIENT][2];
  int Orient[NUM_SLICES];
  int Offset[NUM_SLICES][MRML_SLICER_NUM_ORIENT];

  vtkImageReformat     *BackReformat[NUM_SLICES];
  vtkImageReformat     *ForeReformat[NUM_SLICES];
  vtkImageReformat     *LabelReformat[NUM_SLICES];
  vtkImageMapToColors  *BackMapper[NUM_SLICES];
  vtkImageMapToColors  *ForeMapper[NUM_SLICES];
  vtkImageMapToColors  *LabelMapper[NUM_SLICES];
  vtkImageOverlay      *Overlay[NUM_SLICES];
  vtkMrmlVolume        *BackVolume[NUM_SLICES];
  vtkMrmlVolume        *ForeVolume[NUM_SLICES];
  vtkMrmlVolume        *LabelVolume[NUM_SLICES];
  vtkMatrix4x4         *ReformatMatrix[NUM_SLICES];
  vtkImageLabelOutline *LabelOutline[NUM_SLICES];
  vtkImageCrossHair2D  *Cursor[NUM_SLICES];
  vtkImageZoom2D       *Zoom[NUM_SLICES];
  vtkImageDouble2D     *Double[NUM_SLICES];
  vtkImageDrawROI      *PolyDraw;
  vtkImageReformatIJK  *ReformatIJK;
  vtkMrmlVolume        *NoneVolume;
  vtkMrmlVolumeNode    *NoneNode;

  // Colors
  vtkIndirectLookupTable *LabelIndirectLUT;

  vtkImageToImageFilter *FirstFilter[NUM_SLICES];
  vtkImageToImageFilter *LastFilter[NUM_SLICES];
  int BackFilter;
  int ForeFilter;
  int FilterActive;

  // DAVE need a better way
  float ZoomCenter0[2];
  float ZoomCenter1[2];
  float ZoomCenter2[2];

  // Point
  float WldPoint[3];
  float IjkPoint[3];
  int ReformatPoint[2];
  int Seed[3];
  int Seed2D[3];

  // Draw
  vtkPoints *DrawIjkPoints;
  int DrawX;
  int DrawY;

  vtkTimeStamp UpdateTime;
  vtkTimeStamp BuildLowerTime;
  vtkTimeStamp BuildUpperTime;
};

#endif

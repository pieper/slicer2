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
// .NAME vtkImageDrawROI -  Flexible threshold
// .SECTION Description
// vtkImageDrawROI Can do binary or continous thresholding

#ifndef __vtkImageDrawROI_h
#define __vtkImageDrawROI_h

#include "vtkImageInPlaceFilter.h"
#include "vtkPoints.h"
#include "point.h"

#define ROI_SHAPE_POLYGON 1
#define ROI_SHAPE_LINES   2
#define ROI_SHAPE_POINTS  3

class VTK_EXPORT vtkImageDrawROI : public vtkImageInPlaceFilter
{
public:	
	static vtkImageDrawROI *New();
  vtkTypeMacro(vtkImageDrawROI,vtkImageInPlaceFilter);
	void PrintSelf(ostream& os, vtkIndent indent);

	void SelectPoint(int x, int y);
	void DeselectPoint(int x, int y);
	void TogglePoint(int x, int y);
	void StartSelectBox(int x, int y);
	void DragSelectBox(int x, int y);
	void EndSelectBox(int x, int y);
	void SelectAllPoints();
	void DeselectAllPoints();
	void AppendPoint(int x, int y);
	void InsertAfterSelectedPoint(int x, int y);
	void DeleteSelectedPoints();
	void DeleteAllPoints();
	void MoveSelectedPoints(int deltaX, int deltaY);
	void MoveAllPoints(int deltaX, int deltaY);

	vtkSetVector3Macro(PointColor, float);
	vtkGetVectorMacro(PointColor, float, 3);
	vtkSetVector3Macro(SelectedPointColor, float);
	vtkGetVectorMacro(SelectedPointColor, float, 3);
	vtkSetVector3Macro(LineColor, float);
	vtkGetVectorMacro(LineColor, float, 3);

	vtkGetMacro(NumSelectedPoints, int);
	vtkGetMacro(NumPoints, int);

	vtkGetMacro(PointRadius, int);
	vtkSetMacro(PointRadius, int);

	vtkGetMacro(HideROI, int);
	vtkSetMacro(HideROI, int);
	vtkBooleanMacro(HideROI, int);

	void SetShapeToPolygon() {
		this->Shape = ROI_SHAPE_POLYGON; this->Modified();};
	void SetShapeToLines() {
		this->Shape  = ROI_SHAPE_LINES; this->Modified();};
	void SetShapeToPoints() {
		this->Shape = ROI_SHAPE_POINTS; this->Modified();};
	int GetShape() {return this->Shape;};
	char *GetShapeString() {switch (this->Shape) {
		case ROI_SHAPE_POLYGON: return "Polygon";
		case ROI_SHAPE_LINES: return "Lines";
		case ROI_SHAPE_POINTS: return "Points";
		default: return "None";};};

	vtkPoints* GetPoints(void);

protected:
        vtkImageDrawROI();
	~vtkImageDrawROI();
        vtkImageDrawROI(const vtkImageDrawROI&) {};
	void operator=(const vtkImageDrawROI&) {};

	vtkPoints *Points;
	
	Point *firstPoint;
	Point *lastPoint;

	int drawSelectBox;
	Point sbox;
	Point sbox1;
	Point sbox2;

	int NumPoints;
	int NumSelectedPoints;
	int PointRadius;
	int HideROI;
	int Shape;

	float PointColor[3];
	float SelectedPointColor[3];
	float LineColor[3];

	void DrawPoints(vtkImageData *outData, int extent[6]);
	void DrawLines(vtkImageData *outData, int extent[6]);
	void DrawSelectBox(vtkImageData *outData, int extent[6]);

	// Not threaded because its too simple of a filter
	void Execute(vtkImageData *inData, vtkImageData *outData);
};

#endif


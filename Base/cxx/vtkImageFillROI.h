/*=auto=========================================================================
Copyright (c) 2001 Surgical Planning Lab, Brigham and Women's Hospital
 
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
// .NAME vtkImageFillROI - Paints on top of an image.
// .SECTION Description
// vtkImageFillROI will draw a cursor on a 2d image.

#ifndef __vtkImageFillROI_h
#define __vtkImageFillROI_h

#include "vtkImageInPlaceFilter.h"
#include "vtkPoints.h"
#include "point.h"
#include <string.h>

#define SHAPE_POLYGON 1
#define SHAPE_LINES   2
#define SHAPE_POINTS  3

class VTK_EXPORT vtkImageFillROI : public vtkImageInPlaceFilter
{
public:
	static vtkImageFillROI *New();
  vtkTypeMacro(vtkImageFillROI,vtkImageInPlaceFilter);
	void PrintSelf(ostream& os, vtkIndent indent);

	vtkSetMacro(Value, float);
	vtkGetMacro(Value, float);

	void SetShapeToPolygon() {this->shape = SHAPE_POLYGON;};
	void SetShapeToLines() {this->shape = SHAPE_LINES;};
	void SetShapeToPoints() {this->shape = SHAPE_POINTS;};
	void SetShape(int s) {this->shape = s;};
	int GetShape() {return this->shape;};
	char *GetShapeString() {switch (this->shape) {
		case SHAPE_POLYGON: return "Polygon";
		case SHAPE_LINES: return "Lines";
		case SHAPE_POINTS: return "Points";
		default: return "None";};};
	void SetShapeString(char *str) {
		if (strcmp(str,"Polygon") == 0) this->SetShapeToPolygon();
		else if (strcmp(str,"Lines") == 0) this->SetShapeToLines();
		else this->SetShapeToPoints();};

	vtkSetMacro(Radius, int);
	vtkGetMacro(Radius, int);

	vtkSetObjectMacro(Points, vtkPoints);
	vtkGetObjectMacro(Points, vtkPoints);

protected:
	vtkImageFillROI();
  ~vtkImageFillROI();
  vtkImageFillROI(const vtkImageFillROI&) {};
	void operator=(const vtkImageFillROI&) {};

	vtkPoints *Points;
	float Value;
	int Radius;
	int shape;

	// not threaded because too simple a filter
        void Execute(vtkImageData *inData, vtkImageData *outData);
};
#endif

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
// .NAME vtkImageConnectivity - Identify and process islands of similar pixels
// .SECTION Description
//  The input data type must be shorts.

#ifndef __vtkImageConnectivity_h
#define __vtkImageConnectivity_h

#include "vtkImageToImageFilter.h"

#define CONNECTIVITY_IDENTIFY 1
#define CONNECTIVITY_REMOVE 2
#define CONNECTIVITY_CHANGE 3
#define CONNECTIVITY_MEASURE 4
#define CONNECTIVITY_SAVE 5

class VTK_EXPORT vtkImageConnectivity : public vtkImageToImageFilter
{
public:
	static vtkImageConnectivity *New();
  vtkTypeMacro(vtkImageConnectivity,vtkImageToImageFilter);
	void PrintSelf(ostream& os, vtkIndent indent);

  // Function
  void SetFunction(int func) {
	  this->function = func;};
  void SetFunctionToIdentifyIslands() {
	  this->SetFunction(CONNECTIVITY_IDENTIFY);};
  void SetFunctionToRemoveIslands() {
	  this->SetFunction(CONNECTIVITY_REMOVE);};
  void SetFunctionToChangeIsland() {
	  this->SetFunction(CONNECTIVITY_CHANGE);};
  void SetFunctionToMeasureIsland() {
	  this->SetFunction(CONNECTIVITY_MEASURE);};
  void SetFunctionToSaveIsland() {
	  this->SetFunction(CONNECTIVITY_SAVE);};
  int GetFunction() {return this->function;}
  char* GetFunctionString();

	// outputs of MeasureIsland
	vtkGetMacro(IslandSize, int);
	vtkSetMacro(IslandSize, int);
	vtkGetMacro(LargestIslandSize, int);
	vtkSetMacro(LargestIslandSize, int);

  // Description:
	vtkGetMacro(SliceBySlice, int);
	vtkSetMacro(SliceBySlice, int);
	vtkBooleanMacro(SliceBySlice, int);

  vtkSetVector3Macro(Seed, int);
	vtkGetVector3Macro(Seed, int);

	vtkSetMacro(OutputLabel, int);
	vtkGetMacro(OutputLabel, int);
	vtkSetMacro(MinSize, int);
	vtkGetMacro(MinSize, int);
	vtkSetMacro(Background, float);
	vtkGetMacro(Background, float);
	vtkSetMacro(MinForeground, float);
	vtkGetMacro(MinForeground, float);
	vtkSetMacro(MaxForeground, float);
	vtkGetMacro(MaxForeground, float);

protected:
	vtkImageConnectivity();
	~vtkImageConnectivity(){};
	vtkImageConnectivity(const vtkImageConnectivity&) {};
	void operator=(const vtkImageConnectivity&) {};

	float Background;
	float MinForeground;
	float MaxForeground;
	int LargestIslandSize;
	int IslandSize;
	int MinSize;
	int OutputLabel;
	int Seed[3];
	int function;
  int SliceBySlice;
	
	void Execute(vtkImageData *inData, vtkImageData *outData);
};

#endif




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
// .NAME vtkImageCCA -  Flexible threshold
// .SECTION Description
//  The output data type may be different than the output, but defaults
// to the same type.

#ifndef __vtkImageCCA_h
#define __vtkImageCCA_h

#include "vtkImageFilter.h"

#define FUNCTION_REMOVE 1
#define FUNCTION_CHANGE 2
#define FUNCTION_MEASURE 3
#define FUNCTION_SAVE 4

class VTK_EXPORT vtkImageCCA : public vtkImageFilter
{
public:
	static vtkImageCCA *New();
	const char *GetClassName() {return "vtkImageCCA";};
	void PrintSelf(ostream& os, vtkIndent indent);

  // Function
  void SetFunction(int func) {
	  this->function = func;};
  void SetFunctionToRemoveIslands() {
	  this->SetFunction(FUNCTION_REMOVE);};
  void SetFunctionToChangeIsland() {
	  this->SetFunction(FUNCTION_CHANGE);};
  void SetFunctionToMeasureIsland() {
	  this->SetFunction(FUNCTION_MEASURE);};
  void SetFunctionToSaveIsland() {
	  this->SetFunction(FUNCTION_SAVE);};
  int GetFunction() {return this->function;}
  char* GetFunctionString();

	// outputs of MeasureIsland
	vtkGetMacro(IslandSize, int);
	vtkSetMacro(IslandSize, int);
	vtkGetMacro(LargestIslandSize, int);
	vtkSetMacro(LargestIslandSize, int);

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
	vtkImageCCA();
	~vtkImageCCA(){};
	vtkImageCCA(const vtkImageCCA&) {};
	void operator=(const vtkImageCCA&) {};

	float Background;
	float MinForeground;
	float MaxForeground;
	int LargestIslandSize;
	int IslandSize;
	int MinSize;
	int OutputLabel;
	int Seed[3];
	int function;
	
	void Execute(vtkImageData *inData, vtkImageData *outData);
};

#endif




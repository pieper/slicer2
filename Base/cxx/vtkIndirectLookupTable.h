/*=auto=========================================================================

(c) Copyright 2002 Massachusetts Institute of Technology

Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for any purpose, 
provided that the above copyright notice and the following three paragraphs 
appear on all copies of this software.  Use of this software constitutes 
acceptance of these terms and conditions.

IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================auto=*/
// .NAME vtkIndirectLookupTable - indirectly map scalar values into colors.
// .SECTION Description
// vtkIndirectLookupTable is an object that is used by mapper objects.
// Normally, a mapper maps scalar values to colors by using a 
// VtkLookupTable object.  This object offers an alternative that can
// execute 2.4 times faster.  Mapping is performed indirectly by first
// constructing an internal, intermediate lookup table that maps each
// possible scalar value to an index into the externally specified
// LookupTable.
//
// This object uses the first color in the LookupTable as the color to use for
// data that fails to satisfy the thresholds.  Therefore, it overwrites this
// first color with transparent black during the Build procedure.
//
// DAVE: This only maps to RGBA, but it should have all the options of a
// vtkScalarsToColors class.
//
// .SECTION See Also
// vtkLookupTable

#ifndef __vtkIndirectLookupTable_h
#define __vtkIndirectLookupTable_h

#include "vtkObject.h"
#include "vtkLookupTable.h"
#include "vtkScalarsToColors.h"
#include "vtkUnsignedShortArray.h"

class VTK_EXPORT vtkIndirectLookupTable : public vtkScalarsToColors
{
public:
  static vtkIndirectLookupTable *New();
  vtkTypeMacro(vtkIndirectLookupTable,vtkScalarsToColors);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Generate lookup table from window and level or direct.
  // Table is built as a linear ramp, centered at Level and of width Window.
  void Build();

  // Description:
  // Set the Window for the lookuptable. Window is the width of the
  // lookuptable ramp.
  vtkSetClampMacro(Window,float,1.0,65536.0);
  vtkGetMacro(Window,float);

  // Description:
  // Set the Level for the lookuptable. Level is the center of the ramp of
  // the lookuptable.  ramp.
  vtkSetMacro(Level,float);
  vtkGetMacro(Level,float);

  // Description:
  // Set/get the threshold
  vtkBooleanMacro(ApplyThreshold, int);
  vtkSetMacro(ApplyThreshold,int);
  vtkGetMacro(ApplyThreshold,int);
  vtkSetMacro(LowerThreshold,float);
  vtkGetMacro(LowerThreshold,float);
  vtkSetMacro(UpperThreshold,float);
  vtkGetMacro(UpperThreshold,float);

  // Description:
  // Sets/Gets the range of scalars which will be mapped.
  float *GetRange() {return &this->Range[0];};
  void SetRange(float min, float max) {
    this->SetMapRange((long)min, (long)max);};
  void SetRange(float rng[2]) {
    this->SetRange(rng[0],rng[1]);};
  vtkGetMacro(MapOffset,long);

  // Description:
  // Lookup table containing the RGBA colors that the input scalars will
  // be indirectly mapped to
  vtkSetObjectMacro(LookupTable,vtkLookupTable);
  vtkGetObjectMacro(LookupTable,vtkLookupTable);

  // Description:
  // Set the Default index into the LookupTable that will be used
  // when the Direct map is initialized.
  vtkSetMacro(DirectDefaultIndex,long);
  vtkGetMacro(DirectDefaultIndex,long);

  // Description:
  // We need to check the modified time of the LookupTable too.
  unsigned long GetMTime();

  // Description:
  // The map for translating input scalar values to LookupTable indices
  // can be either specified directly or by Window/Level.
  // These functions set/get which method to use.
  vtkGetMacro(Direct, int);
  vtkSetMacro(Direct, int);    
  vtkBooleanMacro(Direct, int);

  // Description:
  // Initialize the map to translate all input scalar values to 
  // the DirectDefaultIndex into the LookupTable
  void InitDirect();

#if (VTK_MAJOR_VERSION == 3 && VTK_MINOR_VERSION == 2)
  // Description:
  // Map one value through the lookup table and return the color as
  // an RGB array of floats between 0 and 1.
  float *GetColor(float x) { 
    return vtkScalarsToColors::GetColor(x); }
#endif

  void GetColor(float x, float rgb[3]);


  // Description:
  // Specifically set a certain input scalar value to map to a
  // certain index into the LookupTable
  void MapDirect(float scalar, int index);

  // Description:
  // Indirectly Map one value through the LookupTable
  virtual unsigned char *MapValue(float v);

  // Description:
  // indirectly map a set of scalars through the LookupTable
  void MapScalarsThroughTable2(void *input, unsigned char *output,
    int inputDataType, int numberOfValues, int inputIncrement,
    int outputFormat);

  // Description:
  // The indirect LUT
  vtkGetObjectMacro(Map,vtkUnsignedShortArray);

protected:
  vtkIndirectLookupTable();
  ~vtkIndirectLookupTable();
  vtkIndirectLookupTable(const vtkIndirectLookupTable&) {};
  void operator=(const vtkIndirectLookupTable&) {};

  float Range[2];
  long MapRange[2];
  long MapOffset;
  void SetMapRange(long min, long max);

  float Window;
  float Level;
  float UpperThreshold;
  float LowerThreshold;
  int ApplyThreshold;

  short inHiPrev;
  short inLoPrev;
  short outHiPrev;
  short outLoPrev;
  void WindowLevel();

  int Direct;
  int DirectDefaultIndex;

  vtkLookupTable *LookupTable;

  vtkUnsignedShortArray *WinLvlMap;
  vtkUnsignedShortArray *DirectMap;
  vtkUnsignedShortArray *Map;

  vtkTimeStamp BuildTime;
};

#endif



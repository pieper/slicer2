/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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
// .NAME vtkImageLiveWireTester - Wrapper around vtkImageLiveWire
// .SECTION Description
//  This poorly named class handles the multiple edge inputs to 
// vtkImageLiveWire.  It replaces a bit of tcl code and may be
// replaced itself in future...
//

#ifndef __vtkImageLiveWireTester_h
#define __vtkImageLiveWireTester_h

#include "vtkImageToImageFilter.h"
#include "vtkImageLiveWire.h"
#include "vtkImageLiveWireEdgeWeights.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkImageLiveWireTester : public vtkImageToImageFilter
{
  public:
  static vtkImageLiveWireTester *New();
  vtkTypeMacro(vtkImageLiveWireTester,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // LiveWire object this class makes edges for
  vtkSetObjectMacro(LiveWire, vtkImageLiveWire);
  vtkGetObjectMacro(LiveWire, vtkImageLiveWire);

  // Description:
  // Set up the pipeline (give edge images to LiveWire filter)
  void InitializePipeline();

  // Description:
  // Number of filters for edges (number of directions of edges)
  vtkGetMacro(NumberOfEdgeFilters, int);
  void SetNumberOfEdgeFilters(int number);

  // Description:
  // Array of edge filters.
  vtkImageLiveWireEdgeWeights **GetEdgeFilters(){return this->EdgeFilters;};
  vtkImageLiveWireEdgeWeights *GetEdgeFilter(int i){return this->EdgeFilters[i];};

  // Description:
  // Returns output of one of the edge-producing filters.
  vtkImageData *GetEdgeImage(int filter);
  
  // Description: 
  // The file where all edge filters' settings will be written when using 
  // WriteFeatureSettings.  Use this to save training information.
  vtkSetStringMacro(SettingsFileName);
  vtkGetStringMacro(SettingsFileName);

  // Description: 
  // Write settings of all edge filters to file
  void WriteFilterSettings();

protected:
  vtkImageLiveWireTester();
  ~vtkImageLiveWireTester();
  vtkImageLiveWireTester(const vtkImageLiveWireTester&) {};
  void operator=(const vtkImageLiveWireTester&) {};

  char *SettingsFileName;

  // Description:
  // object we are creating input (multiple edges) for.
  // this filter's output should also be input to it
  vtkImageLiveWire *LiveWire;

  // Description:
  // Array of filters to find edges in all directions
  vtkImageLiveWireEdgeWeights **EdgeFilters;
  int NumberOfEdgeFilters;

  void Execute(vtkImageData *inData, vtkImageData *outData);
};

#endif




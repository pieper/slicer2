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
// .NAME vtkMrmlData - Abstract Object used in the slicer to perform
// everything related to the access and display of data.
// .SECTION Description
// Used in conjunction with a vtkMrmlNode (which neatly describes
// display settings, file locations, etc.).  Essentially, the MRML 
// node gives the high level description of what this class should 
// actually do with the data
// 

// NEED TO:
// make vtkMrmlNode.h:: Copy constructor to be virtual.
//                   :: Destructor needs to be virtual.
// 
// GetMTime to be virtual

#ifndef __vtkMrmlData_h
#define __vtkMrmlData_h

//#include <fstream.h>
#include <stdlib.h>
//#include <iostream.h>

#include "vtkProcessObject.h"
#include "vtkIndirectLookupTable.h"
#include "vtkMrmlNode.h"
#include "vtkLookupTable.h"
#include "vtkImageData.h"
#include "vtkSlicer.h"

//----------------------------------------------------------------------------
class VTK_SLICER_BASE_EXPORT vtkMrmlData : public vtkProcessObject {
  public:
    static vtkMrmlData *New();

  vtkTypeMacro(vtkMrmlData,vtkProcessObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // 
  // Dealing With Mrml Data
  //

  // Description:
  // Set/Get the MRML info
  vtkSetObjectMacro(MrmlNode, vtkMrmlNode);
  vtkGetObjectMacro(MrmlNode, vtkMrmlNode);
  
  // Description:
  // Copy another MmrlData's MrmlNode to this one
  // This does not need to be virtual 
  void CopyNode(vtkMrmlData *Data);

  //
  // Making sure the filters are UpToDate
  //

  // Description:
  // Provides opportunity to insure internal consistency before access.
  // Transfers all ivars from MrmlNode to internal VTK objects
  //
  // All subclasses MUST call vtkMRMLData::Update and vtkMRMLData::GetMTime
  virtual void Update();
  virtual unsigned long int GetMTime();
//  virtual vtkObject* GetOutput();
  vtkDataObject *GetOutput();

  //
  // Read/Write filters
  //

  // Description:
  // Read/Write the data
  // Return 1 on success, 0 on failure.
  virtual int Read() = 0;
  virtual int Write() = 0;

  // Description:
  // Has the object been changed in a way that one would want to write
  // it to disk? (to replace IsDirty...)
  vtkSetMacro(NeedToWrite, int);
  vtkGetMacro(NeedToWrite, int);
  vtkBooleanMacro(NeedToWrite, int);

  //
  // Display functions
  //

  // Description:
  // Get the indirect LUT (LookupTable).
  // If UseLabelLUT is on, then returns the LabelLUT, otherwise
  // the volume's own IndirectLookupTable.
  vtkIndirectLookupTable *GetIndirectLUT();

  // Description:
  // Set Label IndirectLookupTable
  vtkGetMacro(UseLabelIndirectLUT, int);
  vtkSetMacro(UseLabelIndirectLUT, int);
  vtkBooleanMacro(UseLabelIndirectLUT, int);
  vtkSetObjectMacro(LabelIndirectLUT, vtkIndirectLookupTable);
  vtkGetObjectMacro(LabelIndirectLUT, vtkIndirectLookupTable);

  // Description:
  // Set LookupTable
  void SetLookupTable(vtkLookupTable *lut) {
      this->IndirectLUT->SetLookupTable(lut);};
  vtkLookupTable *GetLookupTable() {
    return this->IndirectLUT->GetLookupTable();};

  // Description:
  // For internal use during Read/Write
  vtkGetObjectMacro(ProcessObject, vtkProcessObject);

  // Description
  // set the update progress method
  // recieves a vtkMrmlData
  static void vtkMrmlDataProgress(void *MrmlData);

protected:
  vtkMrmlData();
  // The virtual descructor is critical!!
  virtual ~vtkMrmlData();

  vtkMrmlData(const vtkMrmlData&) {};
  void operator=(const vtkMrmlData&) {};


  // Description: 
  // MUST be implemented by lower functions
  // If MrmlNode is None, create it.
  virtual void CheckMrmlNode();

  vtkMrmlNode *MrmlNode;
  void CheckLabelIndirectLUT();

  int UseLabelIndirectLUT;
  vtkIndirectLookupTable *IndirectLUT;
  vtkIndirectLookupTable *LabelIndirectLUT;

  int NeedToWrite;
  vtkProcessObject *ProcessObject;
  
};

#endif

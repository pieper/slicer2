/*=auto=========================================================================

(c) Copyright 2001 Massachusetts Institute of Technology

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
// .NAME vtkMrmlData - Object used in the slicer to perform
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

//----------------------------------------------------------------------------
class VTK_EXPORT vtkMrmlData : public vtkProcessObject {
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
  // Copy another MmrlVolume's MrmlData to this one
  virtual void CopyNode(vtkMrmlData *Data);

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
  virtual int Read();
  virtual int Write();

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



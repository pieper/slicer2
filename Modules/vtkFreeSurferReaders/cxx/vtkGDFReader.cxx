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
#include "vtkGDFReader.h"
#include "vtkObjectFactory.h"

vtkGDFReader* vtkGDFReader::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkGDFReader");
  if(ret)
  {
      return (vtkGDFReader*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkGDFReader;
}

vtkGDFReader::vtkGDFReader()
{
    // set up defaults
    this->Title = NULL;
    this->MeasurementName = NULL;
    this->SubjectName = NULL;
    this->DataFileName = NULL;
    this->NumClasses = 0;
    this->NumVariables = 0;
    this->DefaultVariable = NULL;
    this->DefaultVariableIndex = 0;
    this->NumberOfSubjects = 0;

    this->val = NULL;
    
}

vtkGDFReader::~vtkGDFReader()
{

}

void vtkGDFReader::ExecuteInformation()
{
    vtkDebugMacro(<< "vtkGDFReader: ExecuteInformation");
}

void vtkGDFReader::Execute()
{
    vtkDebugMacro(<< "vtkGDFReader: Execute");

}

vtkImageData *vtkGDFReader::GetImage(int ImageNumber)
{
  cerr << "vtkGDFReader::GetImage() called. uh oh." << endl;
  return NULL;
}

void vtkGDFReader::Read()
{
    vtkDebugMacro(<< "Read");
}

int vtkGDFReader::ReadHeader(char *filename, int flag)
{
  int newID = 0;
  vtkDebugMacro(<< "ReadHeader with filename " << filename << " and flag " << flag << "\n");

  return newID;
}

void vtkGDFReader::OffsetSlope()
{
    vtkDebugMacro(<< "OffsetSlope");
}

char *vtkGDFReader::GetNthClassLabel(int n)
{
    vtkDebugMacro(<< "GetNthClassLabel\n");
    return this->val;
}
char *vtkGDFReader::GetNthClassMarker(int n)
{
    vtkDebugMacro(<< "GetNthClassMarker\n");
    return this->val;
}
char *vtkGDFReader::GetNthClassColor(int n)
{
    vtkDebugMacro(<< "GetNthClassColor\n");
    return this->val;
}
char *vtkGDFReader::GetNthVariableLabel(int n)
{
    vtkDebugMacro(<< "GetNthVariableLabel\n");
    return this->val;
}
int vtkGDFReader::GetNthSubjectID(int n)
{
    vtkDebugMacro(<< "GetNthSubjectID\n");
    return 0;
}
char *vtkGDFReader::GetNthSubjectClass(int n)
{
    vtkDebugMacro(<< "GetNthSubjectClass\n");
    return this->val;
}
char *vtkGDFReader::GetNthSubjectNthValue(int n1, int n2)
{
    vtkDebugMacro(<< "GetNthSubjectNthValue\n");
    return this->val;
}
vtkFloatingPointType vtkGDFReader::GetNthSubjectMeasurement(int n)
{
    vtkFloatingPointType retval;
    vtkDebugMacro(<< "GetNthSubjectMeasurement\n");
    retval = 0.0;

    return retval;
}

void vtkGDFReader::PrintSelf(ostream& os, vtkIndent indent)
{
    if (this->Title != NULL)
    {
      os << indent << "Title: " << this->Title << endl;
    } 
    else 
    {
      os << indent << "Title:" << endl;
    }
    if (this->MeasurementName != NULL)
    {
      os << indent << "Measurement name: " << this->MeasurementName << endl;
    }
    else 
    {
      os << indent << "Measurement name:" << endl;
    }
    if (this->SubjectName != NULL)
    {
      os << indent << "Subject name: " << this->SubjectName << endl;
    } 
    else 
    {
      os << indent << "Subject name:" << endl;
    }
    if (this->DataFileName != NULL)
      {
        os << indent << "Data file name: " << this->DataFileName << endl;
      } 
    else 
    {
      os << indent << "Data file name:" << endl;
    }
    os << indent << "Number of classes: " << this->NumClasses << endl;
    os << indent << "Number of variables: " << this->NumVariables << endl;
    if (this->DefaultVariable != NULL)
      {
        os << indent << "Default variable: " << this->DefaultVariable << endl;
      } 
    else 
    {
      os << indent << "Default variable:" << endl;
    }
    os << indent << "Default variable index: " << this->DefaultVariableIndex << endl;
    os << indent << "Number of subjects: " << this->NumberOfSubjects << endl;
}

void vtkGDFReader::PrintStdout()
{
  if (this->Title != NULL)
    {
    cout << "Title: " << this->Title << endl;
    }
  else
    {
      cout << "Title:" << endl;
    }
  if (this->MeasurementName != NULL)
    {
    cout << "Measurement name: " << this->MeasurementName << endl;
    }
  else
    {
      cout << "Measurement name:" << endl;
    }
  if (this->SubjectName != NULL)
    {
      cout << "Subject name: " << this->SubjectName << endl;
    }
  else
    {
      cout << "Subject name:" << endl;
    }
    if (this->DataFileName != NULL)
      {
    cout << "Data file name: " << this->DataFileName << endl;
      }
  else
    {
      cout << "Data file name:" << endl;
    }
    cout << "Number of classes: " << this->NumClasses << endl;
    cout << "Number of variables: " << this->NumVariables << endl;
    if (this->DefaultVariable != NULL)
    {
      cout << "Default variable: " << this->DefaultVariable << endl;
    }
  else
    {
      cout << "Default variable:" << endl;
    }
    cout << "Default variable index: " << this->DefaultVariableIndex << endl;
    cout << "Number of subjects: " << this->NumberOfSubjects << endl;
}

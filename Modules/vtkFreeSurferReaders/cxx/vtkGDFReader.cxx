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

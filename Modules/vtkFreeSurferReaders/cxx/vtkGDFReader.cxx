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
    os << indent << "Title: " << this->Title << endl;
    os << indent << "Measurement name: " << this->MeasurementName << endl;
    os << indent << "Subject name: " << this->SubjectName << endl;
    os << indent << "Data file name: " << this->DataFileName << endl;
    os << indent << "Number of classes: " << this->NumClasses << endl;
    os << indent << "Number of variables: " << this->NumVariables << endl;
    os << indent << "Default variable: " << this->DefaultVariable << endl;
    os << indent << "Default variable index: " << this->DefaultVariableIndex << endl;
    os << indent << "Number of subjects: " << this->NumberOfSubjects << endl;
}

void vtkGDFReader::PrintStdout()
{
    cout << "Title: " << this->Title << endl;
    cout << "Measurement name: " << this->MeasurementName << endl;
    cout << "Subject name: " << this->SubjectName << endl;
    cout << "Data file name: " << this->DataFileName << endl;
    cout << "Number of classes: " << this->NumClasses << endl;
    cout << "Number of variables: " << this->NumVariables << endl;
    cout << "Default variable: " << this->DefaultVariable << endl;
    cout << "Default variable index: " << this->DefaultVariableIndex << endl;
    cout << "Number of subjects: " << this->NumberOfSubjects << endl;
}

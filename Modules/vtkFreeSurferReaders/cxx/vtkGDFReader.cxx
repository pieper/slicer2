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
    this->Title = "Title";
    this->MeasurementName = NULL;
    this->SubjectName = NULL;
    this->DataFileName = NULL;
    this->NumClasses = 0;
    this->NumVariables = 0;
    this->DefaultVariable = NULL;
    this->DefaultVariableIndex = 0;
    this->NumberOfSubjects = 0;
    
    this->Tessellation = NULL;
    this->RegistrationSubject = NULL;
    this->DesignMatFile = NULL;
    this->Creator = NULL;
    this->SmoothSteps = 0;
    this->SUBJECTS_DIR = NULL;
    this->SynthSeed = -1;
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

// Format of the file is specified here:
// http://surfer.nmr.mgh.harvard.edu/docs/fsgdf.txt
int vtkGDFReader::ReadHeader(char *filename, int flag)
{
  int newID = 0;
  // pointer to the header file
  FILE *fp;
  // buffer to read into from the file
  char input[1024];
  // pointer into input
  char *line;
  // pointer for parsing out substrings in line
  char *subline;

  char tempString[1024];
  
  vtkDebugMacro(<< "ReadHeader with filename " << filename << " and flag " << flag << "\n");

  // open the header file
  fp = fopen(filename, "r");
  if (!fp)
  {
      vtkErrorMacro(<<"Can't open GDF header file for reading: "<< filename);
      return newID;
  }

  // check that the first line is "GroupDescriptorFile 1"
  fgets(input, 1024, fp);
  if( input[strlen(input)-1] == '\n' ) {
      input[strlen(input)-1] = '\0';
  }
  line = input;
  while( isspace( (int)(*line) ) ) {
      line++;
  }
  if (strlen(line) > 0 && strncasecmp(line, "GroupDescriptorFile 1", 21) == 0)
  {
      // have a valid file
  }
  else
  {
      vtkErrorMacro(<<"ERROR: first line of file " << filename << " is not 'GroupDescriptorFile 1'\n");
      return newID;
  }

  this->NumClasses = 0;
  this->NumVariables = 0;
  this->NumberOfSubjects = 0;

  this->ClassesVec.clear();
  this->VariablesVec.clear();
  this->SubjectsVec.clear();
  
  // now read the values from the header and save them
  while (!feof(fp))
  {
      // Get a line. Strip newline and skip initial spaces.
      fgets( input, 1024, fp );
      if( input[strlen(input)-1] == '\n' ) {
          input[strlen(input)-1] = '\0';
      }
      line = input;
      while( isspace( (int)(*line) ) ) {
          line++;
      }
      // now parse the lines in the files for the header values
      if (strlen(line) > 0 && strncmp(line, "#", 1) != 0)
      {
          vtkDebugMacro(<<"Parsing line " << line << endl);
          if (strncasecmp(line, "Title", 5) == 0)
          {
              this->Title = (char *)malloc((strlen(line)) * sizeof(char));
              sscanf(line, "%*s %s", this->Title);
          }
          else if (strncasecmp(line, "Class", 5) == 0)
          {
              // this is a bit complex, as we may be defining lots of classes
              this->NumClasses++;
              // the class name is required, but there may be a marker and a
              // colour after it
          }
          else if (strncasecmp(line, "MeasurementName", 15) == 0)
          {
              this->MeasurementName = (char *)malloc((strlen(line)) * sizeof(char));
              sscanf(line, "%*s %s", this->MeasurementName);
          }
          else if (strncasecmp(line, "SubjectName", 11) == 0)
          {
              this->SubjectName = (char *)malloc((strlen(line)) * sizeof(char));
              sscanf(line, "%*s %s", this->SubjectName);
          }
          else if (strncasecmp(line, "DataFileName", 12) == 0)
          {
              this->DataFileName = (char *)malloc((strlen(line)) * sizeof(char));
              sscanf(line,  "%*s %s", this->DataFileName);
          }
          else if (strncasecmp(line, "Variables", 9) == 0)
          {
              // this is a bit complex, can have a list of variable names
              // after it
              // this call will get Variables, next calls will get the list element
              subline = strtok(line, " ");
              while ((subline = strtok(NULL, " ")) != NULL)
              {
                  cerr << "Variables:\n\t line: " << line << "\n\t subline: "<< subline << endl;
                  sscanf(subline, "%s", tempString);
                  if (tempString != NULL)
                  {
                      this->VariablesVec.push_back(tempString);
                      cerr << "Variables, got val, tempString = " << tempString << ", variables vec size = " << this->VariablesVec.size() << ", subline = " << subline << endl;
                      //cerr << "Variables, got val " << this->VariablesVec[this->NumVariables] << ", variables vec size = " << this->VariablesVec.size() << ", subline = " << subline << endl;
                      this->NumVariables++;
                  }
                  else
                  {
                      cerr << "Variables, value is null!\n";
                  }
              }
          }
          else if (strncasecmp(line, "DefaultVariable", 15) == 0)
          {
              this->DefaultVariable = (char *)malloc((strlen(line)) * sizeof(char));
              sscanf(line, "%*s %s", this->DefaultVariable);
          }
          else if (strncasecmp(line, "Input", 5) == 0)
          {
              // complex, this is one of many subjects
              // Input subjectid class var1val var1val .. varnval
              this->NumberOfSubjects++;
          }
          else if (strncasecmp(line, "Tessellation", 12) == 0)
          {
              this->Tessellation = (char *)malloc((strlen(line)) * sizeof(char));
              sscanf(line, "%*s %s", this->Tessellation);
          }
          else if (strncasecmp(line, "RegistrationSubject", 19) == 0)
          {
              this->RegistrationSubject = (char *)malloc((strlen(line)) * sizeof(char));
              sscanf(line, "%*s %s", this->RegistrationSubject);
          }
          else if (strncasecmp(line, "PlotFile", 8) == 0)
          {
              this->DataFileName = (char *)malloc((strlen(line)) * sizeof(char));
              // use this as a synonym for the data file name
              sscanf(line, "%*s %s", this->DataFileName);
          }
          else if (strncasecmp(line, "DesignMatFile", 13) == 0)
          {
              this->DesignMatFile = (char *)malloc((strlen(line)) * sizeof(char));
              sscanf(line, "%*s %s", this->DesignMatFile);
          }
          else if (strncasecmp(line, "Creator", 7) == 0)
          {
              this->Creator = (char *)malloc((strlen(line)) * sizeof(char));
              sscanf(line, "%*s %s", this->Creator);
          }
          else if (strncasecmp(line, "SmoothSteps", 11) == 0)
          {
              sscanf(line, "%*s %d", &this->SmoothSteps);
          }
          else if (strncasecmp(line, "SUBJECTS_DIR", 12) == 0)
          {
              this->SUBJECTS_DIR = (char *)malloc((strlen(line)) * sizeof(char));
              sscanf(line, "%*s %s", this->SUBJECTS_DIR);
          }
          else if (strncasecmp(line, "SynthSeed", 9) == 0)
          {
              sscanf(line, "%*s %d", &this->SynthSeed);
          }
          else
          {
              vtkWarningMacro(<<"Warning: unknown tag in line, skipping:\n" << line);
          }
      }
  }
  // cheating for a bit, allocate data structures and then rewind the file to fill in the data
  fclose(fp);

  if (this->DefaultVariable == NULL && this->NumVariables > 0)
  {
      this->DefaultVariable = this->VariablesVec[0];
  }
  cerr << "Done reading header file\n";
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

    if (this->Tessellation != NULL)
        os << indent << "Tessellation: " << this->Tessellation << endl;
    else
        os << indent << "Tessellation: " << endl;
    if (this->RegistrationSubject != NULL)
        os << indent << "RegistrationSubject: " << this->RegistrationSubject << endl;
    else
        os << indent << "RegistrationSubject: " << endl;
        
    if (this->DesignMatFile != NULL)
        os << indent << "DesignMatFile: " << this->DesignMatFile << endl;
    else
        os << indent << "DesignMatFile: " << endl;
    
    if (this->Creator != NULL)
        os << indent << "Creator: " << this->Creator << endl;
    else
        os << indent << "Creator: " << endl;

    os << indent << "SmoothSteps: " << this->SmoothSteps << endl;
    if (this->SUBJECTS_DIR != NULL)
        os << indent << "SUBJECTS_DIR: " << this->SUBJECTS_DIR << endl;
    else
        os << indent << "SUBJECTS_DIR: " << endl;
    os << indent << "SynthSeed: " << this->SynthSeed << endl;
    
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

    if (this->Tessellation != NULL)
        cout << "Tessellation: " << this->Tessellation << endl;
    else
        cout << "Tessellation: " << endl;
    if (this->RegistrationSubject != NULL)
        cout << "RegistrationSubject: " << this->RegistrationSubject << endl;
    else
        cout << "RegistrationSubject: " << endl;
    
    if (this->DesignMatFile != NULL)
        cout << "DesignMatFile: " << this->DesignMatFile << endl;
    else
        cout << "DesignMatFile: " << endl;
    
    if (this->Creator != NULL)
        cout << "Creator: " << this->Creator << endl;
    else
        cout << "Creator: " << endl;

    cout << "SmoothSteps: " << this->SmoothSteps << endl;
    if (this->SUBJECTS_DIR != NULL)
        cout << "SUBJECTS_DIR: " << this->SUBJECTS_DIR << endl;
    else
        cout << "SUBJECTS_DIR: " << endl;
    cout << "SynthSeed: " << this->SynthSeed << endl;
    
}

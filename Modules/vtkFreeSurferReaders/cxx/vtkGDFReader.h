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
// .NAME vtkGDF.h - GDF functions for FreeSurfer statistical files.
// .SECTION Description
//
//

#ifndef __vtkGDFReader_h
#define __vtkGDFReader_h

#include <vtkFreeSurferReadersConfigure.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string>
#include "vtkImageData.h"
#include "vtkVolumeReader.h"

#ifdef _WIN32
#include <vector>
#define strncasecmp strnicmp
#else
#include <vector.h>
#endif
using namespace std;

class VTK_FREESURFERREADERS_EXPORT vtkGDFReader : public vtkVolumeReader {
public:

    vtkTypeMacro(vtkGDFReader,vtkVolumeReader);
    
    static vtkGDFReader *New ();
    
    void PrintSelf(ostream& os, vtkIndent indent);
    
    // Description: 
    // Other objects make use of these methods but we don't. Left here
    // but not implemented.
    vtkImageData *GetImage(int ImageNumber);
    
    // Description:
    // Read in a header file
    void Read();

    // Description:
    // title of the graph
    vtkSetStringMacro(Title);
    vtkGetStringMacro(Title);

    // Description:
    // measurement label
    vtkSetStringMacro(MeasurementName);
    vtkGetStringMacro(MeasurementName);

    // Description:
    // subject name
    vtkSetStringMacro(SubjectName);
    vtkGetStringMacro(SubjectName);

    // Description:
    // data file name
    vtkSetStringMacro(DataFileName);
    vtkGetStringMacro(DataFileName);

    // Description:
    // number of classes
    vtkGetMacro(NumClasses,int);
    vtkSetMacro(NumClasses,int);
    
    // Description:
    // number of variables
    vtkGetMacro(NumVariables,int);
    vtkSetMacro(NumVariables,int);
    
    // Description:
    // default variable
    vtkGetStringMacro(DefaultVariable);
    vtkSetStringMacro(DefaultVariable);
    
    // Description:
    // 
    vtkGetMacro(DefaultVariableIndex,int);
    vtkSetMacro(DefaultVariableIndex,int);
    
    // Description:
    // number of subjects
    vtkGetMacro(NumberOfSubjects,int);
    vtkSetMacro(NumberOfSubjects,int);


    // Description:
    // type of tesselation
    vtkGetStringMacro(Tessellation);
    vtkSetStringMacro(Tessellation);

    // Description:
    // the averaged registration subject file name
    vtkGetStringMacro(RegistrationSubject);
    vtkSetStringMacro(RegistrationSubject);

    // Description:
    // the file that contains the design matrix
    vtkGetStringMacro(DesignMatFile);
    vtkSetStringMacro(DesignMatFile);
    // Description:
    // who created this descriptor file
    vtkGetStringMacro(Creator);
    vtkSetStringMacro(Creator);

    // Description:
    // number of smoothing steps
    vtkGetMacro(SmoothSteps, int);
    vtkSetMacro(SmoothSteps, int);
    
    // Description:
    // the path to the subjects directory
    vtkGetStringMacro(SUBJECTS_DIR);
    vtkSetStringMacro(SUBJECTS_DIR);

    vtkGetStringMacro(ErrVal);
    
    // Description:
    // seed for synthetic generation
    vtkGetMacro(SynthSeed, int);
    vtkSetMacro(SynthSeed, int);

    // Description:
    // descriptor file to design matrix conversion method
    vtkGetStringMacro(gd2mtx);
   
    char *GetNthClassLabel(int n);
    char *GetNthClassMarker(int n);
    char *GetNthClassColor(int n);
    char *GetNthVariableLabel(int n);
    char *GetNthSubjectID(int n);
    char *GetNthSubjectClass(int n);
    char *GetNthSubjectNthValue(int n1, int n2);
    vtkFloatingPointType GetNthSubjectMeasurement(int n, char *subject, int x, int y, int z);
    
    // Description:
    //
    void OffsetSlope(char *c, char *v, int x, int y, int z);

    // Description:
    // prints out to standard out
    void PrintStdout();

    int ReadHeader(char *filename, int flag);

protected:
    vtkGDFReader();
    ~vtkGDFReader();

    void Execute();
    void ExecuteInformation();


    // title of the graph
    char *Title;

    // measurement label
    char * MeasurementName;

    // subject name
    char * SubjectName;

    // data file name
    char * DataFileName;

    // number of classes
    int NumClasses;

    // numberof variables
    int NumVariables;

    // default variable
    char * DefaultVariable;

    // default variable index
    int DefaultVariableIndex;

    // number of subjects
    int NumberOfSubjects;

    char * Tessellation;
    char * RegistrationSubject;
    char * DesignMatFile;
    char * Creator;
    int SmoothSteps;
    char * SUBJECTS_DIR;
    int SynthSeed;

    // for testing
    char * ErrVal;

    // the method by which the group descriptor file is converted to a design
    // matrix (different offset same slope ; different offset different slope
    // legal values: doss dods 
    char * gd2mtx;
    
private:
   // to keep track of an increasing id for sucessive allocations
    int lastID;
    
//BTX
    string defaultMarker;
    string defaultColour;

    // use a vector to hold the class name, marker, colour (last two optional)
    std::vector<std::vector<std::string> >ClassesVec;
    //std::vector<char *>VariablesVec;
    std::vector<std::string>VariablesVec;
    // use a vector to hold the subject id, class, and values of a number that
    // matches NumVariables
    std::vector<std::vector<std::string> >SubjectsVec;
//ETX
};

#endif

// .NAME vtkGDF.h - GDF functions for FreeSurfer statistical files.
// .SECTION Description
//
//

#ifndef __vtkGDFReader_h
#define __vtkGDFReader_h

#include <vtkFreeSurferReadersConfigure.h>
#include <stdio.h>
#include "vtkImageData.h"
#include "vtkVolumeReader.h"

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


    char *GetNthClassLabel(int n);
    char *GetNthClassMarker(int n);
    char *GetNthClassColor(int n);
    char *GetNthVariableLabel(int n);
    int GetNthSubjectID(int n);
    char *GetNthSubjectClass(int n);
    char *GetNthSubjectNthValue(int n1, int n2);
    vtkFloatingPointType GetNthSubjectMeasurement(int n);
    
    // Description:
    //
    void OffsetSlope();

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
    char *MeasurementName;

    // subject name
    char *SubjectName;

    // data file name
    char *DataFileName;

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

    // for testing
    char * val;
};

#endif

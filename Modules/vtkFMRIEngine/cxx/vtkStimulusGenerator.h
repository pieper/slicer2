/*=auto=========================================================================

(c) Copyright 2004 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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

// .NAME vtkStimulusGenerator - Creates stimulus from file   
// .SECTION Description
// vtkStimulusGenerator is used to generate a stimulus pattern from the text file 
// that describes the experimental paradigm design. The stimulus is returned as 
// a vtkFloatArray.


#ifndef __vtkStimulusGenerator_h
#define __vtkStimulusGenerator_h


#include <vtkFMRIEngineConfigure.h>
#include "vtkFloatArray.h"
#include "vtkObject.h"


class VTK_FMRIENGINE_EXPORT vtkStimulusGenerator : public vtkObject
{
public:
    static vtkStimulusGenerator *New();
    vtkTypeMacro(vtkStimulusGenerator, vtkObject);
    void PrintSelf(ostream& os, vtkIndent indent);

    vtkStimulusGenerator();
    ~vtkStimulusGenerator();

    // Description:
    // Creates a stimulus from the paradigm design file. The stimulus is 
    // returned as a vtkFloatArray.
    vtkFloatArray *CreateStimulusFromFile();

    // Description:
    // Retrieves the stimulus.
    vtkFloatArray *GetStimulus();

    // Description:
    // Gets the number of predictors (parameters).
    vtkGetMacro(NumberOfPredictors, int);

    // Description:
    // Specifies the file name of paradigm data file to read.
    vtkSetStringMacro(FileName);

    // Description:
    // Get file name of paradigm data file to read.
    vtkGetStringMacro(FileName);

private:
    char *FileName;
    vtkFloatArray *StimulusArray;
    int NumberOfPredictors;
    int TotalVolumesPerRun; // number of time points
    int VolumesPerBaseline;
    int VolumesPerTask;
    int VolumesAtStart;
    bool Start;  // false if the experiment starts with baseline; true otherwise (i.e. task)

    int ParseParadigmFile();
};


#endif

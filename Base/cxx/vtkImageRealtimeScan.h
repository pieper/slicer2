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
// .NAME vtkImageRealtimeScan - Get a realtime image from the scanner.
// .SECTION
// vtkImageRealtimeScan just produces images filled with noise.  

#ifndef __vtkImageRealtimeScan_h
#define __vtkImageRealtimeScan_h

#include "vtkImageData.h"
#include "vtkImageSource.h"
#include "vtkMatrix4x4.h"
#include "vtkSlicer.h"

#define LOC_OK 0
#define LOC_NO 1

#define OFFSET_NBYTES      0
#define    LEN_NBYTES      4

#define OFFSET_LOC_NEW     0
#define    LEN_LOC_NEW     2
#define OFFSET_IMG_NEW     OFFSET_LOC_NEW + LEN_LOC_NEW
#define    LEN_IMG_NEW     2
#define OFFSET_LOC_STATUS  OFFSET_IMG_NEW + LEN_IMG_NEW
#define    LEN_LOC_STATUS  2
#define OFFSET_LOC_MATRIX  OFFSET_LOC_STATUS + LEN_LOC_STATUS
#define    LEN_LOC_MATRIX 64

#define OFFSET_IMG_TBLPOS   0
#define    LEN_IMG_TBLPOS   2
#define OFFSET_IMG_PATPOS   OFFSET_IMG_TBLPOS + LEN_IMG_TBLPOS
#define    LEN_IMG_PATPOS   2
#define OFFSET_IMG_IMANUM   OFFSET_IMG_PATPOS + LEN_IMG_PATPOS
#define    LEN_IMG_IMANUM   4 
#define OFFSET_IMG_RECON    OFFSET_IMG_IMANUM + LEN_IMG_IMANUM
#define    LEN_IMG_RECON    4 
#define OFFSET_IMG_MINPIX   OFFSET_IMG_RECON + LEN_IMG_RECON
#define    LEN_IMG_MINPIX   2
#define OFFSET_IMG_MAXPIX   OFFSET_IMG_MINPIX + LEN_IMG_MINPIX
#define    LEN_IMG_MAXPIX   2
#define OFFSET_IMG_DIM      OFFSET_IMG_MAXPIX + LEN_IMG_MAXPIX
#define    LEN_IMG_DIM      6
#define OFFSET_IMG_SPACING  OFFSET_IMG_DIM + LEN_IMG_DIM
#define    LEN_IMG_SPACING 12
#define OFFSET_IMG_MATRIX   OFFSET_IMG_SPACING + LEN_IMG_SPACING
#define    LEN_IMG_MATRIX  64

#define CMD_CLOSE  0 
#define CMD_PING   1
#define CMD_UPDATE 2
#define CMD_HEADER 3
#define CMD_PIXELS 4
#define CMD_POS    5

class VTK_SLICER_BASE_EXPORT vtkImageRealtimeScan : public vtkImageSource 
{
public:
    static vtkImageRealtimeScan *New();
  vtkTypeMacro(vtkImageRealtimeScan,vtkImageSource);
    void PrintSelf(ostream& os, vtkIndent indent);

    void ExecuteInformation();
  
    vtkGetObjectMacro(LocatorMatrix,vtkMatrix4x4);
    vtkGetObjectMacro(ImageMatrix,vtkMatrix4x4);

    int OpenConnection(char *name, int port);
    int CheckConnection();
    int PollRealtime();
    void CloseConnection();
    void SetRefreshImage(int refresh);
    int GetRefreshImage() {return RefreshImage;}

    vtkGetMacro(LocatorStatus, int);
    vtkGetMacro(NewLocator, short);
    vtkGetMacro(NewImage, short);
    vtkGetMacro(PatientPosition, short);
    vtkGetMacro(PatientEntry, short);
    vtkGetMacro(TablePosition, short);
    vtkGetMacro(MinValue, short);
    vtkGetMacro(MaxValue, short);
    vtkGetMacro(Recon, long);
    vtkGetMacro(ImageNum, long);
    
    vtkGetMacro(Test, int);
    vtkSetMacro(Test, int);

    vtkSetStringMacro(TestPrefix);

    int SetPosition(short tblPos, short patEntry, short patPos);

protected:
    vtkImageRealtimeScan();
    ~vtkImageRealtimeScan();
    long SendServer(int cmd);
    void Execute(vtkImageData *data);

    /* Modified is automatically set for NewImage, but not NewLocator. */

    short RefreshImage;
    short NewImage;
    short NewLocator;
    short LocatorStatus;
    short TablePosition;
    short PatientPosition;
    short PatientEntry;
    short MinValue;
    short MaxValue;
    long ImageNum;
    long Recon;
    int Test;
    int sockfd;
    char *TestPrefix;
    vtkMatrix4x4 *LocatorMatrix;
    vtkMatrix4x4 *ImageMatrix;
};


#endif

  

/*=auto=========================================================================
Copyright (c) 2000 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy (but NOT distribute) this
software and its documentation for any NON-COMMERCIAL purpose, provided
that existing copyright notices are retained verbatim in all copies.
The authors grant permission to modify this software and its documentation 
for any NON-COMMERCIAL purpose, provided that such modifications are not 
distributed without the explicit consent of the authors and that existing
copyright notices are retained in all copies. Some of the algorithms
implemented by this software are patented, observe all applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
=========================================================================auto=*/
// .NAME vtkImageSphereSource - Create a binary image of a sphere.
// .SECTION
// vtkImageRealtimeScan just produces images filled with noise.  

#ifndef __vtkImageRealtimeScan_h
#define __vtkImageRealtimeScan_h

#include "vtkImageSource.h"
#include "vtkMatrix4x4.h"

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

class VTK_EXPORT vtkImageRealtimeScan : public vtkImageSource 
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

  

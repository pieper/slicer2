/*=auto=========================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy, and distribute this
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memcpy
#include <iostream.h>
#include <string.h>
#include <fcntl.h>
#define NOREALTIME
#ifndef NOREALTIME
#include <strings.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#include "vtkImageRealtimeScan.h"
#include "vtkObjectFactory.h"

static int Read16BitImage (char *filePrefix, char *filePattern, 
	int start, int end, int nx, int ny, int skip, int SwapBytes, 
	char *fileName, short *image);


//----------------------------------------------------------------------------
vtkImageRealtimeScan::vtkImageRealtimeScan()
{
	RefreshImage = 0;
	NewImage = 0;
	NewLocator = 0;
	LocatorStatus = LOC_NO;
	Test = 0;
	TestPrefix = NULL;
	LocatorMatrix = vtkMatrix4x4::New(); // Identity
	ImageMatrix   = vtkMatrix4x4::New();
	sockfd = -1;
	PatientPosition = PatientEntry = TablePosition = 0;
	MinValue = MaxValue = 0;
	Recon = 0;
	ImageNum = 0;
}

//----------------------------------------------------------------------------
vtkImageRealtimeScan* vtkImageRealtimeScan::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageRealtimeScan");
  if(ret)
    {
    return (vtkImageRealtimeScan*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageRealtimeScan;
}

vtkImageRealtimeScan::~vtkImageRealtimeScan()
{
	if (this->TestPrefix) delete [] this->TestPrefix;
	LocatorMatrix->Delete();
	ImageMatrix->Delete();
}

#ifndef NOREALTIME

/******************************************************************************
readn

This procedure reads from a socket according to page 279 of "UNIX Network
Programming" by W. Ricard Stevens, 1990.

PARAMETERS:
        fd     -- file descriptor for socket to read from
        ptr    -- pointer to buffer to read into
        nbytes -- number of bytes to read
RETURNS:
        number of bytes read on sucess, else negative on error
******************************************************************************/
static long readn(int fd, char *ptr, long nbytes)
{
	long nleft, nread;

	nleft = nbytes;

	while (nleft> 0)
	{
		nread = read(fd, ptr, nleft);

		/* check for error */
		if (nread < 0)
			return nread;

		/* check for EOF */
		else if (nread == 0)
			break;

		nleft -= nread;
		ptr  += nread;
	}
	*ptr = 0;
	return nbytes - nleft;
}

/******************************************************************************
writen

This procedure writes to a socket according to page 279 of "UNIX Network
Programming" by W. Ricard Stevens, 1990.

PARAMETERS:
        fd     -- file descriptor for socket to read from
        ptr    -- pointer to buffer to read into
        nbytes -- number of bytes to write
RETURNS:
        number of bytes written on sucess, else negative on error
******************************************************************************/
static long writen(int fd, char *ptr, long nbytes)
{
	long nleft, nwritten;

	nleft = nbytes;

	while (nleft > 0)
	{
		nwritten = write(fd, ptr, nleft);

		/* Check for error */
		if (nwritten <= 0)
			return nwritten;

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return nbytes - nleft;
}

#endif

/******************************************************************************
SendServer

Sends command 'cmd' to the server.
******************************************************************************/
long vtkImageRealtimeScan::SendServer(int cmd)
{
	long len, n, nbytes=0;
	char buf[LEN_NBYTES];

	if (Test) return 0;

	// Return if not connected yet
	if (sockfd < 0) return -1;

#ifndef NOREALTIME
	
	sprintf(buf, "%d", cmd);
	len = strlen(buf);
	n = writen(sockfd, buf, len);
	if (n < len) {
		// This happens when the server crashes.
		fprintf(stderr, "Client wrote %d instead of %d bytes.\n",n,len);
		close(sockfd);
		return -1;
	}

	len = LEN_NBYTES;
	n = readn(sockfd, buf, len);
	if (n < 0) {
		fprintf(stderr, "Client: read error.\n");
		close(sockfd);
		return -1;
	}
	bcopy(&buf[OFFSET_NBYTES], &nbytes, LEN_NBYTES);

#endif

	return nbytes;
}

/******************************************************************************
SetPosition

Sends the table and patient positions to the server.  The server will then
use these when transforming locator and image points from XYZ to RAS
coordinate systems.
******************************************************************************/
int vtkImageRealtimeScan::SetPosition(short tblPos, short patEntry, 
									  short patPos)
{
	long len, n, nbytes=0;
	char buf[LEN_NBYTES];

	if (Test) return 0;

	// Return if not connected yet
	if (sockfd < 0) return -1;

#ifndef NOREALTIME

	// Send command number	
	sprintf(buf, "%d", CMD_POS);
	len = strlen(buf);
	n = writen(sockfd, buf, len);
	if (n < len) {
		// This happens when the server crashes.
		vtkErrorMacro(<< "Client: wrote " << n << " instead of " << len <<
			" bytes.");
		close(sockfd);
		return -1;
	}

	// Encode patEntry and patPos into patPos
	// tblPos:
	//  0 = front
	//  1 = side
	// patPos:
	//  0 = head-first, supine
	//  1 = head-first, prone
	//  2 = head-first, left-decub
	//  3 = head-first, right-decub
	//  4 = feet-first, supine
	//  5 = feet-first, prone
	//  6 = feet-first, left-decub
	//  7 = feet-first, right-decub
	patPos = patPos + patEntry * 4;

	// Send data:
	bcopy(&tblPos, &buf[OFFSET_IMG_TBLPOS], LEN_IMG_TBLPOS);
	bcopy(&patPos, &buf[OFFSET_IMG_PATPOS], LEN_IMG_PATPOS);
	len = LEN_IMG_TBLPOS + LEN_IMG_PATPOS;
	n = writen(sockfd, buf, len);
	if(n != len) {
		vtkErrorMacro(<< "Client: data write error\n"); 
		return -1;
	}

	// Update member variables when transfer successful
	this->TablePosition = tblPos;
	this->PatientPosition = patPos % 4;
 	this->PatientEntry = patPos / 4;

	// Read server's reply
	len = LEN_NBYTES;
	n = readn(sockfd, buf, len);
	if (n < 0) {
		vtkErrorMacro(<< "Client: connection stinks!");
		close(sockfd);
		return -1;
	}
	bcopy(&buf[OFFSET_NBYTES], &nbytes, LEN_NBYTES);

#endif

	return nbytes;
}

/******************************************************************************
OpenConnection
******************************************************************************/
int vtkImageRealtimeScan::OpenConnection(char *hostname, int port)
{
#ifndef NOREALTIME
	struct sockaddr_in serv_addr;
	struct hostent *hostptr;
#endif
	int len, n;
	char buf[100];

	// If already connected, then just verify the connection
	if (sockfd >= 0)
		return CheckConnection();

	if (Test) {
		sockfd = 1;
		return CheckConnection();
	}

#ifndef NOREALTIME

	if((hostptr = gethostbyname(hostname)) == NULL) {
		fprintf(stderr,"Bad hostname: [%s]\n",hostname);
		return -1;
	}
  
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port   = port;

	bcopy(hostptr->h_addr, (char *)&serv_addr.sin_addr, hostptr->h_length);

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Socket allocation failed.\n");
		return -1;
	}

	if (connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
		fprintf(stderr, "Cannot connect to '%s'.\n", hostname);
		close(sockfd);
		return -1;
	}

#endif

	return CheckConnection();
}

/******************************************************************************
CheckConnection
******************************************************************************/
int vtkImageRealtimeScan::CheckConnection()
{
	// Check Client/Server connection with one ping.
	if (SendServer(CMD_PING) < 0) {
		vtkErrorMacro(<< "Client: connection stinks!");
		return -1;
	}
	return 0;
}

/******************************************************************************
CloseConnection
******************************************************************************/
void vtkImageRealtimeScan::CloseConnection()
{
	if (sockfd < 0) return;

	SendServer(CMD_CLOSE);
#ifndef NOREALTIME
	close(sockfd);
#endif
	sockfd = -1;
}

/******************************************************************************
SetRefreshImage
******************************************************************************/
void vtkImageRealtimeScan::SetRefreshImage(int refresh)
{
	if (refresh == 0)
	{
		// I don't want it to Execute and fetch an image when I'm turning
		// refresh off.
		RefreshImage = refresh;
	}
	else
	{   
		// No Change
		if (RefreshImage != 0)
			return;
 
		// Start refreshing, and also fetch the previous image
		RefreshImage = refresh;
		this->Modified();
	}
}

/******************************************************************************
PollRealtime
******************************************************************************/
int vtkImageRealtimeScan::PollRealtime()
{
	long len, n, nbytes;
	static char buf[200];
	float matrix[16];
	int i,j;
	
#ifndef NOREALTIME

	// Request the update info
	nbytes = SendServer(CMD_UPDATE);
	if (nbytes < 0) return -1;

	// Read the update info
	n = readn(sockfd, buf, nbytes);
	if (n < 0) {
		vtkErrorMacro(<< "Client: read error.");
		close(sockfd);
		sockfd = -1;
		return -1;
	}

	// Parse the update info
	bcopy(&buf[OFFSET_LOC_NEW], &NewLocator, LEN_LOC_NEW);
	bcopy(&buf[OFFSET_IMG_NEW], &NewImage,   LEN_IMG_NEW);

	// Read locator info if it exists
	if (NewLocator) {
		bcopy(&buf[OFFSET_LOC_STATUS], &LocatorStatus, LEN_LOC_STATUS);
		bcopy(&buf[OFFSET_LOC_MATRIX], matrix, LEN_LOC_MATRIX);
		
		for (i=0; i<4; i++) {
			for (j=0; j<4; j++) {
				LocatorMatrix->SetElement(i,j,matrix[i*4+j]);
			}
		}
	}

#else
	NewLocator = 0;
	NewImage = 0;
#endif

	// Read image info if it exists
	if (NewImage && RefreshImage) {
		// Force Execute() to run on the next call to Update()
		this->Modified();  
	}
	return 0;
}

//----------------------------------------------------------------------------
void vtkImageRealtimeScan::UpdateInformation()
{
	float spacing[3];
	short dim[3];
	int i, j, ext[6];
	long n, nbytes;
	static char buf[200];
	float matrix[16];
	
	// Request header info
	if (!Test && sockfd >= 0) {
#ifndef NOREALTIME
		nbytes = SendServer(CMD_HEADER);
		if (nbytes < 0) return;
		n = readn(sockfd, buf, nbytes);
		if (n < 0) {
			vtkErrorMacro(<< "Client: read " << n << " bytes instead of " 
				<< nbytes);
			close(sockfd);
			return;
		}
#endif
	}

	// Dimensions must be fixed in order to not need reformatting
	dim[0] = 256;
	dim[1] = 256;
	dim[2] = 1;

	// Read header info 
	if (Test || sockfd < 0) 
	{
		spacing[0] = 0.9375;
		spacing[1] = 0.9375;
		spacing[2] = 1.5;
	}
	else
	{
#ifndef NOREALTIME
		short patPos;
		bcopy(&buf[OFFSET_IMG_TBLPOS],  &(this->TablePosition), LEN_IMG_TBLPOS);
		bcopy(&buf[OFFSET_IMG_PATPOS],  &patPos, LEN_IMG_PATPOS);
		bcopy(&buf[OFFSET_IMG_IMANUM],  &(this->ImageNum), LEN_IMG_IMANUM);
		bcopy(&buf[OFFSET_IMG_RECON],   &(this->Recon),    LEN_IMG_RECON);
		bcopy(&buf[OFFSET_IMG_MINPIX],  &(this->MinValue), LEN_IMG_MINPIX);
		bcopy(&buf[OFFSET_IMG_MAXPIX],  &(this->MaxValue), LEN_IMG_MAXPIX);
		bcopy(&buf[OFFSET_IMG_DIM    ], dim    , LEN_IMG_DIM);
		bcopy(&buf[OFFSET_IMG_SPACING], spacing, LEN_IMG_SPACING);
		bcopy(&buf[OFFSET_IMG_MATRIX ], matrix,  LEN_IMG_MATRIX);	
	
		// Decode patPos into a position and entry value
		this->PatientPosition = patPos % 4;
		this->PatientEntry = patPos / 4;
		
		if (dim[0] != 256 || dim[1] != 256 || dim[2] != 1)
		{
			vtkErrorMacro(<< "Image dimensions are " << dim[0] << "x" <<
				dim[1] << "x" << dim[2] << " instead of 256x256x1.");
			return;
		}
 
        /* Matrix transforms the scanner's coordinate frame to that of
        the image. The image frame has axis vectors Ux, Uy, Uz
        and translation T.  The final matrix looks like:
    
        Ux(r) Uy(r) Uz(r) T(r)
        Ux(a) Uy(a) Uz(a) T(a)
        Ux(s) Uy(s) Uz(s) T(s)
          0     0     0    1
        */
		for (i=0; i<4; i++) {
			for (j=0; j<4; j++) {
				ImageMatrix->SetElement(i,j,matrix[i*4+j]);
			}
		}
#endif
	}

	// Set output parameters

	ext[0] = ext[2] = ext[4] = 0;
	ext[1] = dim[0] - 1;
	ext[3] = dim[1] - 1; 
	ext[5] = dim[2] - 1; 

  // normal vtk crap for the UpdateInformation procedure
 
  vtkImageData *output = this->GetOutput();
  unsigned long mem;

  output->SetWholeExtent(ext);
  output->SetScalarType(VTK_SHORT);
  output->SetNumberOfScalarComponents(1);
  output->SetSpacing(spacing);
}

void vtkImageRealtimeScan::Execute(vtkImageData *data)
{
	long n, nbytes, numPoints;
	short *image, *outPtr;
	int idxR, idxY, idxZ, ny, nx, outIncX, outIncY, outIncZ;
	int rowLength, errcode;
	int *outExt;
	char *img;
 	char fileName[1000];
 
	if (data->GetScalarType() != VTK_SHORT)
	{
		vtkErrorMacro("Execute: This source only outputs shorts");
	}
	outExt = data->GetExtent();
	nx = outExt[3]-outExt[2]+1;
	ny = outExt[1]-outExt[0]+1;
	numPoints = nx*ny*(outExt[5]-outExt[4]+1);
	outPtr = (short *) data->GetScalarPointer(outExt[0],outExt[2],outExt[4]);

	if (Test)
	{
		if (this->TestPrefix == NULL)
			this->SetTestPrefix("I");
		errcode = Read16BitImage(this->TestPrefix, "%s.%.3d", 1, 1, 
			256, 256, 7904, 0, fileName, outPtr);
		if (errcode) {
			switch (errcode) {
			case 1:  vtkErrorMacro(<< "Open '" << fileName << "'"); break;
			case 2:  vtkErrorMacro(<< "Read '" << fileName << "'"); break;
			default: vtkErrorMacro(<< "???: '" << fileName << "'");
			}
			return;
		}
	}
	else {
#ifndef NOREALTIME
		nbytes = SendServer(CMD_PIXELS);
		if (nbytes < 0) return;
		if (nbytes != numPoints * sizeof(short)) {
			vtkErrorMacro(<< "Pixel data is " << nbytes << " bytes instead of "
				<< numPoints*sizeof(short));
			return;
		}

		img = new char[nbytes];
		n = readn(sockfd, img, nbytes);
		if (n < 0) {
			vtkErrorMacro(<< "Client: read error.");
			close(sockfd);
			return;
		}
			
		memcpy(outPtr, img, nbytes);
		fprintf(stderr, "New image, ctr pix = %d\n", outPtr[ny/2*nx/2]);
		delete [] img;
#endif
	}
}

void vtkImageRealtimeScan::PrintSelf(ostream& os, vtkIndent indent)
{
	vtkImageSource::PrintSelf(os,indent);

	os << indent << "LocatorStatus: " << LocatorStatus<< "\n";
}


// errcode:
//  0 success, 1 can't open file, 2 can't read file
static int Read16BitImage (char *filePrefix, char *filePattern, 
	int start, int end, int nx, int ny, int skip, int swapBytes,
	char *fileName, short *image)
{
	FILE *fp;
	int z, nz = end-start+1;
	long i, nxy = nx*ny, nxyz = nxy*nz;
	short *ptr;

	for (z=start; z <= end; z++)
	{
		sprintf(fileName, filePattern, filePrefix, z);
		fp = fopen(fileName, "rb");
		if (fp == NULL)
			return 1;
		if (skip)
			fseek(fp, skip, 0);

		ptr = &image[nxy*(z-start)+nx*(ny-1)];

		for (int j=0; j < ny; j++, ptr -= nx)
		{
			if ( !fread(ptr, sizeof(short), nx, fp) ) {
				return 2;
			}
		}
		fclose(fp);
	}

	// Binary data needs it bytes put in reverse order
	// if written on a Big Endian machine and read on a
	// Little Endian machine, or visa-versa.
	if (swapBytes) 
	{
		unsigned char *bytes = (unsigned char *) image;
		unsigned char tmp;
		for (i = 0; i < nxyz; i++, bytes += 2) 
		{
			tmp = *bytes; 
			*bytes = *(bytes + 1); 
			*(bytes + 1) = tmp;
		}
	}
	return 0;
}





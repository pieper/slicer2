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

/* 
   dcmparse.h
*/

#ifndef DCMPARSE_H
#define DCMPARSE_H

#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include "vtkObject.h"
//#include "vtkDCMDataElement.h"

#define TFS_IVRLE 1
#define TFS_EVRLE 2
#define TFS_EVRBE 3

/*typedef short INT16;
typedef unsigned short UINT16;
typedef int INT32;
typedef unsigned int UINT32;
typedef long LONG;
typedef unsigned long ULONG;*/

#define INT16 short
#define UINT16 unsigned short
#define INT32 int
#define UINT32 unsigned int
#define LONG long
#define ULONG unsigned long

//typedef void (*dcm_callback)(FILE *file_in, unsigned short group_code,
//			     unsigned short element_code, unsigned int length,
//			     char *vr, int *stop);

typedef struct DCMDataElementStruct
{
  char VR[4];
  unsigned short GroupCode;
  unsigned short ElementCode;
  unsigned int Length;
  unsigned int NextBlock;
};

static char *TFS_String[32] = 
{
  "Implicit VR Little Endian",
  "Explicit VR Little Endian",
  "Explicit VR Big Endian",
  "Unknown"
};

typedef class vtkDCMParser;

typedef void (*dcm_callback)(DCMDataElementStruct des, int *stop, vtkDCMParser *parser);

class VTK_EXPORT vtkDCMParser : public vtkObject
{
 public:
  static vtkDCMParser *New();
  vtkTypeMacro(vtkDCMParser,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  vtkDCMParser();
  vtkDCMParser(const char *filename);

  ~vtkDCMParser();

  void Skip(unsigned int length);
  unsigned short ReadUINT16();
  unsigned int ReadUINT32();
  float ReadFL();
  double ReadFD();
  float ReadFloatAsciiNumeric(unsigned int next_block);
  int ReadIntAsciiNumeric(unsigned int next_block);
  void ReadText(char *str, unsigned int length);
  char *ReadText(unsigned int length);
  char *ReadElement();
  unsigned long ReadElementLength();
  //void ReadElement(char *vr, char *group_code,
  //		   char *element_code, char *length);
  void ReadElement(DCMDataElementStruct *des);
  void UnreadLastElement();
  void ReadDICOMMetaHeaderInfo();
  void ReadDICOMHeaderInfo(dcm_callback dcm_funct);
  int FindElement(unsigned short group, unsigned short element);
  int FindNextElement(unsigned short group, unsigned short element);
  void SeekFirstElement();

  const char *GetMediaStorageSOPClassUID();
  const char *GetMediaStorageSOPInstanceUID();
  const char *GetTransferSyntaxUID();
  const char *GetImplementationClassUID();

  int IsMachineLittleEndian();
  int GetMustSwap();
  void SetMustSwap(int i);
  int GetTransferSyntax();
  const char *GetTransferSyntaxAsString();

  int OpenFile(const char *filename);
  void CloseFile();
  FILE *GetFileID();
  long GetFilePosition();

 protected:
  void Init();
  char *stringncopy(char *dest, const char *src, long max);

 private:
  FILE *file_in;

  char MediaStorageSOPClassUID[65];
  char MediaStorageSOPInstanceUID[65];
  char TransferSyntaxUID[65];
  char ImplementationClassUID[65];

  int MachineLittleEndian;
  int MustSwap;
  int TransferSyntax;
  
  int FileIOMessage;
  char buff[255];
  
  long PrevFilePos;
  long HeaderStartPos;
};

#endif

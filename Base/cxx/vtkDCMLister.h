/*=auto=========================================================================

(c) Copyright 2002 Massachusetts Institute of Technology

Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for any purpose, 
provided that the above copyright notice and the following three paragraphs 
appear on all copies of this software.  Use of this software constitutes 
acceptance of these terms and conditions.

IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================auto=*/

/* 
   dcmlister.h
*/
//
// DICOM header lister
// Attila Tanacs
// ERC for CISST, Johns Hopkins University, USA
// Dept. of Applied Informatics, University of Szeged, Hungary
// tanacs@cs.jhu.edu tanacs@inf.u-szeged.hu
//

#ifndef DCMLISTER_H
#define DCMLISTER_H

//#include <iostream.h>
//#include <fstream.h>
#include <stdio.h>
#include "vtkObject.h"
#include "vtkDCMParser.h"

#include "vtkSlicer.h"

#define AUX_STR_MAX 4096

struct DataElement {
  UINT16 Group;
  UINT16 Element;
  char VR[4];
  char *Name;
  struct DataElement *Next;
};

class VTK_SLICER_BASE_EXPORT vtkDCMLister : public vtkDCMParser
{
 public:
  static vtkDCMLister *New();
  vtkTypeMacro(vtkDCMLister,vtkDCMParser);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  vtkDCMLister();
  vtkDCMLister(const char *filename);
  ~vtkDCMLister();

  int ReadList(const char *filename);
  void ClearList();
  char * PrintList();
  void ListElement(unsigned short Group,
           unsigned short Element, unsigned long length,
           char *VR, char *Name);
  char * callback(unsigned short group_code,
        unsigned short element_code,
        unsigned long length,
        char *vr);

  int GetListAll() { return this->ListAll; }
  void SetListAll(int l) { this->ListAll = l; }

  char * GetTCLPreviewRow(int width, int SkipColumn, int max);

 protected:
 void Init();
 int isname(char ch);
 void getelement(int *i);
 void getquotedtext(int *i);

 char *aux_ret;
 char *aux_str;
 char *buff;
 int buff_maxlen;

 private:
 int ListAll;
 DataElement *FirstElement;

 char *line;
 char *element;
};

#endif

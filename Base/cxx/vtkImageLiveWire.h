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

// .NAME vtkImageLiveWire - 
// .SECTION Description
//

#ifndef __vtkImageLiveWire_h
#define __vtkImageLiveWire_h

#include "vtkImageTwoInputFilter.h"
#include "vtkImageDrawROI.h"

// avoid tcl wrapping ("begin Tcl exclude")
//BTX
//----------------------------------------------------------------------------
// doubly linked list element
class ListElement {
 public:
  ListElement *Prev; 
  ListElement *Next; 
  int Coord[2];
  ListElement() {this->Prev=NULL; this->Next=NULL;};
  
};

//----------------------------------------------------------------------------
// 2D array of list elements.
class LinkedList {
 public:
  ListElement GetElement(int x,int y) {return this->List[x + y*this->Cols];};
  ListElement *Element(int x,int y) {return (this->List + x + y*this->Cols);};

  LinkedList(int rows, int cols){
    this->Rows = rows;
    this->Cols = cols;
    this->List = new ListElement[this->Rows*this->Cols];
    for (int i = 0; i < this->Rows; i++) {
      for (int j = 0; j < this->Cols; j++) {
	this->Element(i,j)->Coord[0] = i;
	this->Element(i,j)->Coord[1] = j;
      }
    }   
  };

  ~LinkedList(){if (this->List) delete[] this->List;};

 protected:
  ListElement *List;
  int Rows, Cols;
};

//----------------------------------------------------------------------------
// This is a *really* circular queue.  Circle points to the heads
// of the linked lists in A.  Vertices are put in a "bucket"
// in Circle, where cost to vertex mod buckets in Circle (-1) defines
// the bucket.  The number of buckets is the max edge cost plus one.
// This way the buckets will always hold vertices with the same 
// cumulative path cost.
//
// Each linked list starts at a bucket and winds its way through A.
// New vertices are inserted at the top (by the bucket).
// To extract vertices for examination, it is best to use older
// ones (so it is more like a breadth first search than a depth first one)
// so the last vertex in A is linked back to the bucket.  
// This gives another circle (or a FIFO queue).
// 
// See IEEE Trans Med Img Jan 2000, Live Wire on the Fly for more.
//
class CircularQueue {
 public:  
  CircularQueue(int buckets, int rows, int cols)
    {
      this->A = new LinkedList(rows,cols);
      this->C = buckets;
      this->Circle = new ListElement[this->C+1];
      // link each bucket into its circle
      for (int i=0; i<C+1; i++) 
	{
	  this->Circle[i].Prev = this->Circle[i].Next = &this->Circle[i];
	}
    };
  
  ~CircularQueue()
    {
      // Lauren do we need to delete A?
      if (this->Circle) delete[] this->Circle;
    };

  void Insert(int bucket,ListElement *el)
    {
      //      if (this->Circle[bucket].Next)
      //	{
      //	  // the bucket is not empty.
      //	  this->Circle[bucket].Next->Prev = el;
      //	}
      //      else 
      //	{
      //	  // the bucket is empty.  link back around.
      //	  el->Next = &this->Circle[bucket];
      //	  this->Circle[bucket].Prev = el;
      //	}

      // insert el at the top of the list from the bucket
      el->Next = this->Circle[bucket].Next;
      this->Circle[bucket].Next->Prev = el;      
      this->Circle[bucket].Next = el;
      el->Prev = &this->Circle[bucket];



      //printf("put somebody in Q \n");
    }
  void Insert(int bucket, int x, int y)
    {
      ListElement *el = this->A->Element(x,y);
      this->Insert(bucket, el);
    }
  void Insert(int bucket, int *coord)
    {
      this->Insert(bucket, coord[0], coord[1]);
    }

  void Remove(int *coord)
    {
      this->Remove(coord[0],coord[1]);      
    }
  void Remove(int x, int y)
    {
      ListElement *el = this->A->Element(x,y);
      this->Remove(el);      
    }
  void Remove(ListElement *el)
    {
      // if el is in linked list
      if (el->Prev != NULL) {
      
	if (el->Next == NULL) {
	  printf("ERROR in vtkImageLiveWire.  el->Next is NULL.\n");
	}
	el->Next->Prev = el->Prev;
	el->Prev->Next = el->Next;
	
	// clear el's pointers
	el->Prev = el->Next = NULL;
      }
      return;
    }
  
  int FindMinBucket(int cost)
    {
      // Lauren check rounding okay...
      int bucket = (int)fmodf(cost, C+1);

      while (this->Circle[bucket].Next == &this->Circle[bucket])
	{
	  // search around the Q for the next vertex
	  //printf("bucket %d\n", bucket);
	  bucket++;
	}
      if (this->Circle[bucket].Prev == &this->Circle[bucket])
	{
	  printf("ERROR in vtkImageLiveWire.  Prev not linked right.\n");
	}
      return bucket;
    }

  ListElement *GetListElement(int bucket)
    {
      // return the last one in the linked list.
      if (this->Circle[bucket].Prev == NULL)
	{
	  printf("ERROR in vtkImageLiveWire.  Unlinked list.\n");
	}
      if (this->Circle[bucket].Next == &this->Circle[bucket])
	{
	  printf("ERROR in vtkImageLiveWire.  Empty linked list.\n");
	}
      return this->Circle[bucket].Prev;
    }

 protected:
  LinkedList *A;
  ListElement *Circle;
  int C;
};

// 2D array.
template <class T>
class array2D {
 public:
  T *array;
  int rows, cols;
  T GetElement(int x,int y) {return this->list[x + y*rows];};
  T *Element(int x,int y) {return (this->list + x + y*rows);};
  array2D(T init_val, int rows, int cols){
    this->array = new T[rows*cols];
    for(int i=0;i<rows*cols;i++){    
      this->array[i]= init_val;
    }
  };
  ~array2D(){if (this->array) delete[] this->array;};
};

// start Tcl wrapping again (cryptic acronym for End Tcl Exclude)
//ETX

class VTK_EXPORT vtkImageLiveWire : public vtkImageTwoInputFilter
{
public:
  static vtkImageLiveWire *New();
  vtkTypeMacro(vtkImageLiveWire,vtkImageTwoInputFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Top edges, direction from L to R
  void SetTopEdges(vtkImageData *image) {this->SetInput1(image);}
  vtkImageData *GetTopEdges() {return this->GetInput1();}

  // Description:
  // Right edges, direction downward
  void SetRightEdges(vtkImageData *image) {this->SetInput2(image);}
  vtkImageData *GetRightEdges() {return this->GetInput2();}

  // Description:
  // Starting point of shortest path (mouse click #1)
  vtkSetVector2Macro(StartPoint, int);
  vtkGetVector2Macro(StartPoint, int);

  // Description:
  // Ending point of shortest path (mouse click #2)
  vtkSetVector2Macro(EndPoint, int);
  vtkGetVector2Macro(EndPoint, int);

  // Description:
  // Max cost of any single pixel edge; also size of circular queue
  vtkSetMacro(MaxEdgeCost, int);
  vtkGetMacro(MaxEdgeCost, int);
  
  // Description:
  // This filter represents contour points
  // it outputs an image containing them
  vtkGetObjectMacro(ContourPoints, vtkImageDrawROI);

  // 
  CircularQueue *Q;


protected:
  vtkImageLiveWire();
  ~vtkImageLiveWire() {};
  vtkImageLiveWire(const vtkImageLiveWire&) {};
  void operator=(const vtkImageLiveWire&) {};

  int StartPoint[2];
  int EndPoint[2];
  
  int MaxEdgeCost;
  
  // Lauren here for now but vkMrmlSlicer has one of these
  // we should use?
  vtkImageDrawROI *ContourPoints;

  void ExecuteInformation(vtkImageData **inputs, vtkImageData *output); 
  void ComputeInputUpdateExtent(int inExt[6], int outExt[6],
				int whichInput);
  void ExecuteInformation(){this->vtkImageTwoInputFilter::ExecuteInformation();};
  virtual void Execute(vtkImageData **inDatas, vtkImageData *outData);  

};

#endif



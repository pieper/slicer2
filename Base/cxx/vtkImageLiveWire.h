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
// 2D array.
// uses x,y indices like image coords, so x=row and y=column.
template <class T>
class array2D {
 public:
  array2D(int x, int y){
    this->Rows = y;
    this->Cols = x;
    this->array = new T[this->Rows*this->Cols];
  };

  array2D(int x, int y, T initVal){
    this->Rows = y;
    this->Cols = x;
    this->array = new T[this->Rows*this->Cols];

    for( int i=0; i < this->Rows*this->Cols; i++){    
      this->array[i]= initVal;
    }
  };

  ~array2D(){if (this->array) delete[] this->array;};

  // Get/Set functions 
  T GetElement(int x,int y) {return this->array[x + y*this->Cols];};
  void SetElement(int x,int y, T value) {this->array[x + y*this->Cols] = value;};

  // return the array element itself
  T&       operator() (int x, int y){return this->array[x + y*this->Cols];};

  // return a pointer to the array element
  T *Element(int x,int y) {return (this->array + x + y*this->Cols);};

 private:
  T *array;
  int Rows, Cols; 

};



//----------------------------------------------------------------------------
// 2D array of list elements
class LinkedList : public array2D<ListElement>{
 public:

  LinkedList(int rows, int cols)
    :
    array2D(rows,cols)
    {
      for (int i = 0; i < rows; i++) {
	for (int j = 0; j < cols; j++) {
	  this->Element(i,j)->Coord[0] = i;
	  this->Element(i,j)->Coord[1] = j;
	}
      }
    }  
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
// Lauren is this really much better?
// To extract vertices for examination, it is best to use older
// ones (so it is more like a breadth first search than a depth first one)
// so the last vertex in A is linked back to the bucket.  
// This gives another circle (or a FIFO queue).
// 
// See IEEE Trans Med Img Jan 2000, Live Wire on the Fly for more.
//
class circularQueue {
 public:  
  circularQueue(int rows, int cols, int buckets)
    {
      this->A = new LinkedList(rows,cols);
      this->C = buckets;
      this->Circle = new ListElement[this->C+1];
      // link each bucket into its circle
      for (int i=0; i<C+1; i++) 
	{
	  this->Circle[i].Prev = this->Circle[i].Next = &this->Circle[i];
	}
      this->Verbose = 0;
    };
  
  ~circularQueue()
    {
      if (this->A) delete this->A;
      if (this->Circle) delete[] this->Circle;
    };

  void Insert(int x, int y, int cost)
    {
      int bucket = this->GetBucket(cost);
		
      ListElement *el = this->A->Element(x,y);
      // insert el at the top of the list from the bucket
      el->Next = this->Circle[bucket].Next;
      if (el->Next == NULL) 
	{
	  cout <<"ERROR in vtkImageLiveWire.  bucket is NULL, not linked to self.\n" << endl;
	}
      this->Circle[bucket].Next->Prev = el;      
      this->Circle[bucket].Next = el;
      el->Prev = &this->Circle[bucket];

      if (this->Verbose)
	{
	  cout << "Q_INSERT " << "b: " << bucket << " " << "c: " 
	       << cost << " (" << x << "," << y << ")" << endl;
	}
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
      if (el->Prev != NULL) 
	{
      
	if (el->Next == NULL)
	  {
	    cout <<"ERROR in vtkImageLiveWire.  el->Next is NULL.\n "<< endl;
	  }
	el->Next->Prev = el->Prev;
	el->Prev->Next = el->Next;
	
	// clear el's pointers
	el->Prev = el->Next = NULL;
	}
      
      if (this->Verbose)
	{
	  cout << "Q_REMOVE " << "(" << el->Coord[0] << "," 
	       << el->Coord[1] <<")" << endl;
	}

      return;
    }
  
  ListElement *GetListElement(int cost)
    {
      int bucket = FindMinBucket(cost);

      // return the last one in the linked list.
      if (this->Circle[bucket].Prev == NULL)
	{
	  cout << "ERROR in vtkImageLiveWire.  Unlinked list." << endl;
	}
      if (this->Circle[bucket].Next == &this->Circle[bucket])
	{
	  cout << "ERROR in vtkImageLiveWire.  Empty linked list." << endl;
	}
      if (this->Verbose)
	{
	  cout << "Q_GET b: " << bucket << endl;
	}
      return this->Circle[bucket].Prev;
    }

 private:

  int GetBucket(int cost)
    {
      //return (int)fmodf(cost, this->C+1);

      // return remainder
      return div(cost,this->C+1).rem;
    }

  // Lauren if the queue is empty this will loop forever.
  int FindMinBucket(float cost)
    {
      int bucket = this->GetBucket(cost);
      int count = 0;

      while (this->Circle[bucket].Next == &this->Circle[bucket] && count < this->C+1)
	{
	  // search around the Q for the next vertex
	  cost++;
	  bucket = this->GetBucket(cost);
	  // have we looped all the way around?
	  count++;
	}

      if (count > this->C) 
	{
	  cout << "ERROR in vtkImageLiveWire.  Empty Q." << endl;
	}
      if (this->Circle[bucket].Prev == &this->Circle[bucket])
	{
	  cout <<"ERROR in vtkImageLiveWire.  Prev not linked right." << endl;
	}

      return bucket;
    }

  LinkedList *A;
  ListElement *Circle;
  int C;
  bool Verbose;
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
  // For testing.  
  vtkSetMacro(Verbose, int);
  vtkGetMacro(Verbose, int);
  
  // Description:
  // This filter represents contour points
  // it outputs an image containing them
  vtkGetObjectMacro(ContourPoints, vtkImageDrawROI);

  // ---- Data structures for internal use in path computation -- //
  // Description:
  // 
  circularQueue *Q;

  //BTX
  // Description:
  // CC is the cumulative cost from StartPoint to each pixel
  array2D<int> *CC;

  // Description:
  // Dir is the direction the optimal path takes through each pixel.
  array2D<int> *Dir;

  // Description:
  // The directions the path may take.
  enum {NONE, UP, DOWN, RIGHT, LEFT};


  // Description:
  // L is the list of edges ("bels") which have already been processed
  array2D<bool> *L;

  // Description:
  // B is list of edges ("bels") on the contour already
  array2D<bool> *B;
  //ETX
  // ---- End of data structures for internal use in path computation -- //



protected:
  vtkImageLiveWire();
  ~vtkImageLiveWire() {};
  vtkImageLiveWire(const vtkImageLiveWire&) {};
  void operator=(const vtkImageLiveWire&) {};

  int StartPoint[2];
  int EndPoint[2];
  
  int MaxEdgeCost;

  int Verbose;

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



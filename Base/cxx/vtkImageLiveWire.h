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

#include "vtkImageMultipleInputFilter.h"
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
	  cout << "ERROR in vtkImageLiveWire.  bucket is NULL, not linked to self." << endl;
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
	    cout <<"ERROR in vtkImageLiveWire.  el->Next is NULL."<< endl;
	    return;
	  }
	el->Next->Prev = el->Prev;
	el->Prev->Next = el->Next;
	
	// clear el's pointers
	el->Prev = el->Next = NULL;
	}
      else
	{
	  if (this->Verbose)
	    {
	      cout <<"Q_REMOVE: el->Prev is NULL, el (" << el->Coord[0] << "," 
		   << el->Coord[1] << ") not in Q."<< endl;
	      return;
	    }
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
	  int x = this->Circle[bucket].Prev->Coord[0];
	  int y = this->Circle[bucket].Prev->Coord[1];
	  cout << "Q_GET b: " << bucket << ", point: ("<< x << "," << y << ")" << endl;	  
	}
      return this->Circle[bucket].Prev;
    }

  void VerboseOn() 
    {
      this->Verbose = 1;
    }

 private:

  int GetBucket(int cost)
    {
      if (cost < 0 ) 
	{
	  cout << "ERROR in vtkImageLiveWire: negative cost of " << cost << endl;
	}
      
      //return (int)fmodf(cost, this->C+1);

      // return remainder
      return div(cost,this->C+1).rem;
    }

  int FindMinBucket(float cost)
    {
      int bucket = this->GetBucket(cost);
      int count = 0;

      while (this->Circle[bucket].Next == &this->Circle[bucket] && count <= this->C)
	{
	  // search around the Q for the next vertex
	  cost++;
	  bucket = this->GetBucket(cost);
	  count++;
	}

      // have we looped all the way around?
      if (count > this->C) 
	{
	  cout << "ERROR in vtkImageLiveWire.  Empty Q." << endl;
	}
      if (this->Circle[bucket].Prev == &this->Circle[bucket])
	{
	  cout <<"ERROR in vtkImageLiveWire.  Prev not linked to bucket." << endl;
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

class VTK_EXPORT vtkImageLiveWire : public vtkImageMultipleInputFilter
{
public:
  static vtkImageLiveWire *New();
  vtkTypeMacro(vtkImageLiveWire,vtkImageMultipleInputFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Image data all these edges were created from
  void SetOriginalImage(vtkImageData *image) {this->SetInput(0,image);}
  vtkImageData *GetOriginalImage() {return this->GetInput(0);}

  // Description:
  // Top edges, direction from L to R  (corresponds to Dir RIGHT)
  void SetTopEdges(vtkImageData *image) {this->SetInput(1,image);}
  vtkImageData *GetTopEdges() {return this->GetInput(1);}

  // Description:
  // Right edges, direction downward (corresponds to Dir DOWN)
  void SetRightEdges(vtkImageData *image) {this->SetInput(2,image);}
  vtkImageData *GetRightEdges() {return this->GetInput(2);}

  // Description:
  // Bottom edges, direction from R to L (corresponds to Dir LEFT)
  void SetBottomEdges(vtkImageData *image) {this->SetInput(3,image);}
  vtkImageData *GetBottomEdges() {return this->GetInput(3);}

  // Description:
  // Left edges, direction upward (corresponds to Dir UP)
  void SetLeftEdges(vtkImageData *image) {this->SetInput(4,image);}
  vtkImageData *GetLeftEdges() {return this->GetInput(4);}

  // Description:
  // Starting point of shortest path (mouse click #1)
  void SetStartPoint(int x, int y);
  void SetStartPoint(int *point){this->SetStartPoint(point[0],point[1]);};
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
  // Cumulative cost of current path.
  // Don't set this; it's here for access from vtkImageLiveWireExecute
  vtkSetMacro(CurrentCC, int);
  vtkGetMacro(CurrentCC, int);

  // Description:
  // Current point of "longest shortest" path found so far.
  // Don't set this; it's here for access from vtkImageLiveWireExecute
  vtkSetVector2Macro(CurrentPoint, int);
  vtkGetVector2Macro(CurrentPoint, int);
  
  // Description:
  // Points on the shortest path ("contour")
  // The output image has these highlighted.
  vtkGetObjectMacro(ContourPoints, vtkPoints);

  // ---- Data structures for internal use in path computation -- //
  // Description:
  // Circular queue, composed of buckets that hold vertices of each path cost.
  // The vertices are stored in a doubly linked list for each bucket.
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
  enum {UP, DOWN, LEFT, RIGHT, NONE};


  // Description:
  // L is the list of edges ("bels") which have already been processed
  array2D<bool> *L;

  // Description:
  // B is list of edges ("bels") on the contour already
  array2D<bool> *B;
  //ETX
  // ---- End of data structures for internal use in path computation -- //

  // Description:
  // Points on the contour...  (maybe this is B?)
  // Lauren why was there no get/set before??  (fix)
  //vtkGetObjectMacro(ContourPoints, vtkPoints);
  vtkPoints *ContourPoints;

  // Description:
  // This is public since it is called from the non-class function 
  // vtkImageLiveWireExecute...  Don't call this.
  void AllocatePathInformation(int numRows, int numCols);

protected:
  vtkImageLiveWire();
  ~vtkImageLiveWire();
  vtkImageLiveWire(const vtkImageLiveWire&) {};
  void operator=(const vtkImageLiveWire&) {};

  int StartPoint[2];
  int EndPoint[2];
  int PrevEndPoint[2];

  int CurrentCC;
  int *CurrentPoint;

  int MaxEdgeCost;

  int Verbose;


  void DeallocatePathInformation();

  void ExecuteInformation(vtkImageData **inputs, vtkImageData *output); 
  void ComputeInputUpdateExtent(int inExt[6], int outExt[6],
				int whichInput);
  void ExecuteInformation(){this->vtkImageMultipleInputFilter::ExecuteInformation();};
  virtual void Execute(vtkImageData **inDatas, vtkImageData *outData);  

};

#endif




This document describes how to build FMRIEngine module in 3D Slicer.

1. Follow the general instructions at http://www.slicer.org/ to 
   Get 3D Slicer source code,
   Install required software components (e.g. vtk, itk, tcl/tk and cmake), and
   Set up environmental variables.

2. Install the GNU Scientific Library (GSL) 
   The GSL is a free GNU numerical library for C and C++ programmers. The FMRIEngine
   module use it to compute activation volume for a time series.

   The current version is GSL-1.4, which was released on 14 August 2003. You can
   download the library from http://sources.redhat.com/gsl/. Follow their instructions 
   to build.

3. Go to /your path to/slicer2/Modules/vtkFMRIEngine/cxx/.
   Modify file "CMakeListsLocal.txt" to point to the location of your installed GSL.

4. Go to /your path to/slicer2/Scripts/. Execute the following command to compile
   your 3D Slicer:
   ./cmaker.tcl --clean


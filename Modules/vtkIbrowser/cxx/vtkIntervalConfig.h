
#ifndef __vtkIntervalConfig_h
#define __vtkIntervalConfig_h
// Types of vtkIntervals
#define IMAGEINTERVAL         0
#define DATAINTERVAL          1
#define EVENTINTERVAL         2
#define GEOMINTERVAL          3
#define NOTEINTERVAL           4

// Kinds of interpolation
#define LINEAR                      0
#define BSPLINE                    1

// Kinds of deriving you can do
#define COPY                         0

enum intervalType { IMAGE, DATA, EVENT, GEOMETRY, NOTE };
enum mybool { False, True };
#endif

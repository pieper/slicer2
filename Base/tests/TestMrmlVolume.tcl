catch {load vtktcl}
source vtkImageInclude.tcl

vtkMrmlVolumeNode node
node LittleEndianOn
node SetDescription "This code better work!"
node SetName "Dave"
node SetFilePrefix "../../../vtkdata/fullHead/headsq"
# the full prefix is used by the reader, not the file prefix.
node SetFullPrefix "../../../vtkdata/fullHead/headsq"
node SetFilePattern "%s.%d"
node SetImageRange 1 93
node SetDimensions 256 256

vtkMrmlVolume vol
vol SetMrmlNode node
puts [node GetFilePrefix]
vol Read

vtkImageViewer viewer
viewer SetInput [vol GetImageData]
viewer SetZSlice 0 
viewer SetColorWindow 2000
viewer SetColorLevel 1000

#make interface
source WindowLevelInterface.tcl








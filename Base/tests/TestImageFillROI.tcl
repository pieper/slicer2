catch {load vtktcl}
source vtkImageInclude.tcl

# Image pipeline

vtkImageReader reader
reader ReleaseDataFlagOff
reader SetDataByteOrderToLittleEndian
reader SetDataExtent 0 255 0 255 1 93
reader SetFilePrefix "../../../vtkdata/fullHead/headsq"
reader SetDataMask 0x7fff

# get just one slice
vtkImageClip clip
clip SetInput [reader GetOutput]
clip SetOutputWholeExtent 0 255 0 255 10 10
clip ClipDataOn
clip ReleaseDataFlagOff

vtkPoints roiPoints
roiPoints InsertNextPoint 2 2 0
roiPoints InsertNextPoint 200 230 0
roiPoints InsertNextPoint 220 50 0
roiPoints InsertNextPoint 4 10 0

#vtkImageLabelMerge flroi
vtkImageFillROI flroi
#flroi SetInput [reader GetOutput]
flroi SetInput [clip GetOutput]
flroi SetValue 1000
flroi SetRadius 10

#flroi SetShapeString Polygon
flroi SetShapeString Lines
#flroi SetShapeString Points
flroi SetPoints roiPoints

vtkImageViewer viewer
viewer SetInput [flroi GetOutput]
viewer SetZSlice 22
viewer SetColorWindow 2000
viewer SetColorLevel 1000

#make interface
source WindowLevelInterface.tcl








catch {load vtktcl}
source vtkImageInclude.tcl

# Image pipeline

vtkImageReader reader
reader ReleaseDataFlagOff
reader SetDataByteOrderToLittleEndian
reader SetDataExtent 0 255 0 255 1 93
reader SetFilePrefix "../../../vtkdata/fullHead/headsq"
reader SetDataMask 0x7fff

vtkImageAccumulateDiscrete accum
accum SetInput [reader GetOutput]

vtkImageMeasureVoxels measure
measure SetInput [accum GetOutput]
measure SetFileName TestImageMeasureVoxels.txt

measure Update

puts "results are in the file TestImageMeasureVoxels.txt"
exit







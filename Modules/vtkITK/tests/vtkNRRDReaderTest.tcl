
package require vtkITK

catch "nr Delete"
vtkNRRDReader nr

nr SetFileName d:/data/namic/HUVA00024864/HUVA00024864_spgr.nhdr

nr UpdateInformation

puts [nr Print]

catch "v Delete"
vtkImageViewer v
v SetInput [nr GetOutput]
v SetColorWindow 300
v SetColorLevel 200

set zdim [lindex [[nr GetOutput] GetDimensions] 0]
for {set z 0} {$z < $zdim} {incr z} {
    v SetZSlice $z
    v Render
    update
}


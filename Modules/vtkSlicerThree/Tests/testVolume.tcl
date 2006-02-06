catch "sc Delete"
vtkMRMLScene sc

catch "vn Delete"
vtkMRMLVolumeNode vn

sc RegisterNodeClass vn "Volume"
sc SetURL Modules/vtkSlicerThree/Tests/scene1.xml
sc Connect

puts "GetNumberOfNodesByClass [sc GetNumberOfNodesByClass vtkMRMLVolumeNode]"
puts "GetNodeClasses [sc GetNodeClasses]"
puts "GetNthNode"

set v0 [sc GetNthNode 0]
puts "Print volume node 0"
puts "[$v0 Print]"

set v1 [sc GetNthNode 1]
puts "Print volume node 1"
puts "[$v1 Print]"




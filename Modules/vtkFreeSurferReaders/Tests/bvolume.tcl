#! /usr/local/bin/vtk

# first we load in the standard vtk packages into tcl
package require vtk
package require vtkinteraction

load ../bin/libvtkFreeSurferReadersTCL.so

set ksStem /home/kteich/test_data/functional/overlay-bfloat/fsig
set ksRegister /home/kteich/test_data/functional/overlay-bfloat/register.dat

# Try a bunch of different kinds of file names. You can set the
# FileName to one of the bfloat or bshort files, or set the FilePrefix
# to the stem, stem_, or stem_000, where stem is part of the file name
# before the underscore. Both FileName and FilePrefix should have
# complete path information.
puts "Trying FilePrefix = stem"
set reader [vtkBVolumeReader _reader]
$reader SetFilePrefix $ksStem
$reader Update
set stem [$reader GetStem]
if { $stem == "$ksStem" } { 
    puts "Got it"
}

puts "Trying FilePrefix = stem_000"
$reader SetFilePrefix ${ksStem}_000
$reader Update
set stem [$reader GetStem]
if { $stem == "$ksStem" } { 
    puts "Got it"
}

puts "Trying FilePrefix = stem_"
$reader SetFilePrefix ${ksStem}_
$reader Update
set stem [$reader GetStem]
if { $stem == "$ksStem" } { 
    puts "Got it"
}

puts "Trying FilePrefix = stem"
$reader SetFilePrefix $ksStem
$reader Update
set stem [$reader GetStem]
if { $stem == "$ksStem" } { 
    puts "Got it"
}

puts "Trying FileName = stem_000.bfloat"
$reader SetFilePrefix ""
$reader SetFileName ${ksStem}_000.bfloat
$reader Update
set stem [$reader GetStem]
if { $stem == "$ksStem" } { 
    puts "Got it"
}

puts "Trying FileName = stem_.bfloat"
$reader SetFilePrefix ""
$reader SetFileName ${ksStem}_.bfloat
$reader Update
set stem [$reader GetStem]
if { $stem == "$ksStem" } { 
    puts "Got it"
}

puts "Trying FileName = stem.bfloat"
$reader SetFilePrefix ""
$reader SetFileName ${ksStem}.bfloat
$reader Update
set stem [$reader GetStem]
if { $stem == "$ksStem" } { 
    puts "Got it"
}

# Try getting the registration. You have to specify
# RegistrationFileName. Then calling GetRegistrationMatrix will parse
# the file, create a vtkMatrix4x4, and return it.
puts "Getting registration $ksRegister"
$reader SetRegistrationFileName $ksRegister
set reg [$reader GetRegistrationMatrix]
puts "Got it"
puts "[$reg GetElement 0 0] [$reg GetElement 1 0] [$reg GetElement 2 0] [$reg GetElement 3 0]"
puts "[$reg GetElement 0 1] [$reg GetElement 1 1] [$reg GetElement 2 1] [$reg GetElement 3 1s]"
puts "[$reg GetElement 0 2] [$reg GetElement 1 2] [$reg GetElement 2 2] [$reg GetElement 3 2]"
puts "[$reg GetElement 0 3] [$reg GetElement 1 3] [$reg GetElement 2 3] [$reg GetElement 3 3]"


exit


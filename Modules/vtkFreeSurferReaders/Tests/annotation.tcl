#! /usr/local/bin/vtk

# first we load in the standard vtk packages into tcl
package require vtk
package require vtkinteraction

load ../bin/libvtkFreeSurferReadersTCL.so


proc TestAnnot { annotFileName { colorTableFileName "" } } {

    vtkIntArray labels
    vtkLookupTable colors
    vtkFSSurfaceAnnotationReader ar

    # Try to load an annotation file, first by using an embedded color table.
    ar SetFileName $annotFileName
    ar SetOutput labels
    ar SetColorTableOutput colors
    ar DontUseExternalColorTableFile
    set err [ar ReadFSAnnotation] 
    if { $err == 6 } {
    
    # Here you might ask the user to choose a color
    # table. Alternatively, use a 'known' good color table.  Set the
    # color table name, tell the reader to use an external table,
    # and try reading again
    ar SetColorTableFileName $colorTableFileName
    ar DoUseExternalColorTableFile
    set err [ar ReadFSAnnotation]
    }
    
    # Handle errors or warnings. These error code constants are in
    # vtkFSSurfaceAnnotationReader.h
    if { $err } {
    switch $err {
        1 {
        puts "Error: Couldn't load external color table."
        exit
        }
        2 {
        puts "Error: Couldn't load annotation file."
        exit
        }
        3 {
        puts "Error: Couldn't parse external color table."
        exit
        }
        4 {
        puts "Error: Couldn't parse annotation file."
        exit
        }
        5 {
        puts "Warning: some annotation label values did not have corresponding entries in the color table."
        }
    }
    }
    
    # check some values
    puts "Labels:"
    set lRange [labels GetRange]
    puts "Length: [labels GetNumberOfTuples]"
    puts "Range: [lindex $lRange 0] -> [lindex $lRange 1]"

    puts "Colors:"
    puts "Length: [colors GetNumberOfTableValues]"

    set Names [ar GetColorTableNames]
    puts "Names:"
    array set aNames $Names
    puts $aNames(4)

    labels Delete
    colors Delete
    ar Delete
}


puts "old style with external table"
TestAnnot /home/kteich/subjects/anders/label/lh.annot \
    /home/kteich/freesurfer/surface_labels.txt

puts ""
puts "new style with embedded table"
TestAnnot /home/kteich/test_data/surface_annotation/lh.aparc.annot


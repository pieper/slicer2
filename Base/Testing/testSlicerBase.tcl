package require vtkSlicerBase

# get the list of header files in the Base
set flist [glob $::env(SLICER_HOME)/Base/cxx/*.h]

set numFailedDeclare 0
set numFailedDelete 0
set numSuccess 0
set successList {}
set failDeclareList {}
set failDeleteList {}
set exitCode 0

foreach f $flist {
    # get a potential class name
    set classname [file root [file tail $f]]

    # is it a vtk class?
    if {[regexp "^vtk.*" $classname matchvar] == 1 &&
        [regexp ".*Macro$" $classname matchvar] == 0} {
        puts "Testing $classname"
        if {[catch "$classname myclass" errmsg] == 1} {
            puts "$errmsg"
            incr numFailedDeclare
            lappend failDeclareList $classname
        } else {
            if {[catch "myclass Delete" errmsg] == 1} {
                puts "Delete failed: $errmsg"
                incr numFailedDelete
                lappend failDeleteList $classname
            } else {
                incr numSuccess
                lappend sucessList $classname
            }
        }
    }
}

if {$numFailedDeclare > 0} {
    puts "Failed on Declare:"
    foreach d $failDeclareList {
        puts "\t$d"
    }
}
if {$numFailedDelete > 0} {
    puts "Failed on Delete:"
    foreach d $failDeleteList {
        puts "\t$d"
    }
}
puts "$numSuccess classes passed, $numFailedDeclare failed on declaratrion, $numFailedDelete failed on delete"

set exitCode [expr $numFailedDeclare + $numFailedDelete]

exit $exitCode

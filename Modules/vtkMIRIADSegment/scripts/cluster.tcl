#!/bin/sh
# the next line restarts using tclsh \
exec tclsh "$0" "$@"

proc echo {args} {puts "$args"}

set ids "000300742113  000315960404  000331062724  000354437431  000371609839"

set node 0
foreach birnid $ids {
    foreach visit {001 002} {
        puts "Processing $birnid $visit on compute-2-$node"
        set fp [open "| csh -c \"ssh compute-2-$node ~/birn/bin/runvnc :2 \
                    --wm /usr/X11R6/bin/xterm -- \
                    ~/birn/slicer2/slicer2-linux-x86 --no-tkcon \
                        --exec MIRIADSegmentProcessStudy $birnid $visit ., exit \
                  \" |& cat" "r"]
        break
        incr node
    }
    break
}

proc file_event {fp} {
    global END
    if {[eof $fp]} {
        catch "close $fp"
        set END 1
    } else {
        gets $fp line
        puts $line
    }
}

fileevent $fp readable "file_event $fp"

set END 0
while { ![catch "pid $fp"] && ![eof $fp] } {
    vwait END
}


puts done

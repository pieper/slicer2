#!/bin/sh
# the next line restarts using tclsh \
exec tclsh "$0" "$@"

proc echo {args} {puts "$args"}


proc init {} {
    set ::ids [exec ssh gpop.bwh.harvard.edu \
          ls -1 /nas/nas0/pieper/data/MIRIAD/Project_0002]

    set ::jobs ""
    foreach birnid $::ids {
        foreach visit {001 002} {
            lappend ::jobs [list $birnid $visit]
        }
    }

    set ::computes ""
    for {set rack 0} {$rack < 4} {incr rack} {
        for {set row 0} {$row < 32} {incr row} {
            lappend ::computes compute-$rack-$row
        }
    }
    set ::free_computes ""
}


proc load {compute} {
    if { [catch "exec ssh $compute uptime" res] } {
        return 100.00
    } else {
        # return the 1 min average load
        return [lindex $res end-2]
    }
}

proc find_compute {} {
    if { [llength $::free_computes] == 0 } {
        puts "searching for free computes"
        foreach compute $::computes {
            if { [load $compute] < 0.5 } {
                lappend ::free_computes $compute
            }
        }
        if { [llength $::free_computes] == 0 } {
            puts "sorry, all nodes busy"
            return ""
        }
    }
    set compute [lindex $::free_computes 0]
    set ::free_computes [lrange $::free_computes 1 end]
    return $compute 
}

proc is_running {pid} {
    if { [catch "exec kill -18 $pid"] } {
        return 0
    } else {
        return 1
    }
}

proc run_job {compute job} {
    set birnid [lindex $job 0]
    set visit [lindex $job 1]

    if { 1 } {
        set fp [open "| csh -c \"ssh $compute ~/birn/bin/runvnc :2 \
                    --wm /usr/X11R6/bin/xterm -- \
                    ~/birn/slicer2/slicer2-linux-x86 --no-tkcon \
                        --exec exit \
                  \" |& cat" "r"]
    } else {
        set fp [open "| csh -c \"ssh $compute ~/birn/bin/runvnc :2 \
                    --wm /usr/X11R6/bin/xterm -- \
                    ~/birn/slicer2/slicer2-linux-x86 --no-tkcon \
                        --exec MIRIADSegmentProcessStudy $birnid $visit ., exit \
                  \" |& cat" "r"]
    }
    return $fp
}



proc file_event {fp job} {
    global END
    if {[eof $fp]} {
        catch "close $fp"
        set END 1
    } else {
        gets $fp line
        lappend ::logs($job) $line
        #puts $line
    }
}


proc run_jobs {} {

    init

    foreach job $::jobs {
        while { [set compute [find_compute]] == "" } {
            puts "waiting for a computer..."
            after 1000
        }
        puts "launching $job on $compute"
        set fps($job) [run_job $compute $job]
        fileevent $fps($job) readable "file_event $fps($job) \"$job\""
        set pids($job) [pid $fps($job)]
        set ::logs($job) ""
    }
    puts "done launching"


    while { [array names pids] != "" } {
        update
        foreach job [array names pids] {
            if { ![is_running $pids($job)] } {
                puts "finished: $job"
                unset pids($job)
            }
        }
        after 1000
        puts "[llength [array names pids]] still running..."
    }

    set fp [open "cluster-logs" "w"]
    foreach log [array names ::logs] {
        puts $fp $::logs($log)
    }
    close $fp

    puts "done"
    return


    if { 0 } {
        set END 0
        while { ![catch "pid $fp"] && ![eof $fp] } {
            vwait END
        }
    }

}

run_jobs

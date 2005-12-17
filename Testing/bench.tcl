
#
# utility code for running benchmarks
#

proc bench_init {} {

    package require vtk

    catch "bench_mt Delete"
    vtkMultiThreader bench_mt

    set ::BENCH(numThreads) [bench_mt GetNumberOfThreads]
    set ::BENCH(benchmarks) [glob -nocomplain $::env(SLICER_HOME)/Testing/Benchmarks/*.tcl]

    puts "will use up to $::BENCH(numThreads) threads"
}


proc bench_run {} {

    foreach benchmark $::BENCH(benchmarks) {
        set bench [file root [file tail $benchmark]]
        
        set memMultiple 1
        while (1) {
            for {set nthreads 1} {$nthreads <= $::BENCH(numThreads)} {incr nthreads} {

                bench_mt SetGlobalMaximumNumberOfThreads $nthreads

                puts "running $bench on $nthreads at memory multiple $memMultiple"; update

                source $benchmark
                set ret [catch {time "${bench}_run $memMultiple"} res]

                if { $ret } {
                    puts "failed"; update
                    set ::BENCH($bench,$nthreads,$memMultiple) "failed"
                    break
                } else {
                    puts $res; update
                    set ::BENCH($bench,$nthreads,$memMultiple) [lindex $res 0]
                }

            }
            incr memMultiple
            update
        }
    }

    parray ::BENCH
}

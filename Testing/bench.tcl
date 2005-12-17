
#
# utility code for running benchmarks
#

proc bench_init {} {

    package require vtk

    catch "bench_mt Delete"
    vtkMultiThreader bench_mt

    set ::BENCH(numThreads) [bench_mt GetNumberOfThreads]
    set ::BENCH(benchmarks) [glob -nocomplain $::env(SLICER_HOME)/Testing/Benchmarks/*.tcl]
}


proc bench_run {} {

    foreach benchmark $::BENCH(benchmarks) {
        set bench [file root [file tail $benchmark]]
        
        for {set nthreads 1} {$nthreads < $::BENCH(numThreads)} {incr nthreads} {
            set memMultiple 1
            while (1) {
                puts "running $bench at memory multiple $memMultiple"; update
                bench_mt SetGlobalMaximumNumberOfThreads $nthreads
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
                incr memMultiple
            }
            update
        }
    }

    parray ::BENCH
}

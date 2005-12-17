




proc axflip_run { {mem 1} } {

    catch "blur_iss Delete"
    catch "blur_ir Delete"

    vtkImageSinusoidSource blur_iss

    set dim [expr $mem * 200]
    blur_iss SetWholeExtent 0 200 0 200 0 $dim

    vtkImageReslice blur_ir
    blur_ir SetResliceAxesDirectionCosines .707 .707 0  0 .707 .707  .707 0 .707
    blur_ir SetInput [blur_iss GetOutput]
    [blur_ir GetOutput] SetUpdateExtentToWholeExtent

    puts "blur updating 200x200x$dim..."; update
    [blur_ir GetOutput] Update
    set memory_used [[blur_ir GetOutput] GetActualMemorySize]
    puts "blur done on $memory_used"; update

    catch "blur_iss Delete"
    catch "blur_ir Delete"

    return 0
}

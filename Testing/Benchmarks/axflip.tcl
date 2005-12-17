




proc axflip_run { id } {

    catch "ir Delete"

    vtkImageReslice ir
    ir SetResliceAxesDirectionCosines .707 .707 0  0 .707 .707  .707 0 .707
    ir SetInput $id
    [ir GetOutput] SetUpdateExtentToWholeExtent

    puts "reslice updating..."; update
    [ir GetOutput] Update
    set memory_used [[ir GetOutput] GetActualMemorySize]
    puts "reslice done on $memory_used"; update

    catch "ir Delete"

    return 0
}


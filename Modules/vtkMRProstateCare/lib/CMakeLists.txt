PROJECT(coords)


IF (WIN32)
    ADD_LIBRARY(coords STATIC 
        Decompose.c 
        Mat.c
        free_bs.c
        write_output.c
        compute_CRD.c
        compute_transform.c
        parse_input.c
    )
ELSE (WIN32)
    ADD_LIBRARY(coords 
        Decompose.c 
        Mat.c
        free_bs.c
        write_output.c
        compute_CRD.c
        compute_transform.c
        parse_input.c
    )
ENDIF (WIN32)

TARGET_LINK_LIBRARIES (coords)


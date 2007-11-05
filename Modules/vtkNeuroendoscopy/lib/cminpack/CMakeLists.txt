PROJECT(cminpack)

ADD_LIBRARY(cminpack 
    chkder_.c
    chkder.c
    dogleg_.c
    dogleg.c
    dpmpar_.c
    dpmpar.c
    enorm_.c
    enorm.c
    fdjac1_.c
    fdjac1.c
    fdjac2_.c
    fdjac2.c
    hybrd1_.c
    hybrd1.c
    hybrd_.c
    hybrd.c
    hybrj1_.c
    hybrj1.c
    hybrj_.c
    hybrj.c
    lmder1_.c
    lmder1.c
    lmder_.c
    lmder.c
    lmdif1_.c
    lmdif1.c
    lmdif_.c
    lmdif.c
    lmpar_.c
    lmpar.c
    lmstr1_.c
    lmstr1.c
    lmstr_.c
    lmstr.c
    qform_.c
    qform.c
    qrfac_.c
    qrfac.c
    qrsolv_.c
    qrsolv.c
    r1mpyq_.c
    r1mpyq.c
    r1updt_.c
    r1updt.c
    rwupdt_.c
    rwupdt.c
)

INCLUDE_DIRECTORIES( 
   ${VTKNEUROENDOSCOPY_SOURCE_DIR}/lib/cminpack 
)


TARGET_LINK_LIBRARIES (cminpack)


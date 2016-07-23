
SRCDIR += alg/thPlateId/src

SRCS_THPLATEID_ALG = thPlateIdAlg_TI_ialg.c

#Other algorithm source files
SRCS_THPLATEID_ALG += thPlateIdAlg_ti_ires.c
SRCS_THPLATEID_ALG += thPlateIdAlg_ti_dma.c
SRCS_THPLATEID_ALG += thPlateIdAlg_ti_dmaOpt.c

SRCS_c6xdsp +=  $(SRCS_THPLATEID_ALG)
INCDIR += $(SRCDIR) $(SRCDIR)/../ $(SRCDIR)/../common


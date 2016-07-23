
SRCDIR += links_c6xdsp/system links_c6xdsp/utils/src \
		links_c6xdsp/alg_link links_c6xdsp/alg_link/swosd links_c6xdsp/alg_link/scd \
		links_c6xdsp/rvm links_c6xdsp/algVehicle_link links_c6xdsp/algVehicle_link/thPlateId

SRCS_UTILS = utils_dsp.c

SRCS_ALGLINK    = algLink_tsk.c algLink_priv.c

SRCS_ALGVEHICLELINK    = algVehicleLink_tsk.c algVehicleLink_priv.c

SRCS_OSDLINK    = osdLink_alg.c  

SRCS_SCDLINK    = scdLink_alg.c 

SRCS_THPLATEIDLINK    = thPlateIdLink_priv.c

SRCS_RVM        = rvmLink_tsk.c

SRCS_SYSTEMDSP  = system_c6xdsp.c systemLink_tsk_c6xdsp.c system_c6xdsp_edma.c

SRCS_c6xdsp += $(SRCS_SYSTEMDSP) $(SRCS_UTILS) $(SRCS_ALGLINK) $(SRCS_OSDLINK) $(SRCS_SCDLINK) $(SRCS_RVM) $(SRCS_ALGVEHICLELINK) $(SRCS_THPLATEIDLINK)



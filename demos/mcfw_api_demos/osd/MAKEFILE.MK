include $(MCFW_ROOT_PATH)/makerules/common_header_a8.mk

IPC_INC=-I$(MCFW_ROOT_PATH)/demos/mcfw_api_demos/multich_usecase 

INCLUDE += $(MCFW_INC) $(OSA_INC) $(STREAM_INC) $(IPNC_APP_INC) $(XDAIS_INC) $(SYSLINK_INC) $(CODEC_INC) $(IPC_INC) $(LOSD_INC) $(COMMON_INC)

include $(MCFW_ROOT_PATH)/makerules/common_footer_a8.mk



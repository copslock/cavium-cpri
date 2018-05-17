
#  standard component Makefile header
sp              :=  $(sp).x
dirstack_$(sp)  :=  $(d)
d               :=  $(dir)

#  component specification

OBJS_$(d) := $(OBJ_DIR)/camellia.o \
	$(OBJ_DIR)/cmll_misc.o \
	$(OBJ_DIR)/cmll_ecb.o \
	$(OBJ_DIR)/cmll_cbc.o \
	$(OBJ_DIR)/cmll_cfb.o \
	$(OBJ_DIR)/cmll_ofb.o \
	$(OBJ_DIR)/cmll_ctr.o \
	$(OBJ_DIR)/cmll_utl.o

$(OBJS_$(d)):  CFLAGS_LOCAL := -I$(d) -I$(d)/.. -I$(d)/../../include \
	-I$(d)/../../include/openssl $(CFLAGS_CRYPTO) #-DCMLL_DEBUG

#  standard component Makefile rules

DEPS_$(d)   :=  $(OBJS_$(d):.o=.d)

CLEAN_LIST  :=  $(CLEAN_LIST) $(OBJS_$(d)) $(DEPS_$(d))

$(OBJ_DIR)/%.o:	$(d)/%.c
	$(COMPILE)

-include $(DEPS_$(d))

#  standard component Makefile footer

d   := $(dirstack_$(sp))
sp  := $(basename $(sp))

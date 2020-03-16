
NAME=ttyapd

OBJOPT=-g
LNKOPT=-lpthread -ljson-c -liwinfo -luci -g
CC=gcc

OBJDIR?=./_obj
ADDIT_DIR:=./
#####################################################################################
#Don't change anything below. Unless you know what you are doing.
#####################################################################################
#add all C-files in each additional dir
SRC_LIST_C_ADD:=$(foreach ad,$(ADDIT_DIR),$(wildcard $(ad)/*.c))
#add all CPP-files in each additional dir
SRC_LIST_CPP_ADD:=$(foreach ad,$(ADDIT_DIR),$(wildcard $(ad)/*.cpp))
#combine all source-files in one variable. Not used in rules. But used in 'echo' rule.
SRC_LIST:= $(SRC_LIST_C_ADD) $(SRC_LIST_CPP_ADD)
#OBJs_names_ADD=$(foreach ad,$(ADDIT_DIR),$(subst $(ad),,$(SRC_LIST_C_ADD:.c=.o))) $(foreach ad,$(ADDIT_DIR),$(subst $(ad),,$(SRC_LIST_CPP_ADD:.cpp=.o)))
OBJs_names_C_ADD=$(foreach ad,$(ADDIT_DIR),$(subst $(ad),,$(wildcard $(ad)/*.c)))
OBJs_names_CPP_ADD=$(foreach ad,$(ADDIT_DIR),$(subst $(ad),,$(wildcard $(ad)/*.cpp)))
OBJs_names_ADD=$(OBJs_names_C_ADD:.c=.o) $(OBJs_names_CPP_ADD:.cpp=.o)
OBJs=$(subst //,/,$(addprefix $(OBJDIR)/, $(OBJs_names_ADD)))

space := $(subst ,, )
VPATH=$(subst $(space),:,$(ADDIT_DIR))

OBJOPT += $(foreach ad,$(filter-out ./,$(ADDIT_DIR)),$(subst //,/,-I$(ad)/include -I$(ad)))

STAMP=$(shell date +%Y%m%d-%H%M%S )

V ?= 0
ifeq ($(V),0)
SILENT:=--silent
endif
verbose = $(verbose_$(V))
verbose_0 = @set -e;

.DEFAULT_GOAL := all

#Rule template for object files for each ADDITIONAL directory.
define RULE_TEMPL
$(OBJDIR)/%.o: $(1)/%.c | $(OBJDIR)
	$(verbose)echo "  GEN [$@]";\
	$(CC) -D VDATE=\"$(STAMP)\" -D BNAME=\"$(NAME)\" -c $< -o $@ $(OBJOPT)

$(OBJDIR)/%.o: $(1)/%.cpp | $(OBJDIR)
	$(verbose)echo "  GEN [$@]";\
	$(CC) -D VDATE=\"$(STAMP)\" -D BNAME=\"$(NAME)\" -c $< -o $@ $(OBJOPT)

endef
$(foreach ad,$(ADDIT_DIR),$(call RULE_TEMPL,$(ad)))

$(OBJDIR)/%.o : %.c | $(OBJDIR)
	$(verbose)echo "  GEN [$@]";\
	$(CC) -D VDATE=\"$(STAMP)\" -D BNAME=\"$(NAME)\" -c $< -o $@ $(OBJOPT)

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(verbose)echo "  GEN [$@]";\
	$(CC) -D VDATE=\"$(STAMP)\" -D BNAME=\"$(NAME)\" -c $< -o $@ $(OBJOPT)

all: $(OBJs)
	$(verbose)echo "  LNK [$(NAME)]";\
	$(CC) $(OBJs) -o $(NAME) $(LNKOPT)

$(OBJDIR):
	$(verbose)mkdir -p $(OBJDIR)

clean: 
	$(verbose)for i in $(OBJs);do echo "  DEL [$$i]";rm -f $$i;done; \
	echo "  DEL [$(NAME)]";\
	rm -f $(NAME);\
	rmdir $(OBJDIR)

install:
	echo "Nothing to do. Just copy '$(NAME)' where you need.";

echo:
	$(verbose)echo "src: $(SRC_LIST)";echo;\
	echo "objs: $(OBJs)";echo;\
	echo "VPATH: $(VPATH)";

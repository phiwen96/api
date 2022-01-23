############### C++ compiler flags ###################
CXX := /usr/bin/clang++-13
CXX_FLAGS = -std=gnu++2b -stdlib=libc++ -fmodules-ts -fmodules -fbuiltin-module-map -fimplicit-modules -fimplicit-module-maps -fprebuilt-module-path=.
# CXX_FLAGS = -std=gnu++2b -fmodules -fbuiltin-module-map

############### External C++ libraries  ###################
LIB_NLOHMANN := -I/opt/homebrew/Cellar/nlohmann-json/3.10.5

################## Project dirs ##################
PROJ_DIR := $(CURDIR)
BUILD_DIR := $(PROJ_DIR)/build
MODULES_DIR := $(BUILD_DIR)/modules
SOURCES_DIR := $(PROJ_DIR)/modules
TARGETS_DIR := $(PROJ_DIR)/targets
OBJECTS_DIR := $(BUILD_DIR)/objects
APPS_DIR := $(BUILD_DIR)/apps

#################### Targets ###############
SERVER := $(BUILD_DIR)/server
CLIENT := $(BUILD_DIR)/client

######################################
SOURCES := $(wildcard $(SOURCES_DIR)/*.cpp)
TARGETS := $(wildcard $(TARGETS_DIR)/*.cpp)

__MODULES := $(subst .cpp,.pcm,$(SOURCES))
_MODULES := $(foreach F,$(__MODULES),$(word $(words $(subst /, ,$F)),$(subst /, ,$F)))
MODULES := $(foreach name, $(_MODULES), $(addprefix $(MODULES_DIR)/, $(name)))

# __APPS := $(subst .cpp,,$(TARGETS))
__APPS = $(basename $(TARGETS))
_APPS := $(foreach F,$(__APPS),$(word $(words $(subst /, ,$F)),$(subst /, ,$F)))
APPS := $(foreach name, $(_APPS), $(addprefix $(APPS_DIR)/, $(name)))

__OBJECTS := $(subst .cpp,.o,$(TARGETS))
_OBJECTS := $(foreach F,$(__OBJECTS),$(word $(words $(subst /, ,$F)),$(subst /, ,$F)))
OBJECTS := $(foreach name, $(_OBJECTS), $(addprefix $(OBJECTS_DIR)/, $(name)))

_BUILD_DIRS := apps modules targets objects docs tests
BUILD_DIRS := $(foreach dir, $(_BUILD_DIRS), $(addprefix $(BUILD_DIR)/, $(dir)))

######################################
all: $(APPS)

######## Test ###########
$(APPS_DIR)/test: $(OBJECTS_DIR)/test.o
	$(CXX) $(CXX_FLAGS) $(OBJECTS_DIR)/test.o -o $@

$(OBJECTS_DIR)/test.o: $(TARGETS_DIR)/test.cpp $(MODULES)
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -o $@

######## Client ###########
$(APPS_DIR)/client: $(OBJECTS_DIR)/client.o
	$(CXX) $(CXX_FLAGS) $(OBJECTS_DIR)/client.o -o $@

$(OBJECTS_DIR)/client.o: $(TARGETS_DIR)/client.cpp $(MODULES)
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -o $@

######## Server ###########
$(APPS_DIR)/server: $(OBJECTS_DIR)/server.o
	$(CXX) $(CXX_FLAGS) $(OBJECTS_DIR)/server.o -o $@

$(OBJECTS_DIR)/server.o: $(TARGETS_DIR)/server.cpp $(MODULES)
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -o $@ 

# $(info $$NAMES is [${NAMES}])


######## Modules ###########
$(MODULES_DIR)/Server.pcm: $(SOURCES_DIR)/Server.cpp $(MODULES_DIR)/Headers.pcm $(MODULES_DIR)/Connection.pcm  $(MODULES_DIR)/Messenger.pcm $(MODULES_DIR)/Client.pcm $(MODULES_DIR)/Http.pcm $(MODULES_DIR)/Core.pcm
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ 

$(MODULES_DIR)/Messenger.pcm: $(SOURCES_DIR)/Messenger.cpp $(MODULES_DIR)/Connection.pcm  $(MODULES_DIR)/Usr.pcm $(MODULES_DIR)/Client.pcm $(MODULES_DIR)/Http.pcm $(MODULES_DIR)/Core.pcm
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ 

$(MODULES_DIR)/Usr.pcm: $(SOURCES_DIR)/Usr.cpp $(MODULES_DIR)/Client.pcm $(MODULES_DIR)/Http.pcm $(MODULES_DIR)/Core.pcm
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ 

$(MODULES_DIR)/Client.pcm: $(SOURCES_DIR)/Client.cpp $(MODULES_DIR)/Connection.pcm $(MODULES_DIR)/Http.pcm $(MODULES_DIR)/Core.pcm
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ 

$(MODULES_DIR)/Connection.pcm: $(SOURCES_DIR)/Connection.cpp $(MODULES_DIR)/Core.pcm
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ 

$(MODULES_DIR)/Http.pcm: $(SOURCES_DIR)/Http.cpp $(MODULES_DIR)/Core.pcm 
	$(CXX) $(CXX_FLAGS) -c $< -Xclang -emit-module-interface -o $@ 

$(MODULES_DIR)/Ready.pcm: $(SOURCES_DIR)/Ready.cpp
	$(CXX) $(CXX_FLAGS) -c $< -Xclang -emit-module-interface -o $@



$(MODULES_DIR)/Core.pcm: $(SOURCES_DIR)/Core.cpp $(MODULES_DIR)/String.pcm $(MODULES_DIR)/Char.pcm $(MODULES_DIR)/Vector.pcm $(MODULES_DIR)/Size.pcm $(MODULES_DIR)/Convertible.pcm $(MODULES_DIR)/Same.pcm
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ 

$(MODULES_DIR)/String.pcm: $(SOURCES_DIR)/String.cpp $(MODULES_DIR)/Char.pcm $(MODULES_DIR)/Size.pcm $(MODULES_DIR)/Convertible.pcm $(MODULES_DIR)/Same.pcm
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ 

$(MODULES_DIR)/Char.pcm: $(SOURCES_DIR)/Char.cpp $(MODULES_DIR)/Convertible.pcm
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ 

$(MODULES_DIR)/Vector.pcm: $(SOURCES_DIR)/Vector.cpp $(MODULES_DIR)/Size.pcm $(MODULES_DIR)/Same.pcm
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ 

$(MODULES_DIR)/Size.pcm: $(SOURCES_DIR)/Size.cpp $(MODULES_DIR)/Convertible.pcm
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ 

$(MODULES_DIR)/Convertible.pcm: $(SOURCES_DIR)/Convertible.cpp
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ 

$(MODULES_DIR)/Same.pcm: $(SOURCES_DIR)/Same.cpp
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@

$(MODULES_DIR)/Headers.pcm: $(SOURCES_DIR)/Headers.cpp
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@

######################################
directories := $(foreach dir, $(BUILD_DIRS), $(shell [ -d $(dir) ] || mkdir -p $(dir)))

clean:
	rm -rf $(BUILD_DIR)/*
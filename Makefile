############### C++ compiler flags ###################
CXX := clang++
CXX_FLAGS = -std=c++2a -stdlib=libc++ -fmodules-ts -fmodules -fbuiltin-module-map -fimplicit-modules -fimplicit-module-maps -fprebuilt-module-path=.

############### External C++ libraries  ###################
LIB_NLOHMANN := -I/opt/homebrew/Cellar/nlohmann-json/3.10.5

LIB_OPENSSL := /opt/homebrew/Cellar/openssl@3/3.0.1

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
	$(CXX) $(CXX_FLAGS) $(OBJECTS_DIR)/test.o -o $@ $(LIB_OPENSSL)/lib/libssl.a $(LIB_OPENSSL)/lib/libcrypto.a

$(OBJECTS_DIR)/test.o: $(TARGETS_DIR)/test.cpp $(MODULES)
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -o $@ $(LIB_NLOHMANN)/include

######## facebook ###########
$(APPS_DIR)/facebook: $(OBJECTS_DIR)/facebook.o
	$(CXX) $(CXX_FLAGS) $< -o $@

$(OBJECTS_DIR)/facebook.o: $(TARGETS_DIR)/facebook.cpp $(MODULES)
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -o $@ $(LIB_NLOHMANN)/include

######## movie ###########
$(APPS_DIR)/movie: $(OBJECTS_DIR)/movie.o
	$(CXX) $(CXX_FLAGS) $< -o $@ $(LIB_OPENSSL)/lib/libssl.a $(LIB_OPENSSL)/lib/libcrypto.a

$(OBJECTS_DIR)/movie.o: $(TARGETS_DIR)/movie.cpp $(MODULES)
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -o $@ $(LIB_NLOHMANN)/include -I/$(LIB_OPENSSL)/include

######## spotify ###########
$(APPS_DIR)/spotify: $(OBJECTS_DIR)/spotify.o
	$(CXX) $(CXX_FLAGS) $< -o $@

$(OBJECTS_DIR)/spotify.o: $(TARGETS_DIR)/spotify.cpp $(MODULES)
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -o $@ $(LIB_NLOHMANN)/include

######## Client ###########
$(APPS_DIR)/client: $(OBJECTS_DIR)/client.o
	$(CXX) $(CXX_FLAGS) $(OBJECTS_DIR)/client.o -o $@

$(OBJECTS_DIR)/client.o: $(TARGETS_DIR)/client.cpp $(MODULES)
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -o $@ $(LIB_NLOHMANN)/include

######## Server ###########
$(APPS_DIR)/server: $(OBJECTS_DIR)/server.o
	$(CXX) $(CXX_FLAGS) $(OBJECTS_DIR)/server.o -o $@

$(OBJECTS_DIR)/server.o: $(TARGETS_DIR)/server.cpp $(MODULES)
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -o $@ $(LIB_NLOHMANN)/include

# $(info $$NAMES is [${NAMES}])


######## Modules ###########
$(MODULES_DIR)/Server.pcm: $(SOURCES_DIR)/Server.cpp $(MODULES_DIR)/RemoteClient.pcm $(MODULES_DIR)/Socket.pcm $(MODULES_DIR)/Connection.pcm $(MODULES_DIR)/Caller.pcm $(MODULES_DIR)/Messenger.pcm $(MODULES_DIR)/Client.pcm $(MODULES_DIR)/Http.pcm $(MODULES_DIR)/Common.pcm
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ $(LIB_NLOHMANN)/include

$(MODULES_DIR)/RemoteClient.pcm: $(SOURCES_DIR)/RemoteClient.cpp
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ $(LIB_NLOHMANN)/include

$(MODULES_DIR)/Email.pcm: $(SOURCES_DIR)/Email.cpp $(MODULES_DIR)/RemoteServer.pcm
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ $(LIB_NLOHMANN)/include -I/$(LIB_OPENSSL)/include

$(MODULES_DIR)/RemoteServer.pcm: $(SOURCES_DIR)/RemoteServer.cpp
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ $(LIB_NLOHMANN)/include

$(MODULES_DIR)/Messenger.pcm: $(SOURCES_DIR)/Messenger.cpp $(MODULES_DIR)/Connection.pcm $(MODULES_DIR)/Caller.pcm $(MODULES_DIR)/Usr.pcm $(MODULES_DIR)/Client.pcm $(MODULES_DIR)/Http.pcm $(MODULES_DIR)/Common.pcm
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ $(LIB_NLOHMANN)/include

$(MODULES_DIR)/Caller.pcm: $(SOURCES_DIR)/Caller.cpp $(MODULES_DIR)/Connection.pcm $(MODULES_DIR)/Client.pcm $(MODULES_DIR)/Http.pcm $(MODULES_DIR)/Common.pcm
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ $(LIB_NLOHMANN)/include

$(MODULES_DIR)/Usr.pcm: $(SOURCES_DIR)/Usr.cpp $(MODULES_DIR)/Client.pcm $(MODULES_DIR)/Http.pcm $(MODULES_DIR)/Common.pcm
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ $(LIB_NLOHMANN)/include

$(MODULES_DIR)/Client.pcm: $(SOURCES_DIR)/Client.cpp $(MODULES_DIR)/Connection.pcm $(MODULES_DIR)/Http.pcm $(MODULES_DIR)/Common.pcm
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ $(LIB_NLOHMANN)/include

$(MODULES_DIR)/Connection.pcm: $(SOURCES_DIR)/Connection.cpp $(MODULES_DIR)/Common.pcm $(MODULES_DIR)/Socket.pcm
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ $(LIB_NLOHMANN)/include

$(MODULES_DIR)/Http.pcm: $(SOURCES_DIR)/Http.cpp $(MODULES_DIR)/Common.pcm 
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ 

$(MODULES_DIR)/Socket.pcm: $(SOURCES_DIR)/Socket.cpp $(MODULES_DIR)/Common.pcm 
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ 

$(MODULES_DIR)/Common.pcm: $(SOURCES_DIR)/Common.cpp
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ 

$(MODULES_DIR)/Ready.pcm: $(SOURCES_DIR)/Ready.cpp
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ 

$(MODULES_DIR)/Facebook.pcm: $(SOURCES_DIR)/Facebook.cpp $(MODULES_DIR)/API.pcm
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ 

$(MODULES_DIR)/Movie.pcm: $(SOURCES_DIR)/Movie.cpp $(MODULES_DIR)/API.pcm
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ 

$(MODULES_DIR)/Spotify.pcm: $(SOURCES_DIR)/Spotify.cpp $(MODULES_DIR)/API.pcm
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ 

$(MODULES_DIR)/API.pcm: $(SOURCES_DIR)/API.cpp
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@ 



######################################
directories := $(foreach dir, $(BUILD_DIRS), $(shell [ -d $(dir) ] || mkdir -p $(dir)))

clean:
	rm -rf $(BUILD_DIR)/*
############### C++ compiler flags ###################
# CXX := /usr/bin/clang++-13
CXX := /usr/bin/g++-11
# CXX_FLAGS = -std=gnu++2b -stdlib=libc++ -fmodules-ts -fmodules -fbuiltin-module-map -fimplicit-modules -fimplicit-module-maps -fprebuilt-module-path=.
# CXX_FLAGS = -std=gnu++2b -fmodules -fbuiltin-module-map
CXX_FLAGS = -std=c++20 -fmodules-ts
############### External C++ libraries  ###################
# LIB_NLOHMANN := -I/opt/homebrew/Cellar/nlohmann-json/3.10.5
LIB_NLOHMANN := /usr/local/include/nlohmann/json.hpp

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

__MODULES := $(subst .cpp,.o,$(SOURCES))
_MODULES := $(foreach F,$(__MODULES),$(word $(words $(subst /, ,$F)),$(subst /, ,$F)))
MODULES := $(foreach name, $(_MODULES), $(addprefix $(MODULES_DIR)/, $(name)))

# $(info $$MODULES is [${MODULES}])
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
# all: $(APPS)
all: $(APPS_DIR)/client $(APPS_DIR)/server

######## Test ###########
$(APPS_DIR)/test: $(OBJECTS_DIR)/test.o
	$(CXX) $(CXX_FLAGS) $(OBJECTS_DIR)/test.o -o $@

$(OBJECTS_DIR)/test.o: $(TARGETS_DIR)/test.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@





######## Client ###########
# $(APPS_DIR)/client: $(OBJECTS_DIR)/client.o $(MODULES_DIR)/RemoteServer.o $(MODULES_DIR)/Core.o $(MODULES_DIR)/Buffer.o $(MODULES_DIR)/Size.o $(MODULES_DIR)/Convertible.o $(MODULES_DIR)/Same.o#gcm.cache/Email.gcm gcm.cache/RemoteServer.gcm gcm.cache/Usr.gcm gcm.cache/Http.gcm gcm.cache/Connection.gcm gcm.cache/RemoteClient.gcm gcm.cache/Core.gcm gcm.cache/Buffer.gcm gcm.cache/Reference.gcm gcm.cache/Pointer.gcm gcm.cache/String.gcm gcm.cache/Char.gcm gcm.cache/Vector.gcm gcm.cache/Size.gcm gcm.cache/Convertible.gcm gcm.cache/Same.gcm#$(MODULES_DIR)/Email.o $(MODULES_DIR)/RemoteServer.o $(MODULES_DIR)/Usr.o $(MODULES_DIR)/Http.o $(MODULES_DIR)/Connection.o $(MODULES_DIR)/RemoteClient.o $(MODULES_DIR)/Core.o $(MODULES_DIR)/Buffer.o $(MODULES_DIR)/Reference.o $(MODULES_DIR)/Pointer.o $(MODULES_DIR)/String.o $(MODULES_DIR)/Char.o $(MODULES_DIR)/Vector.o $(MODULES_DIR)/Size.o $(MODULES_DIR)/Convertible.o $(MODULES_DIR)/Same.o
# 	$(CXX) $(CXX_FLAGS) -o $@ $^ 
# $(APPS_DIR)/client: $(TARGETS_DIR)/client.cpp $(SOURCES_DIR)/RemoteServer.cpp $(SOURCES_DIR)/Core.cpp $(SOURCES_DIR)/Buffer.cpp $(SOURCES_DIR)/Size.cpp $(SOURCES_DIR)/Convertible.cpp $(SOURCES_DIR)/Same.cpp#gcm.cache/Email.gcm gcm.cache/RemoteServer.gcm gcm.cache/Usr.gcm gcm.cache/Http.gcm gcm.cache/Connection.gcm gcm.cache/RemoteClient.gcm gcm.cache/Core.gcm gcm.cache/Buffer.gcm gcm.cache/Reference.gcm gcm.cache/Pointer.gcm gcm.cache/String.gcm gcm.cache/Char.gcm gcm.cache/Vector.gcm gcm.cache/Size.gcm gcm.cache/Convertible.gcm gcm.cache/Same.gcm#$(MODULES_DIR)/Email.o $(MODULES_DIR)/RemoteServer.o $(MODULES_DIR)/Usr.o $(MODULES_DIR)/Http.o $(MODULES_DIR)/Connection.o $(MODULES_DIR)/RemoteClient.o $(MODULES_DIR)/Core.o $(MODULES_DIR)/Buffer.o $(MODULES_DIR)/Reference.o $(MODULES_DIR)/Pointer.o $(MODULES_DIR)/String.o $(MODULES_DIR)/Char.o $(MODULES_DIR)/Vector.o $(MODULES_DIR)/Size.o $(MODULES_DIR)/Convertible.o $(MODULES_DIR)/Same.o
# 	$(CXX) $(CXX_FLAGS) -o $@ $^ 
$(APPS_DIR)/client: $(SOURCES_DIR)/Convertible.cpp $(SOURCES_DIR)/Same.cpp $(SOURCES_DIR)/Buffer.cpp $(SOURCES_DIR)/Json.cpp $(SOURCES_DIR)/Core.cpp $(SOURCES_DIR)/RemoteServer.cpp $(TARGETS_DIR)/client.cpp# $(TARGETS_DIR)/client.cpp#gcm.cache/Email.gcm gcm.cache/RemoteServer.gcm gcm.cache/Usr.gcm gcm.cache/Http.gcm gcm.cache/Connection.gcm gcm.cache/RemoteClient.gcm gcm.cache/Core.gcm gcm.cache/Buffer.gcm gcm.cache/Reference.gcm gcm.cache/Pointer.gcm gcm.cache/String.gcm gcm.cache/Char.gcm gcm.cache/Vector.gcm gcm.cache/Size.gcm gcm.cache/Convertible.gcm gcm.cache/Same.gcm#$(MODULES_DIR)/Email.o $(MODULES_DIR)/RemoteServer.o $(MODULES_DIR)/Usr.o $(MODULES_DIR)/Http.o $(MODULES_DIR)/Connection.o $(MODULES_DIR)/RemoteClient.o $(MODULES_DIR)/Core.o $(MODULES_DIR)/Buffer.o $(MODULES_DIR)/Reference.o $(MODULES_DIR)/Pointer.o $(MODULES_DIR)/String.o $(MODULES_DIR)/Char.o $(MODULES_DIR)/Vector.o $(MODULES_DIR)/Size.o $(MODULES_DIR)/Convertible.o $(MODULES_DIR)/Same.o
	$(CXX) $(CXX_FLAGS) -c -x c++-system-header iostream vector array utility string coroutine thread chrono
	$(CXX) $(CXX_FLAGS) $^ -o $@

$(APPS_DIR)/server: $(SOURCES_DIR)/Convertible.cpp $(SOURCES_DIR)/Same.cpp $(SOURCES_DIR)/Buffer.cpp $(SOURCES_DIR)/Json.cpp $(SOURCES_DIR)/Core.cpp $(SOURCES_DIR)/RemoteClient.cpp $(SOURCES_DIR)/RemoteServer.cpp $(SOURCES_DIR)/Email.cpp $(SOURCES_DIR)/Server.cpp $(TARGETS_DIR)/server.cpp# $(TARGETS_DIR)/client.cpp#gcm.cache/Email.gcm gcm.cache/RemoteServer.gcm gcm.cache/Usr.gcm gcm.cache/Http.gcm gcm.cache/Connection.gcm gcm.cache/RemoteClient.gcm gcm.cache/Core.gcm gcm.cache/Buffer.gcm gcm.cache/Reference.gcm gcm.cache/Pointer.gcm gcm.cache/String.gcm gcm.cache/Char.gcm gcm.cache/Vector.gcm gcm.cache/Size.gcm gcm.cache/Convertible.gcm gcm.cache/Same.gcm#$(MODULES_DIR)/Email.o $(MODULES_DIR)/RemoteServer.o $(MODULES_DIR)/Usr.o $(MODULES_DIR)/Http.o $(MODULES_DIR)/Connection.o $(MODULES_DIR)/RemoteClient.o $(MODULES_DIR)/Core.o $(MODULES_DIR)/Buffer.o $(MODULES_DIR)/Reference.o $(MODULES_DIR)/Pointer.o $(MODULES_DIR)/String.o $(MODULES_DIR)/Char.o $(MODULES_DIR)/Vector.o $(MODULES_DIR)/Size.o $(MODULES_DIR)/Convertible.o $(MODULES_DIR)/Same.o
	$(CXX) $(CXX_FLAGS) $^ -o $@

# $(OBJECTS_DIR)/client.o: $(TARGETS_DIR)/client.cpp #$(MODULES_DIR)/RemoteServer.o $(MODULES_DIR)/Core.o $(MODULES_DIR)/Buffer.o $(MODULES_DIR)/Size.o $(MODULES_DIR)/Convertible.o $(MODULES_DIR)/Same.o#gcm.cache/Email.gcm gcm.cache/RemoteServer.gcm gcm.cache/Usr.gcm gcm.cache/Http.gcm gcm.cache/Connection.gcm gcm.cache/RemoteClient.gcm gcm.cache/Core.gcm gcm.cache/Buffer.gcm gcm.cache/Reference.gcm gcm.cache/Pointer.gcm gcm.cache/String.gcm gcm.cache/Char.gcm gcm.cache/Vector.gcm gcm.cache/Size.gcm gcm.cache/Convertible.gcm gcm.cache/Same.gcm#$(MODULES_DIR)/Email.o $(MODULES_DIR)/RemoteServer.o $(MODULES_DIR)/Usr.o $(MODULES_DIR)/Http.o $(MODULES_DIR)/Connection.o $(MODULES_DIR)/RemoteClient.o $(MODULES_DIR)/Core.o $(MODULES_DIR)/Buffer.o $(MODULES_DIR)/Reference.o $(MODULES_DIR)/Pointer.o $(MODULES_DIR)/String.o $(MODULES_DIR)/Char.o $(MODULES_DIR)/Vector.o $(MODULES_DIR)/Size.o $(MODULES_DIR)/Convertible.o $(MODULES_DIR)/Same.o
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@

# $(OBJECTS_DIR)/libDelta.so: $(SOURCES_DIR)/Delta.cpp $(SOURCES_DIR)/Same.cpp $(SOURCES_DIR)/Convertible.cpp
# 	$(CXX) $(CXX_FLAGS) -shared $^ -o $@

# $(OBJECTS_DIR)/Delta.o: $(SOURCES_DIR)/Delta.cpp 
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@

# $(OBJECTS_DIR)/client.o: $(TARGETS_DIR)/client.cpp gcm.cache/Email.gcm gcm.cache/RemoteServer.gcm gcm.cache/Usr.gcm gcm.cache/Http.gcm gcm.cache/Connection.gcm gcm.cache/RemoteClient.gcm gcm.cache/Core.gcm gcm.cache/Buffer.gcm gcm.cache/Reference.gcm gcm.cache/Pointer.gcm gcm.cache/String.gcm gcm.cache/Char.gcm gcm.cache/Vector.gcm gcm.cache/Size.gcm gcm.cache/Convertible.gcm gcm.cache/Same.gcm
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@ $(filter-out, $<, $^)

# $(OBJECTS_DIR)/client.o: $(TARGETS_DIR)/client.cpp $(MODULES_DIR)/Email.o $(MODULES_DIR)/RemoteServer.o $(MODULES_DIR)/Client.o $(MODULES_DIR)/Usr.o $(MODULES_DIR)/Http.o $(MODULES_DIR)/Connection.o $(MODULES_DIR)/RemoteClient.o $(MODULES_DIR)/Core.o $(MODULES_DIR)/Buffer.o $(MODULES_DIR)/Reference.o $(MODULES_DIR)/Pointer.o $(MODULES_DIR)/String.o $(MODULES_DIR)/Char.o $(MODULES_DIR)/Vector.o $(MODULES_DIR)/Size.o $(MODULES_DIR)/Convertible.o $(MODULES_DIR)/Same.o
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@ $(filter-out, $<, $^)

######## Server ###########
# $(APPS_DIR)/server: $(OBJECTS_DIR)/server.o $(MODULES_DIR)/Email.o $(MODULES_DIR)/RemoteServer.o $(MODULES_DIR)/Server.o $(MODULES_DIR)/Usr.o $(MODULES_DIR)/Http.o $(MODULES_DIR)/RemoteClient.o $(MODULES_DIR)/Core.o $(MODULES_DIR)/Buffer.o $(MODULES_DIR)/Reference.o $(MODULES_DIR)/Pointer.o $(MODULES_DIR)/String.o $(MODULES_DIR)/Char.o $(MODULES_DIR)/Vector.o $(MODULES_DIR)/Size.o $(MODULES_DIR)/Convertible.o $(MODULES_DIR)/Same.o
# 	$(CXX) $(CXX_FLAGS) -o $@ $^

# $(OBJECTS_DIR)/server.o: $(TARGETS_DIR)/server.cpp $(MODULES_DIR)/Email.o $(MODULES_DIR)/RemoteServer.o $(MODULES_DIR)/Server.o $(MODULES_DIR)/Usr.o $(MODULES_DIR)/Http.o $(MODULES_DIR)/RemoteClient.o $(MODULES_DIR)/Core.o $(MODULES_DIR)/Buffer.o $(MODULES_DIR)/Reference.o $(MODULES_DIR)/Pointer.o $(MODULES_DIR)/String.o $(MODULES_DIR)/Char.o $(MODULES_DIR)/Vector.o $(MODULES_DIR)/Size.o $(MODULES_DIR)/Convertible.o $(MODULES_DIR)/Same.o
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@ $(filter-out, $<, $^)
# $(info $$NAMES is [${NAMES}])


######## Modules ###########
# $(MODULES_DIR)/Server.o: $(SOURCES_DIR)/Server.cpp $(MODULES_DIR)/RemoteClient.o $(MODULES_DIR)/Http.o $(MODULES_DIR)/Core.o $(MODULES_DIR)/Connection.o #$(MODULES_DIR)/Messenger.o $(MODULES_DIR)/Client.o $(MODULES_DIR)/Http.o $(MODULES_DIR)/Core.o
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@ 

# $(MODULES_DIR)/Messenger.o: $(SOURCES_DIR)/Messenger.cpp $(MODULES_DIR)/Connection.o  $(MODULES_DIR)/Usr.o $(MODULES_DIR)/Client.o $(MODULES_DIR)/Http.o $(MODULES_DIR)/Core.o
# 	$(CXX) $(CXX_F
# 	LAGS) -c $< -o $@ 

# $(MODULES_DIR)/Usr.o: $(SOURCES_DIR)/Usr.cpp $(MODULES_DIR)/Client.o $(MODULES_DIR)/Http.o $(MODULES_DIR)/Core.o
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@ 

# $(MODULES_DIR)/Client.o: $(SOURCES_DIR)/Client.cpp $(MODULES_DIR)/Connection.o $(MODULES_DIR)/Http.o $(MODULES_DIR)/Core.o
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@ 

# $(MODULES_DIR)/Connection.o: $(SOURCES_DIR)/Connection.cpp $(MODULES_DIR)/Core.o
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@ 

# $(MODULES_DIR)/Usr.o: $(SOURCES_DIR)/Usr.cpp $(MODULES_DIR)/RemoteClient.o $(MODULES_DIR)/Core.o 
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@ 

# $(MODULES_DIR)/Http.o: $(SOURCES_DIR)/Http.cpp $(MODULES_DIR)/Core.o 
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@ 

# $(MODULES_DIR)/NetStream.o: $(SOURCES_DIR)/NetStream.cpp $(MODULES_DIR)/Core.o 
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@ 

# $(MODULES_DIR)/Email.o: $(SOURCES_DIR)/Email.cpp $(MODULES_DIR)/RemoteServer.o $(MODULES_DIR)/Core.o 
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@ -lssl -lcrypto

# $(MODULES_DIR)/RemoteServer.o: $(SOURCES_DIR)/RemoteServer.cpp $(MODULES_DIR)/Core.o 
#	$(CXX) $(CXX_FLAGS) -c $< -o $@ 

# $(MODULES_DIR)/RemoteClient.o: $(SOURCES_DIR)/RemoteClient.cpp $(MODULES_DIR)/Core.o 
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@ 




# # $(MODULES_DIR)/Ready.o: $(SOURCES_DIR)/Ready.cpp
# # 	$(CXX) $(CXX_FLAGS) -c $< -o $@



# $(MODULES_DIR)/Core.o: $(SOURCES_DIR)/Core.cpp $(MODULES_DIR)/Buffer.o $(MODULES_DIR)/Size.o $(MODULES_DIR)/Convertible.o $(MODULES_DIR)/Same.o
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@

# $(MODULES_DIR)/String.o: $(SOURCES_DIR)/String.cpp $(MODULES_DIR)/Char.o $(MODULES_DIR)/Size.o $(MODULES_DIR)/Convertible.o $(MODULES_DIR)/Same.o
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@

# $(MODULES_DIR)/Char.o: $(SOURCES_DIR)/Char.cpp $(MODULES_DIR)/Convertible.o
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@

# $(MODULES_DIR)/Vector.o: $(SOURCES_DIR)/Vector.cpp $(MODULES_DIR)/Size.o $(MODULES_DIR)/Same.o
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@

# $(MODULES_DIR)/Buffer.o: $(SOURCES_DIR)/Buffer.cpp $(MODULES_DIR)/Size.o $(MODULES_DIR)/Same.o
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@

# $(MODULES_DIR)/Reference.o: $(SOURCES_DIR)/Reference.cpp
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@

# $(MODULES_DIR)/Pointer.o: $(SOURCES_DIR)/Pointer.cpp
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@

# $(MODULES_DIR)/Size.o: $(SOURCES_DIR)/Size.cpp $(MODULES_DIR)/Convertible.o
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@

# $(MODULES_DIR)/Convertible.o: $(SOURCES_DIR)/Convertible.cpp
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@

# $(MODULES_DIR)/Same.o: $(SOURCES_DIR)/Same.cpp
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@

# $(MODULES_DIR)/Headers.o: $(SOURCES_DIR)/Headers.cpp
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@

######################################
directories := $(foreach dir, $(BUILD_DIRS), $(shell [ -d $(dir) ] || mkdir -p $(dir)))

clean:
	rm -rf $(BUILD_DIR)/* && rm -rf gcm.cache
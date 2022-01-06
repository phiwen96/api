CXX := clang++
CXX_FLAGS = -std=c++2a -stdlib=libc++ -fmodules-ts -fmodules -fbuiltin-module-map -fimplicit-modules -fimplicit-module-maps -fprebuilt-module-path=.


PROJ_DIR := $(CURDIR)
BUILD_DIR := $(PROJ_DIR)/build
OBJ_DIR := $(BUILD_DIR)/obj
MODULES_DIR := $(PROJ_DIR)/modules

MAIN := $(BUILD_DIR)/main

MODULES := $(wildcard $(MODULES_DIR)/*.cpp)

__OBJ := $(subst .cpp,.pcm,$(MODULES))
_OBJ := $(foreach F,$(__OBJ),$(word $(words $(subst /, ,$F)),$(subst /, ,$F)))
OBJ := $(foreach name, $(_OBJ), $(addprefix $(OBJ_DIR)/, $(name)))


$(MAIN): $(OBJ_DIR)/main.o $(MODULES)
	$(CXX) $(CXX_FLAGS) $(OBJ_DIR)/server.o -o $@

$(OBJ_DIR)/main.o: $(PROJ_DIR)/main.cpp $(LIB)
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@

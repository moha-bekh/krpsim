CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++17 -g3
INCLUDES := -Iinclude
DEPFLAGS := -MMD -MP

BUILD_DIR := build

COMMON_SRCS := src/MockConfigs.cpp src/Simulator.cpp src/Solver.cpp
KRPSIM_SRCS := src/KrpsimMain.cpp $(COMMON_SRCS)
VERIF_SRCS := src/KrpsimVerifMain.cpp $(COMMON_SRCS)

KRPSIM_OBJS := $(KRPSIM_SRCS:src/%.cpp=$(BUILD_DIR)/%.o)
VERIF_OBJS := $(VERIF_SRCS:src/%.cpp=$(BUILD_DIR)/%.o)
DEPS := $(KRPSIM_OBJS:.o=.d) $(VERIF_OBJS:.o=.d)

KRPSIM := $(BUILD_DIR)/krpsim
KRPSIM_VERIF := $(BUILD_DIR)/krpsim_verif

all: $(KRPSIM) $(KRPSIM_VERIF)

$(KRPSIM): $(KRPSIM_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(KRPSIM_VERIF): $(VERIF_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(INCLUDES) $(DEPFLAGS) $(CXXFLAGS) -c $< -o $@

run: $(KRPSIM)
	./$(KRPSIM)

verif: $(KRPSIM_VERIF)
	./$(KRPSIM_VERIF)

clean:
	rm -rf $(BUILD_DIR)

fclean: clean

re: fclean all

.PHONY: all clean fclean re run verif

-include $(DEPS)

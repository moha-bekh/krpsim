CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++17 -Iinclude

KRPSIM := krpsim
KRPSIM_VERIF := krpsim_verif

KRPSIM_SRCS := src/krpsim_main.cpp
VERIF_SRCS := src/krpsim_verif_main.cpp

.PHONY: all clean fclean re

all: $(KRPSIM) $(KRPSIM_VERIF)

$(KRPSIM): $(KRPSIM_SRCS)
	$(CXX) $(CXXFLAGS) $(KRPSIM_SRCS) -o $(KRPSIM)

$(KRPSIM_VERIF): $(VERIF_SRCS)
	$(CXX) $(CXXFLAGS) $(VERIF_SRCS) -o $(KRPSIM_VERIF)

clean:

fclean: clean
	rm -f $(KRPSIM) $(KRPSIM_VERIF)

re: fclean all

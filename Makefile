CXXFLAGS = -Wall -Wextra -O2 -std=c++17

.PHONY: all
all: wallet_example

wallet_example: zadanie/wallet_example.o src/wallet.o
	$(CXX) $^ $(CXXFLAGS) -o $@

zadanie/%: CXXFLAGS += -I src

%.o: %.cc
	$(CXX) $< $(CXXFLAGS) -c -o $@

clean:
	rm -f wallet_example
	find zadanie src -type f -name '*.o' | xargs rm -f

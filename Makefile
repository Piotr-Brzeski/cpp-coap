LIBRARY_NAME=libcpp-coap.a
LIBRARY_SOURCES=cpp-coap/context.cpp cpp-coap/session.cpp cpp-coap/client.cpp cpp-coap/server.cpp

SERVER_NAME=server
SERVER_SOURCES=server/server.cpp

CLIENT_NAME=client
CLIENT_SOURCES=client/client.cpp

COMPILER_OPTIONS=-O3 -I/opt/homebrew/include -I.
LINKER_OPTIONS=-L/opt/homebrew/lib -lcoap-3-openssl

################################

BUILD_DIR=BUILD
INTERMEDIATES_DIR=$(BUILD_DIR)/Intermediates

all: $(BUILD_DIR)/$(LIBRARY_NAME) $(BUILD_DIR)/$(SERVER_NAME) $(BUILD_DIR)/$(CLIENT_NAME)

$(BUILD_DIR)/$(SERVER_NAME): $(SERVER_SOURCES:%.cpp=$(INTERMEDIATES_DIR)/%.o) $(BUILD_DIR)/$(LIBRARY_NAME)
	@mkdir -p `dirname "$@"`
	g++ $(LINKER_OPTIONS) -o $@ $^ 

$(BUILD_DIR)/$(CLIENT_NAME): $(CLIENT_SOURCES:%.cpp=$(INTERMEDIATES_DIR)/%.o) $(BUILD_DIR)/$(LIBRARY_NAME)
	@mkdir -p `dirname "$@"`
	g++ $(LINKER_OPTIONS) -o $@ $^ 

$(INTERMEDIATES_DIR)/%.o: %.cpp
	@mkdir -p `dirname "$@"`
	g++ -c -g -std=c++20 $(COMPILER_OPTIONS) $< -o $@

$(BUILD_DIR)/%.a: $(LIBRARY_SOURCES:%.cpp=$(INTERMEDIATES_DIR)/%.o)
	@mkdir -p `dirname "$@"`
	ar rcs $@ $^

clean:
	rm -rf $(BUILD_DIR)

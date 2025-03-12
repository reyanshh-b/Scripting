CC = g++                  # The compiler you're using
CFLAGS = -Wall             # Compiler flags to show warnings
SOURCES = main.cpp         # Your source files
OBJECTS = $(SOURCES:.cpp=.o)  # Converts .cpp to .o for object files
EXEC = main                # Name of the final executable

# Default target: all
all: $(EXEC)

# How to build the executable from object files
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC)

# How to build object files from source files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(OBJECTS) $(EXEC)

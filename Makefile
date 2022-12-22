CC = clang++
src_dir = src
obj_dir = obj
header_dir = include
# flags = -std=c++20 -D _CRT_SECURE_NO_WARNINGS -D _USE_MATH_DEFINES -Wall -Wextra -I $(header_dir)
# flags = -fopenmp -std=c++20 -D _CRT_SECURE_NO_WARNINGS -D _USE_MATH_DEFINES -Wall -Wextra -O2 -I $(header_dir)
flags = -std=c++20 -D _CRT_SECURE_NO_WARNINGS -D _USE_MATH_DEFINES -O2 -fopenmp -I include

headers = $(wildcard $(header_dir)/*.hpp)
sources = $(wildcard $(src_dir)/*.cpp)
objects = $(patsubst $(src_dir)/%.cpp,$(obj_dir)/%.o,$(sources))
product_name = omp4

$(product_name): obj $(objects)
	@echo Compiling of $(sources) finished
	@$(CC) $(flags) -o $(product_name) $(objects)

all: $(product_name)

obj: 
	@echo Directory obj created
	@mkdir obj

$(obj_dir)/%.o: $(src_dir)/%.cpp $(headers)
	@echo Compiling $@ from $<
	@$(CC) $(flags) -o $@ -c $<

clean:
	@rm -rf obj $(product_name)

single: all
	@./$(product_name) -1 test_data/in.pgm out.pgm

parallel: all
	@./$(product_name) 2 test_data/in.pgm out.pgm


.PHONY: clean build

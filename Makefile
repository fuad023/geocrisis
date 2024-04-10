CXX	      = g++
OUTPUT    = bin\sfmlgame

CXX_FLAGS = -O3 -std=c++20 -Wno-unused-result
SFML_DIR  = C:\dev\libraries\SFML-2.5.1
SFML_H    = $(SFML_DIR)\include
LDFLAGS   = -lsfml-graphics -lsfml-window -lsfml-system -lopengl32 -LC:\dev\libraries\SFML-2.5.1\lib
		  
SRCDIR 	  = src
OBJDIR    = obj

SRC_M	  = $(wildcard src/*.cpp)
SRC_ImGUI = $(wildcard src/imgui/*.cpp)
OBJ_FILES = $(SRC_M:src/%.cpp=obj/%.o) $(SRC_ImGUI:src/imgui/%.cpp=obj/%.o)

all: $(OUTPUT)

$(OUTPUT): $(OBJ_FILES)
	$(CXX) $^ -o $@    $(LDFLAGS)

obj/%.o: src/%.cpp
	$(CXX) $(CXX_FLAGS) -I$(SFML_H) -o $@ -c $<

obj/%.o: src/imgui/%.cpp
	$(CXX) $(CXX_FLAGS) -I$(SFML_H) -o $@ -c $<

clean:
	cd obj && rm -f *.o && cd .. && cd bin && rm -f *.exe

run: $(OUTPUT)
	cd bin && ./sfmlgame.exe && cd ..

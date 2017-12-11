CC := g++
CXXFLAGS := -Wall -g -std=c++11
INC := -I./include

target1 := bin/Agenda
target2 := bin/AgendaClient

all : $(target1) $(target2)

$(target1) : build/AgendaSocket.o build/Agenda.o build/AgendaUI.o build/AgendaService.o build/Date.o build/Meeting.o build/Storage.o build/User.o
	@mkdir -p bin
	$(CC) $(CXXFLAGS) $(INC) $^ -o $@

$(target2) : src/AgendaClient.cpp
	@mkdir -p bin
	$(CC) $(CXXFLAGS) $(INC) $< -o $@

# build/Agenda.o : src/main/Agenda.cpp
# 	@mkdir -p build
# 	$(CC) $(CXXFLAGS) $(INC) $< -o $@


build/%.o: src/%.cpp
	@mkdir -p build
	$(CC) $(CXXFLAGS) $(INC) -c $< -o $@

clean:
	@echo "Cleaning..."
	rm -rf build/
	rm -rf bin/

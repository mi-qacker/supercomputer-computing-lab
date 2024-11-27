# Имя выходного файла
TARGET = main

# Компилятор и флаги
CXX = mpic++
CXXFLAGS = -Wall -Wextra

# number of processes
NP = 4

# Исходный файл
SRCS = src/main.cpp

# Правило по умолчанию
all: clean $(TARGET)

# Правило компиляции
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

# Правило для запуска программы
run: $(TARGET)
	./$(TARGET)

# Правило для запуска программы
run-mpi: $(TARGET)
	mpiexec -n $(NP) ./$(TARGET)

# Очистка скомпилированных файлов
clean:
	rm -f $(TARGET)

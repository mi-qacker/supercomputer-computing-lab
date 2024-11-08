# Имя выходного файла
TARGET = main

# Компилятор и флаги
CXX = g++
CXXFLAGS = -Wall -Wextra

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

# Очистка скомпилированных файлов
clean:
	rm -f $(TARGET)

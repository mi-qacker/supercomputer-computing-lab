#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono;

int main()
{
  const int N = 10000;
  const int N_T = 10000;
  const double LAMBDA = 46;
  const double RHO = 7800;
  const double C = 460;

  // окончание по времени
  double t_end = 60;
  // толщина пластины
  double L = 0.1;
  // начальная температура
  double T0 = 20;
  // температуру на границе х=0
  double Tl = 300;
  // температуру на границе х=L
  double Tr = 100;
  vector<double> T;

  // Запоминаем начало времени
  auto start = high_resolution_clock::now();
  // Определяем количество доступных потоков
  int num_threads = std::thread::hardware_concurrency();

  // Определяем расчетный шаг сетки по пространственной координате
  double h = L / (N - 1);
  // Определяем расчетный шаг сетки по времени
  double tau = t_end / N_T;

  double ai = LAMBDA / (h * h);
  double bi = 2.0 * LAMBDA / (h * h) + RHO * C / tau;
  double ci = LAMBDA / (h * h);

  // Определяем поле температуры в начальный момент времени
  T.resize(N);
  for (int i = 0; i < N; ++i)
  {
    T[i] = T0;
  }

  // Проводим интегрирование
  auto compute_temperature = [&](int start, int end)
  {
    vector<double> alfa, beta;
    alfa.resize(N);
    beta.resize(N);

    double time = start;
    while (time < end)
    {
      time += tau;

      // Начальные прогоночные коэффициенты
      alfa[0] = 0.0;
      beta[0] = Tl;

      // Цикл для определения прогоночных коэффициентов
      for (int i = 1; i < N - 1; ++i)
      {
        double fi = -RHO * C * T[i] / tau;

        alfa[i] = ai / (bi - ci * alfa[i - 1]);
        beta[i] = (ci * beta[i - 1] - fi) / (bi - ci * alfa[i - 1]);
      }

      // Значение температуры на правой границе
      T[N - 1] = Tr;

      // Определяем поле температуры
      for (int i = N - 2; i >= 0; --i)
      {
        T[i] = alfa[i] * T[i + 1] + beta[i];
      }
    }
  };

  std::vector<std::thread> threads;
  int chunk_size = (t_end) / num_threads; // Размер чанка для потоков

  for (int i = 0; i < num_threads; ++i)
  {
    int start = i * chunk_size + 1;
    int end = (i == num_threads - 1) ? t_end : start + chunk_size;
    threads.emplace_back(compute_temperature, start, end);
  }

  // Ожидаем завершения всех потоков
  for (auto &t : threads)
  {
    t.join();
  }
  // threads.clear(); // очищаем массив потоков

  // Запоминаем конец времени
  auto end = high_resolution_clock::now();
  // Вычисляем время выполнения
  auto duration = duration_cast<milliseconds>(end - start);
  cout << duration.count() << endl;

  // Вывод результата в файл
  ofstream f("res.txt");
  f << fixed << setprecision(4);
  f << "Толщина пластины L = " << L << endl;
  f << "Число узлов по координате N = " << N << endl;
  f << "Коэффициент теплопроводности материала пластины lamda = " << LAMBDA << endl;
  f << "Плотность материала пластины ro = " << RHO << endl;
  f << "Теплоемкость материала пластины c = " << C << endl;
  f << "Начальная температура T0 = " << T0 << endl;
  f << "Температура на границе x = 0, Tl = " << Tl << endl;
  f << "Температура на границе x = L, Tr = " << Tr << endl;
  f << "Результат получен с шагом по координате h = " << h << endl;
  f << "Результат получен с шагом по времени tau = " << tau << endl;
  f << "Температурное поле в момент времени t = " << t_end << endl;
  f << "Время выполнения: " << duration.count() << " миллисекунд." << endl;
  f.close();

  // Ввод температуры в отдельный файл
  ofstream g("tempr.txt");
  g << fixed << setprecision(3);
  g << "Length" << " " << setw(8) << setprecision(5) << "Temperature" << endl;
  for (int i = 0; i < N; ++i)
  {
    g << h * i << " " << setw(8) << setprecision(5) << T[i] << endl;
  }
  g.close();

  return 0;
}
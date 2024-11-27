#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <thread>
#include <mpi.h>

using namespace std;
using namespace std::chrono;

int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv); // Инициализация MPI

  int mpi_rank, mpi_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank); // Получение номера процесса
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size); // Получение числа процессов

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

  // Определяем поле температуры в начальный момент времени
  vector<double> T;

  // Инициализируем массив температуры
  if (mpi_rank == 0)
  {
    T.resize(N, T0);
  }
  // Рассылаем начальные данные всем процессам
  MPI_Bcast(T.data(), T.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);

  // Запоминаем начало времени
  auto start = high_resolution_clock::now();

  // Определяем расчетный шаг сетки по пространственной координате
  double h = L / (N - 1);
  // Определяем расчетный шаг сетки по времени
  double tau = t_end / N_T;

  double ai = LAMBDA / (h * h);
  double bi = 2.0 * LAMBDA / (h * h) + RHO * C / tau;
  double ci = LAMBDA / (h * h);

  // Определяем количество временных шагов для каждого процесса
  int steps_per_proc = N_T / mpi_size;
  int start_step = mpi_rank * steps_per_proc;
  int end_step = (mpi_rank == mpi_size - 1) ? N_T : start_step + steps_per_proc;

  vector<double> alfa, beta;
  alfa.resize(N);
  beta.resize(N);

  // Проводим интегрирование
  for (int step = start_step; step < end_step; ++step)
  {
    // Начальные прогоночные коэффициенты
    alfa[0] = 0.0;
    beta[0] = Tl;
    for (int i = 1; i < N - 1; ++i)
    {
      double fi = -RHO * C * T[i] / tau;
      alfa[i] = ai / (bi - ci * alfa[i - 1]);
      beta[i] = (ci * beta[i - 1] - fi) / (bi - ci * alfa[i - 1]);
    }

    T[N - 1] = Tr; // Температура на правой границе

    for (int i = N - 2; i >= 0; --i)
    {
      T[i] = alfa[i] * T[i + 1] + beta[i];
    }
  }

  // Собираем результаты на процессе 0
  MPI_Gather(
      T.data(), T.size(), MPI_DOUBLE,
      T.data(), T.size(), MPI_DOUBLE,
      0, MPI_COMM_WORLD);

  if (mpi_rank == 0)
  {
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
  }
  MPI_Finalize(); // Завершение работы MPI
  return 0;
}
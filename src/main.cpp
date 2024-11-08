#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

using namespace std;

int main()
{
  const int N = 100;
  const double LAMBDA = 46;
  const double RHO = 7800;
  const double C = 460;
  double t_end, L, T0, Tl, Tr;
  vector<double> T, alfa, beta;
  cout << "Введите окончание по времени, t_end: ";
  cin >> t_end;
  cout << "Введите толщину пластины, L: ";
  cin >> L;
  cout << "Введите начальную температуру, T0: ";
  cin >> T0;
  cout << "Введите температуру на границе х=0, Tl: ";
  cin >> Tl;
  cout << "Введите температуру на границе х=L, Tr: ";
  cin >> Tr;

  // Определяем расчетный шаг сетки по пространственной координате
  double h = L / (N - 1);
  // Определяем расчетный шаг сетки по времени
  double tau = t_end / 100.0;

  // Определяем поле температуры в начальный момент времени
  T.resize(N);
  for (int i = 0; i < N; ++i)
  {
    T[i] = T0;
  }

  // Проводим интегрирование
  double time = 0;
  while (time < t_end)
  {
    time += tau;

    // Начальные прогоночные коэффициенты
    alfa.resize(N);
    beta.resize(N);
    alfa[0] = 0.0;
    beta[0] = Tl;

    // Цикл для определения прогоночных коэффициентов
    for (int i = 1; i < N - 1; ++i)
    {
      double ai = LAMBDA / (h * h);
      double bi = 2.0 * LAMBDA / (h * h) + RHO * C / tau;
      double ci = LAMBDA / (h * h);
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
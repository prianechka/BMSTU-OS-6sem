#include <iostream>
#include <vector>
#include <thread>
#include <memory>
#include <ctime>
#include <algorithm>
#include <chrono>
#include <list>
#include <numeric>
#include <random>
#include <cmath>
#include <mutex>
#include <fstream>
#include <string>

void find_most_corr(std::vector<std::vector<double>> matrix, int n, int m)
{
    std::vector<double> mean(m, 0);
    for (int i = 0; i < m; i++)
    {
        double sum = 0;
        for (int j = 0; j < n; j++)
            sum += matrix[j][i];
        mean[i] = (sum / n);
    }
    int idx_1 = 0;
    int idx_2 = 0;
    double max_cor = 0; 
    for (int i = 0; i < m; i++) 
    {
        for (int j = i + 1; j < m; j++)
        {
            double var_x = 0;
            double var_y = 0;
            double cov = 0;
            for (int k = 0; k < n; k++)
            {
                double slag_1 = (matrix[k][i] - mean[i]);
                double slag_2 = (matrix[k][j] - mean[j]); 
                var_x += (slag_1 * slag_1);
                var_y += (slag_2 * slag_2);
                cov += (slag_1 * slag_2);
            }
            var_x /= n;
            var_y /= n;
            cov /= n;
            double cor = fabs(cov / sqrt(var_x * var_y));
            if (cor > max_cor)
            {
                idx_1 = i;
                idx_2 = j;
                max_cor = cor;
            }
        }
    }
    std::cout << "Результаты работы классического алгоритма: " << std::endl;
    std::cout << "Номер столбца 1 (начиная с 1): " << idx_1  + 1 << std::endl;
    std::cout << "Номер столбца 2 (начиная с 1): " << idx_2 + 1 << std::endl;
    std::cout << "Значение корреляции: " << max_cor << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
}

void scheme_one_for_thread(std::vector<std::vector<double>> matrix, std::vector<double> &mean, 
                           int start_index, int end_index, int n)
{
    for (int i = start_index; i < end_index; i++)
    {
        float sum = 0;
        for (int j = 0; j < n; j++)
            sum += matrix[j][i];
        mean[i] = (sum / n);
    }
}

void write_max(double local_max, double local_idx_1, double local_idx_2,
                volatile double &max_cor, volatile double &idx_1, volatile double &idx_2)
{
    static std::mutex my_mutex;
    my_mutex.lock();
    if (local_max > max_cor)
    {
        idx_1 = local_idx_1;
        idx_2 = local_idx_2;
        max_cor = local_max;
    }
    my_mutex.unlock();
}

void scheme_two_for_thread(std::vector<std::vector<double>> matrix, std::vector<double> mean, 
                           int start_index, int end_index, int n, int m, volatile double &max_cor, 
                           volatile double &idx_1, volatile double &idx_2)
{
    int local_idx_1 = 0;
    int local_idx_2 = 0;
    double local_max_cor = 0;
    for (int i = start_index; i < end_index; i++) 
    {
        for (int j = i + 1; j < m; j++)
        {
            double var_x = 0;
            double var_y = 0;
            double cov = 0;
            for (int k = 0; k < n; k++)
            {
                double slag_1 = (matrix[k][i] - mean[i]);
                double slag_2 = (matrix[k][j] - mean[j]); 
                var_x += (slag_1 * slag_1);
                var_y += (slag_2 * slag_2);
                cov += (slag_1 * slag_2);
            }
            var_x /= n;
            var_y /= n;
            cov /= n;
            double cor = fabs(cov / sqrt(var_x * var_y));
            if (cor > local_max_cor)
            {
                local_idx_1 = i;
                local_idx_2 = j;
                local_max_cor = cor;
            }
        }
    }
    write_max(local_max_cor, local_idx_1, local_idx_2, max_cor, idx_1, idx_2);
}

void find_most_corr_with_threads(std::vector<std::vector<double>> matrix, int n, int m, int num_threads)
{
    if (num_threads > m)
    {
        std::cout << "Max enough threads: " << m << std::endl;
        num_threads = m;
    }
    int koef = m / num_threads;
    int ost = m % num_threads;
    auto* threads = new std::thread[num_threads];
    std::vector<double> mean(m, 0);
    for (int i = 0; i < num_threads; i++)
    {
        int start_index = (i * koef);
        int end_index = ((i + 1) * koef);
        if (i == num_threads - 1)
            end_index += ost;
        threads[i] = std::thread(scheme_one_for_thread, matrix, std::ref(mean), start_index, end_index, n);
    }
    for (int i = 0; i < num_threads; i++)
    {
        threads[i].join();
    }
    auto* threads_1 = new std::thread[num_threads];
    volatile double max_cor = 0;
    volatile double idx_1 = 0;
    volatile double idx_2 = 0;
    
    for (int i = 0; i < num_threads; i++)
    {
        int start_index = i * koef;
        int end_index = (i + 1) * koef;
        if (i == num_threads - 1)
            end_index += ost;
        threads_1[i] = std::thread(scheme_two_for_thread, matrix, mean, start_index, end_index, n, m,
                                std::ref(max_cor), std::ref(idx_1), std::ref(idx_2));
    }
    for (int i = 0; i < num_threads; i++)
    {
        threads_1[i].join();
    }

    std::cout << "Результаты работы алгоритма с использованием потоков: " << std::endl;
    std::cout << "Номер столбца 1 (начиная с 1): " << idx_1  + 1 << std::endl;
    std::cout << "Номер столбца 2 (начиная с 1): " << idx_2 + 1 << std::endl;
    std::cout << "Значение корреляции: " << max_cor << std::endl;
}

void measure_time(std::vector<std::vector<double>> matrix, int row, int col, int n_threads)
{
    int N = 2;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; i++)
        find_most_corr(matrix, row, col);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    float time = duration.count();
    std::cout << "Время работы классического алгоритма: " << time / N << std::endl;


    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; i++)
        find_most_corr_with_threads(matrix, row, col, 4);
    end = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    time = duration.count();
    std::cout << "Время работы алгоритма с потоками: " << time / N << std::endl;
}

std::vector<std::vector<double>> read_matrix_from_file(size_t &row, size_t &col)
{
    std::string filename;
    std::cout << "Введите имя теста, которое нужно открыть (с расширением)" << std::endl;
    std::cin >> filename;
    std::ifstream in(filename);
    in >> row;
    in >> col;
    std::vector<std::vector<double>> matrix(row);
    for(size_t i = 0; i < row; ++i)
    {
        matrix[i].resize(col);
        for(size_t j = 0; j < col; ++j)
        {
            in >> matrix[i][j];
        }   
    }
    return matrix;
}

void print_matrix(std::vector<std::vector<double>> matrix, int row, int col)
{
    std::cout << "Исходная матрица: " << std::endl;
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
            std::cout << matrix[i][j] << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

int check_size(size_t row, size_t col)
{
    int result = 0;
    if ((row < 2) or (col < 2))
        result = 1;
    return result;
}

void measure_threads(int n_threads)
{
    std::vector<int> array {2, 5, 10, 25, 50, 75, 100, 200, 500, 1000};
    std::vector<int> repeat {1000, 1000, 1000, 100, 100, 100, 50, 20, 2, 2};
    for (int k = 0; k < 10; k++)
    {
        int row = array[k], col = array[k];
        int N = repeat[k];
        std::vector<std::vector<double>> matrix(row);
        for(size_t i = 0; i < row; ++i)
        {
            matrix[i].resize(col);
            for(size_t j = 0; j < col; ++j)
            {
                matrix[i][j] = rand() % 668;
            }   
        }
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < N; i++)
            find_most_corr_with_threads(matrix, row, col, 15);
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        auto time = duration.count();
        std::cout << time / N << std::endl;
    }
}

int main()
{
    size_t row, col;
    std::vector<std::vector<double>> matrix = read_matrix_from_file(row, col);
    int n_threads = 3;
    int check = check_size(row, col);
    if (check == 0)
    {
        print_matrix(matrix, row, col);
        find_most_corr(matrix, row, col);
        find_most_corr_with_threads(matrix, row, col, n_threads);
    }
    else
        std::cout << "Размеры не соответствуют ТЗ";
    int x;
    std::cin >> x;
    return 0;
}
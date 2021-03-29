#include <omp.h>
#include <cmath>
#include <cstdio>
#include <chrono>
using namespace std::chrono;

void qsort_serial(int *array, int start, int end) {
    if (end - start + 1 < 2) {
        return;
    }
    int first = start;
    int last = end;
    int middle = array[(start + end) / 2];
    do {
        while (array[first] < middle) first++;
        while (array[last] > middle) last--;
        if (first <= last) {
            int temp = array[first];
            array[first] = array[last];
            array[last] = temp;
            first++;
            last--;
        }
    } while (first < last);
    if (start < last) {
        qsort_serial(array, start, last);
    }
    if (first < end) {
        qsort_serial(array, first, end);
    }
}

void qsort_parallel(int *array, int start, int end, int threads = 8) {
    // printf_s("Thread %d: from %d to %d\n", omp_get_thread_num(), start, end);
    if (end - start + 1 < 2) {
        return;
    }
    int first = start;
    int last = end;
    int middle = array[(start + end) / 2];
    do {
        while (array[first] < middle) first++;
        while (array[last] > middle) last--;
        if (first <= last) {
            int temp = array[first];
            array[first] = array[last];
            array[last] = temp;
            first++;
            last--;
        }
    } while (first < last);
    if (threads > 1) {
#pragma omp parallel sections
        {
#pragma omp section
            if (start < last) {

                qsort_parallel(array, start, last, threads / 2);
            }
#pragma omp section
            if (first < end) {
                qsort_parallel(array, first, end, threads / 2);
            }
        }
    } else {
        if (start < last) {
            qsort_serial(array, start, last);
        }
        if (first < end) {
            qsort_serial(array, first, end);
        }
    }
}

int *array;
int size = 10000000;

void run_serial_sort() {
    int *array_copy = new int[size];
    for (int i = 0; i < size; i++) {
        array_copy[i] = array[i];
    }
    qsort_serial(array_copy, 0, size - 1);
    delete[] array_copy;;
}

void run_parallel_sort() {
    int *array_copy = new int[size];
    for (int i = 0; i < size; i++) {
        array_copy[i] = array[i];
    }
    qsort_parallel(array_copy, 0, size - 1, 16);
    delete[] array_copy;;
}

void benchmark(void func(), int warmups, int benchmarks) {
    printf("Warm up %d times:\n", warmups);
    for (int i = 0; i < warmups; i++) {
        auto start = high_resolution_clock::now();
        func();
        auto stop = high_resolution_clock::now();
        double ms = (double)(duration_cast<microseconds>(stop - start).count()) / 1000;
        printf("WarmUp %d: %f ms\n", i + 1, ms);
    }
    printf("\nBenchmark %d times:\n", benchmarks);
    double meanTime = 0;
    for (int i = 0; i < benchmarks; i++) {
        auto start = high_resolution_clock::now();
        func();
        auto stop = high_resolution_clock::now();
        double ms = (double)(duration_cast<microseconds>(stop - start).count()) / 1000;
        printf("Benchmark %d: %f ms\n", i + 1, ms);
        meanTime += ms;
    }
    printf("Mean time: %f\n", meanTime / benchmarks);
}

int main() {
    int sizes[7] = {1000, 10000, 100000, 1000000, 10000000, 20000000, 30000000};
    for (int i : sizes) {
        size = i;
        array = new int[size];
        for (int j = 0; j < size; j++) {
            array[j] = rand();
        }
        printf("\n%d\n", size);
        printf("Serial:\n");
        benchmark(run_serial_sort, 3, 5);
        printf("Parallel:\n");
        benchmark(run_parallel_sort, 3, 5);
    }
    return 0;
}
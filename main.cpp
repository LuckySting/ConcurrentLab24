#include <omp.h>
#include <cmath>
#include <cstdio>
#include <Windows.h>

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
    ULONGLONG dwStart;
    ULONGLONG dwStop;
    for (int i = 0; i < warmups; i++) {
        dwStart = GetTickCount64();
        func();
        dwStop = GetTickCount64();
        printf("WarmUp %d: %llu milliseconds\n", i + 1, dwStop - dwStart);
    }
    printf("\nBenchmark %d times:\n", benchmarks);
    ULONGLONG meanTime = 0;
    for (int i = 0; i < benchmarks; i++) {
        dwStart = GetTickCount64();
        func();
        dwStop = GetTickCount64();
        printf("Benchmark %d: %llu milliseconds\n", i + 1, dwStop - dwStart);
        meanTime += dwStop - dwStart;
    }
    printf("Mean time: %llu\n", meanTime / benchmarks);
}

int main() {
    array = new int[size];

    for (int i = 0; i < size; i++) {
        array[i] = rand();
    }
    benchmark(run_serial_sort, 5, 10);
    benchmark(run_parallel_sort, 5, 10);
    return 0;
}
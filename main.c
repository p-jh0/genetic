#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define NUM_POINTS 8
#define POP_SIZE 8
#define MAX_GEN 1000
#define CROSSOVER_RATIO 1.0
#define MUTATION_RATIO 0.01

// 점 구조체
typedef struct {
    double x, y;
} Point;

// 후보해 구조체
typedef struct {
    int path[NUM_POINTS];
    double distance;
} Candidate;

// 점 데이터
Point points[NUM_POINTS] = {
        {0, 3}, // A
        {7, 5}, // B
        {6, 0}, // C
        {4, 3}, // D
        {1, 0}, // E
        {5, 3}, // F
        {2, 2}, // G
        {4, 1}  // H
};

// 거리 계산 함수
double calculate_distance(Point a, Point b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

// 경로의 총 거리 계산
double calculate_total_distance(int path[]) {
    double total = 0.0;
    for (int i = 0; i < NUM_POINTS - 1; i++) {
        total += calculate_distance(points[path[i]], points[path[i + 1]]);
    }
    // 마지막 점에서 시작점으로 돌아감
    total += calculate_distance(points[path[NUM_POINTS - 1]], points[path[0]]);
    return total;
}

// 후보해 초기화
void initialize_population(Candidate population[]) {
    for (int i = 0; i < POP_SIZE; i++) {
        for (int j = 0; j < NUM_POINTS; j++) {
            population[i].path[j] = j;
        }
        // 경로 섞기 (A 제외)
        for (int j = 1; j < NUM_POINTS; j++) {
            int rand_idx = 1 + rand() % (NUM_POINTS - 1);
            int temp = population[i].path[j];
            population[i].path[j] = population[i].path[rand_idx];
            population[i].path[rand_idx] = temp;
        }
        population[i].distance = calculate_total_distance(population[i].path);
    }
}

// 사이클 교차 연산
void cycle_crossover(int parent1[], int parent2[], int child[]) {
    int visited[NUM_POINTS] = {0};
    int start = 1; // A 제외하고 시작
    int current = start;

    while (!visited[current]) {
        child[current] = parent1[current];
        visited[current] = 1;
        for (int i = 1; i < NUM_POINTS; i++) {
            if (parent2[i] == parent1[current]) {
                current = i;
                break;
            }
        }
    }
    for (int i = 1; i < NUM_POINTS; i++) {
        if (!visited[i]) {
            child[i] = parent2[i];
        }
    }
    child[0] = 0; // 시작점은 항상 A
}

// 돌연변이
void mutate(int path[]) {
    if ((double)rand() / RAND_MAX < MUTATION_RATIO) {
        int idx1 = 1 + rand() % (NUM_POINTS - 1);
        int idx2 = 1 + rand() % (NUM_POINTS - 1);
        int temp = path[idx1];
        path[idx1] = path[idx2];
        path[idx2] = temp;
    }
}

// 후보해 복사
void copy_candidate(Candidate *dest, Candidate *src) {
    for (int i = 0; i < NUM_POINTS; i++) {
        dest->path[i] = src->path[i];
    }
    dest->distance = src->distance;
}

// 메인 함수
int main() {
    srand(time(NULL));

    Candidate population[POP_SIZE];
    Candidate new_population[POP_SIZE];
    Candidate best_candidate;

    initialize_population(population);
    copy_candidate(&best_candidate, &population[0]);

    for (int gen = 0; gen < MAX_GEN; gen++) {
        for (int i = 0; i < POP_SIZE; i += 2) {
            int parent1_idx = rand() % POP_SIZE;
            int parent2_idx = rand() % POP_SIZE;

            cycle_crossover(population[parent1_idx].path, population[parent2_idx].path, new_population[i].path);
            cycle_crossover(population[parent2_idx].path, population[parent1_idx].path, new_population[i + 1].path);

            mutate(new_population[i].path);
            mutate(new_population[i + 1].path);

            new_population[i].distance = calculate_total_distance(new_population[i].path);
            new_population[i + 1].distance = calculate_total_distance(new_population[i + 1].path);
        }

        for (int i = 0; i < POP_SIZE; i++) {
            copy_candidate(&population[i], &new_population[i]);
            if (population[i].distance < best_candidate.distance) {
                copy_candidate(&best_candidate, &population[i]);
            }
        }
    }

    printf("최적 경로: ");
    for (int i = 0; i < NUM_POINTS; i++) {
        printf("%c ", 'A' + best_candidate.path[i]);
    }
    printf("A\n");
    printf("최소 거리: %.2f\n", best_candidate.distance);

    return 0;
}

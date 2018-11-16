//
//  HidatoGenerator.h
//

#ifndef HidatoGenerator_h
#define HidatoGenerator_h

#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <random>

class Random {
public:
    static void setRandomSeed() {
        gen.seed(std::time(0));
    }

    static int range(int min, int max) {
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(gen);
    }

    static double range(double min, double max) {
        std::uniform_real_distribution<double> distribution(min, max);
        return distribution(gen);
    }

private:
    static std::mt19937 gen;

};

std::mt19937 Random::gen;

class Hidato {
public:
    static Hidato *get() {
        return &instance;
    }

    // difficulty = 0 ~ 1 (easy ~ hard)
    int generate(std::vector<int> &map, int width, int height, float difficulty) {
        Random::setRandomSeed();

        solution = map;

        Generator gen(map, width, height);
        
        int c = 10;
        while (c--) {
            gen.randomize();
            if (gen.simulate(10, 0.1, 100000) == 0) break;
            gen.reset();
        }
        if (c <= 0) return -1;

        gen.putNumbers(solution);

        gen.punch(difficulty);
        map = gen.result();
        problem = map;
        
        return 1;
    }

    std::vector<int> getSolution() {
        return solution;
    }

    // 1 = 정답, -1 = 문제와 다름, -2 = 이어지지 않음, -3 = 기타
    int verify(std::vector<int> &map, int width, int height) {
        // difference check
        for (int i = 0; i < width * height; i++) {
            // 문제지에서 빈칸은 넘어가고, 문제지와 입력으로 받은 해결된 퍼즐 맵에서 다른 부분이 있으면 틀린 답으로 처리
            if (problem[i] != 0 && problem[i] != map[i]) {
                return -1;
            }
        }

        // find 1 index and last number(max number)
        int index = -1, checkNumber = 2, lastNumber = -1;
        for (int i = 0; i < width * height; i++) {
            if (map[i] == 1) index = i;
            if (map[i] > lastNumber) lastNumber = map[i];
        }

        while (checkNumber != lastNumber) {
            int count = 0;
            bool flag = true;
            // 현재 인덱스 기준 이웃 8칸 범위에서 다음 숫자를 찾음
            for (int i = -1; i <= 1 && flag; i++) {
                for (int j = -1; j <= 1 && flag; j++) {
                    if (i == 0 && j == 0) continue;

                    int x = index % width + j, y = index / width + i;
                    int di = y * width + x;

                    // 이웃 타일의 값이 다음 숫자와 일치하지 않거나 맵을 벗어났으면 그 다음 이웃 타일을 검사함
                    if (x > width - 1 || x < 0 || y > height - 1 || y < 0 || map[di] != checkNumber) {
                        count += 1;
                        continue;
                    }
                    checkNumber += 1;
                    index = di;
                    // 일치하면 반복문을 빠져나감
                    flag = false;
                }
            }
            // 이웃 타일 8개 전부 다 다르면 오류
            if (count >= 8) return -2;
        }

        return 1;
    }
    
private:
    Hidato() {}
    ~Hidato() {}

    static Hidato instance;

    std::vector<int> problem, solution;

private:
    /* ----- Generator class ----- */
    class Generator {
    public:
        Generator(const std::vector<int> &m, int width, int height) : width(width), height(height) {
            size = width * height;
            for (int i = 0; i < size; i++) {
                next.push_back(0);
                
                if (m[i] != -1) realSize += 1;
                map.push_back(m[i]);
            }
        }
        
        // next에 랜덤한 좌표값을 넣어주는 함수
        void randomize() {
            start = getRandomIndex();
            for (int i = 0; i < size; i++) {
                if (map[i] != -1) next[i] = getNeighbor(i);
            }
        }
        
        int getNeighbor(int index) {
            int x = index % width, y = index / width;
            do {
                // 랜덤한 8 방향중 하나를 선택함
                int dx = Random::range(-1, 1), dy = Random::range(-1, 1);
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx, ny = y + dy;
                if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
                if (map[ny * width + nx] == -1) continue;
                
                // 현재 위치에서 랜덤하게 골라진 이웃 타일 하나의 위치를 넣음
                return ny * width + nx;
            } while (true);
        }
        
        // 타일을 랜덤으로 깔았을 때 빈 공간을 최대한 줄이는 걸 목표로 하는 함수
        int simulate(double max, double min, int steps) {
            Generator best = Generator(*this);
            double factor = -log(max / min); // max=10, min=0.1, factor=-2
            int curCount = blankCount();
            int prevCount = curCount, bestCount = curCount;
            
            for (int step = 0; step < steps; step++) {
                // step동안 10에서 시작해서 1.35xxx로 수렴하는 값
                double temp = max * exp(factor * step / steps);
                std::pair<int, int> undoData = moveRandom();
                // 랜덤으로 타일을 깔고 끝났을 때 남은 빈 공간의 수를 저장함
                curCount = blankCount();
                double delta = curCount - prevCount;
                // 랜덤으로 타일을 깔고 남은 빈 공간이 이전 상태보다 많으면 되돌림
                // 뒤의 조건은 엄청 복잡해 보이지만 결과적으로 값의 차이가 적을 수록 되돌아갈 확률이 높아지게 하는 조건
                // 그런데 exp함수 때문인지 없는게 더 빠름
                if (delta > 0 && exp(-delta / temp) < Random::range(0.0, 1.0)) {
                    undo(undoData);
                } else {
                    prevCount = curCount;
                    if (curCount < bestCount) {
                        bestCount = curCount;
                        // 지금까지 빈 공간의 개수가 가장 적었던 방을 계속 기록함
                        best.copy(*this);
                        if (curCount <= 0) break;
                    }
                }
            }
            copy(best);
            
            return bestCount;
        }
        
        // 저장된 순서에 따라 입력으로 받은 배열에 빈 칸이 나오지 않을 때 까지 숫자를 순차적으로 넣음
        // 끝나면 마지막으로 넣은 숫자 - 1 값을 반환함(맵에 채워진 칸 수)
        int putNumbers(std::vector<int> &grid) {
            int index = start, num = 1;
            while (grid[index] == 0) {
                grid[index] = num++;
                index = next[index];
            }
            return num - 1;
        }
        
        // 맵에 빈 칸이 몇 개인지 체크하는 함수
        int blankCount() {
            std::vector<int> grid;
            for (int i = 0; i < size; i++) grid.push_back(0);
            int cnt = putNumbers(grid);
            return realSize - cnt;
        }
        
        // 랜덤한 위치의 타일에서 원래 갖고 있던 이웃 타일 위치를 다른 이웃 타일로 옮기는 함수
        std::pair<int, int> moveRandom() {
            // 랜덤한 위치를 뽑아서
            int index = getRandomIndex();
            // 그 위치의 이웃 타일 중 하나를 선택
            int before = next[index];
            int after;
            do {
                // 뽑은 랜덤한 위치의 주변 타일 중 하나를 고름
                after = getNeighbor(index);
                // 그 위치가 before과 겹치지 않을 때 까지 반복
            } while (after == before);
            next[index] = after;
            
            // 틀렸을 때 복구하기 위한
            return std::make_pair(index, before);
        }
        
        void undo(std::pair<int, int> data) {
            next[data.first] = data.second;
        }
        
        // 맵에서 막힌 공간(-1)이 아닌 위치의 랜덤한 위치를 찾아서 반환하는 함수
        int getRandomIndex() {
            int ret;
            do {
                ret = Random::range(0, size - 1);
                if (map[ret] != -1) break;
            } while (true);
            
            return ret;
        }

        int getLastNumberIndex() {
            int num = 1, ret = start;
            while (num++ < realSize) ret = next[ret];
            return ret;
        }
        
        void copy(const Generator &data) {
            width = data.width;
            height = data.height;
            size = data.size;
            start = data.start;
            next.clear();
            for (int i = 0; i < data.next.size(); i++) next.push_back(data.next[i]);
        }
        
        void reset() {
            start = getRandomIndex();
            next.clear();
            for (int i = 0; i < width * height; i++) next.push_back(0);
        }

        void punch(float emptyRatio) {
            visibleMap = map;

            int visibleCount = realSize * (1 - emptyRatio) - 2, end = getLastNumberIndex();
            // 시작 숫자와 끝 숫자는 무조건 보여야 함
            visibleMap[start] = 1;
            visibleMap[end] = 1;
            while (visibleCount) {
                int randomIndex = getRandomIndex();
                if (visibleMap[randomIndex] == 1) continue;
                visibleMap[randomIndex] = 1;
                visibleCount--;
            }
        }

        std::vector<int> result() {
            puzzle = map;
            putNumbers(puzzle);

            for (int i = 0; i < size; i++) {
                if (visibleMap[i] == 0) puzzle[i] = 0;
            }

            return puzzle;
        }
        
    public:
        int width, height, size;
        int realSize = 0; // x칸을 제외 한 실제 크기
        int start;
        std::vector<int> next, map;
        std::vector<int> visibleMap; // 히다토 퍼즐에서 보일 부분이 담긴 배열
        std::vector<int> puzzle; // 만들어진 히다토 퍼즐 결과물
        
    };
    
};

Hidato Hidato::instance = Hidato();

#endif /* HidatoGenerator_h */
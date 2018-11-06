//
//  HidatoGenerator.h
//
//

#ifndef HidatoGenerator_h
#define HidatoGenerator_h

#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

using namespace std;

class Hidato {
public:
    // difficulty = 0 ~ 1 (easy - hard)
    static int generate(vector<int> &map, int width, int height, float difficulty) {
        Generator gen(map, width, height);
        
        int c = 10;
        while (c--) {
            gen.randomize();
            if (gen.simulate(10, 0.1, 100000) == 0) {
                gen.extract(map);
                break;
            }
            gen.reset();
        }
        if (c <= 0) return -1;
        
        return 1;
    }
    
private:
    /* ----- Generator class ----- */
    class Generator {
    public:
        Generator(const vector<int> &m, int width, int height) : width(width), height(height) {
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
                int dx = (rand() % 3) - 1, dy = (rand() % 3) - 1;
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
                pair<int, int> undoData = moveRandom();
                // 랜덤으로 타일을 깔고 끝났을 때 남은 빈 공간의 수를 저장함
                curCount = blankCount();
                double delta = curCount - prevCount;
                // 랜덤으로 타일을 깔고 남은 빈 공간이 이전 상태보다 많으면 되돌림
                // 뒤의 조건은 엄청 복잡해 보이지만 결과적으로 값의 차이가 적을 수록 되돌아갈 확률이 높아지게 하는 조건
                // 그런데 exp함수 때문인지 없는게 더 빠름
                if (delta > 0 && exp(-delta / temp) < ((double)rand() / (double)RAND_MAX)) {
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
        
        int extract(vector<int> &grid) {
            int index = start, num = 1;
            while (grid[index] == 0) {
                grid[index] = num++;
                index = next[index];
            }
            return num - 1;
        }
        
        int blankCount() {
            vector<int> grid;
            for (int i = 0; i < size; i++) grid.push_back(0);
            int cnt = extract(grid);
            return realSize - cnt;
        }
        
        // 랜덤한 위치의 타일에서 원래 갖고 있던 이웃 타일 위치를 다른 이웃 타일로 옮기는 함수
        pair<int, int> moveRandom() {
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
            return make_pair(index, before);
        }
        
        void undo(pair<int, int> data) {
            next[data.first] = data.second;
        }
        
        // 맵에서 막힌 공간(-1)이 아닌 위치의 랜덤한 위치를 찾아서 반환하는 함수
        int getRandomIndex() {
            int ret;
            do {
                ret = rand() % size;
                if (map[ret] != -1) break;
            } while (true);
            
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
        
    public:
        int width, height, size;
        int realSize = 0; // x칸을 제외 한 실제 크기
        int start;
        vector<int> next, map;
        
    };
    
};

#endif /* HidatoGenerator_h */
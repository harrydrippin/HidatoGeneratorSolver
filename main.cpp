#include "hidato_generator.h"
#include "hidato_solver.h"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <chrono>

using namespace std;

void printPuzzle(const vector<int> map, int width, int height) {
    for (int i = 0; i < height; i++) {
        cout << "\t";
        for (int j = 0; j < width; j++) {
            if (map[i * width + j] == -1) printf("%3s", "");
            else if (map[i * width + j] == 0) printf("xx ");
            else printf("%02d ", map[i * width + j]);
        }
        cout << "\n";
    }
}

int main() {    
    /// generate 
    
    ifstream input("input.txt");

    int t;
    input >> t;

    while (t--) {
        int width, height;
        input >> width >> height;

        vector<int> puzzle(width * height);
        for (int i = 0; i < width * height; i++) input >> puzzle[i];

        /// ----------

        chrono::system_clock::time_point startTimePoint = chrono::system_clock::now();
        cout << "generate start" << "\n";
        Hidato::get()->generate(puzzle, width, height, 0.5f);

        chrono::duration<float> elapsed = chrono::system_clock::now() - startTimePoint;
        printPuzzle(puzzle, width, height);
        cout << "generate end\nelapsed time: " << elapsed.count() << " sec" << "\n";
        cout << "\n";

        /// ----------

        Solver solver;

        startTimePoint = chrono::system_clock::now();
        cout << "solve start" << "\n";

        solver.solve(puzzle, height, width);
        elapsed = chrono::system_clock::now() - startTimePoint;

        printPuzzle(puzzle, width, height);

        cout << "solve end\nelapsed time: " << elapsed.count() << " sec" << "\n";
        cout << "\n";

        /// ----------

        switch(Hidato::get()->verify(puzzle, width, height)) {
        case 1:     cout << "correct" << endl;          break;
        case -1:    cout << "different" << endl;        break;
        case -2:    cout << "not connected" << endl;    break;
        default:    cout << "error" << endl;            break;
        }

        cout << endl;

        /// ----------
    }
    input.close();

    return 0;
}
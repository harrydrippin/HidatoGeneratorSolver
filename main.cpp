#include "hidato_generator.h"

#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;

int main() {    
    /// generate 
    
    ifstream input("input.txt");

    int width, height;
    input >> width >> height;

    vector<int> map(width * height);
    for (int i = 0; i < width * height; i++) input >> map[i];

    Hidato::get()->generate(map, width, height, 0.5f);

    /// ----------

    /// save

    fstream output("output.txt", ios::out);
    output << width << " " << height << "\n";
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            output << map[i * width + j] << " ";

            if (map[i * width + j] == -1) printf("%3s", "");
            else if (map[i * width + j] == 0) printf("xx ");
            else printf("%02d ", map[i * width + j]);
        }
        output << "\n";
        cout << "\n";
    }
    output << "\n";
    output.close();

    /// ----------

    /// verify

    // ifstream inputPuzzle("output.txt");

    // inputPuzzle >> width >> height;

    // vector<int> puzzle(width * height);
    // for (int i = 0; i < width * height; i++) inputPuzzle >> puzzle[i];

    // for (int i = 0; i < height; i++) {
    //     for (int j = 0; j < width; j++) {
    //         if (puzzle[i * width + j] == -1) printf("%3s", "");
    //         else if (puzzle[i * width + j] == 0) printf("xx ");
    //         else printf("%02d ", puzzle[i * width + j]);
    //     }
    //     cout << "\n";
    // }

    // switch(Hidato::get()->verify(puzzle, width, height)) {
    // case 1:     cout << "correct" << endl;          break;
    // case -1:    cout << "different" << endl;        break;
    // case -2:    cout << "not connected" << endl;    break;
    // default:    cout << "error" << endl;            break;
    // }

    /// ----------

    return 0;
}
#include "hidato_generator.h"

#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;

int main() {    
    ifstream input("input.txt");

    int width, height;
    input >> width >> height;

    vector<int> map(width * height);
    for (int i = 0; i < width * height; i++) input >> map[i];

    Hidato::generate(map, width, height, 0.75f);

    fstream output("output.txt", ios::app);
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

    return 0;
}
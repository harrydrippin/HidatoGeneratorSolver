/*
Kookmin University Software
알고리즘 프로젝트
Hidato Puzzle
Solver Programming
*/

#include <iostream>
#include <vector>
#include <iterator>

using namespace std;

class Solver {
	// 하나의 칸이 갈 수 있는 8가지 경로(dx, dy)
	const int dx[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
	const int dy[8] = { -1,  0,  1,-1, 1, 1, 0,-1 };
	int row, col;
	int max;
	int* arr;
	bool* isCheckArr;

public:
	void solve(vector<int>& puzz, int max_row, int max_col) {
		// if(puzz.size() < 1) return;
		row = max_row;
		col = max_col;

		arr = new int[row*col];
		isCheckArr = new bool[row*col];

		for (int i = 0; i <= row * col; i++) { // isCheckArr 초기화 
			isCheckArr[i] = 0;
		}

		max = 0;
		int count = 0;
		int x, y;

		// arr 배열 초기화(puzz에 있는 값들로) 및 최대값 구하기
        // isCheckArr배열은 arr배열에 값이 존재한다면 그 값을 인덱스로 하여 그 부분을 1로 채운다.
		for (vector<int>::iterator i = puzz.begin(); i != puzz.end(); i++) {
			arr[count] = *i;
			if (max < arr[count])
				max = arr[count];
			if (arr[count] > 0)
				isCheckArr[arr[count]] = 1;
			count++;
		}

		findStartPoint(x, y); // 1을 찾아주고 
		bool solveCheck = search(x, y, 2); // 값이 1인 인덱스에서부터 탐색 시작 

		if(solveCheck) {
			// 해결된 답 puzz 벡터에 넣어주기
			count = 0;
			for (vector<int>::iterator i = puzz.begin(); i != puzz.end(); i++) {
				if ((*i) == 0)
					(*i) = arr[count];
				count++;
			}
		} else {
			cout << "퍼즐 해결 불가" << endl;
		}

		delete[] arr;
		delete[] isCheckArr;
	}

	// 시작점을 찾는 함수. 시작점은 1인 곳에서 시작하므로 배열에서 1이 존재하는 인덱스를 찾는다.
	void findStartPoint(int& x, int& y) {
		for (int i = 0; i<col; i++) {
			for (int j = 0; j<row; j++) {
				if (arr[j + row * i] == 1) {
					x = j;
					y = i;
					return;
				}
			}
		}
	}

	// DFS 알고리즘 - 다음 경로를 탐색하는 함수 
	bool search(int x, int y, int nextPoint) {
		if (nextPoint == max) return true;

        // 현재 인덱스 주변 8개의 인덱스 칸에 nextpoint, 즉 다음 값이 존재한다면
		if (isCheckArr[nextPoint]) {
			// 하나의 칸이 갈 수 있는 8가지 경로
			for (int i = 0; i < 8; i++) {
				int row_tmp = x + dx[i];
				int col_tmp = y + dy[i];
                // 인덱스가 -1이거나 행,열 사이즈가 초과되면 continue
				if ((row_tmp < 0) || (col_tmp < 0) || (row_tmp >= row) || (col_tmp >= col))
					continue;
				if (arr[row_tmp + col_tmp * row] == nextPoint) {
					if (search(row_tmp, col_tmp, nextPoint + 1))
						return true;
				}
			}
			return false;
		}

		for (int i = 0; i < 8; i++) {
			// 하나의 칸이 갈 수 있는 8가지 경로 
			int row_tmp = x + dx[i];
			int col_tmp = y + dy[i];
            // 인덱스가 -1이거나 행,열 사이즈가 초과되면 continue
			if ((row_tmp < 0) || (col_tmp < 0) || (row_tmp >= row) || (col_tmp >= col))
				continue;
			if (arr[row_tmp + col_tmp * row] == 0) {
				arr[row_tmp + col_tmp * row] = nextPoint;
				if (search(row_tmp, col_tmp, nextPoint + 1))
					return true;
				arr[row_tmp + col_tmp * row] = 0;
			}
		}
		return false;
	}
};

int main() {
	ios::sync_with_stdio(false);
	cin.tie(NULL);
	cout.tie(NULL);

	int testcase;
	cin >> testcase;
	while (testcase--) {

		int input_row, input_col;
		cin >> input_row >> input_col;

		Solver s;

		vector<int> puzz;

		int puzz_size = input_row * input_col;
		while (puzz_size--) {
			int add_num;
			cin >> add_num;
			puzz.push_back(add_num);
		}

		s.solve(puzz, input_row, input_col);

		int c = 0;
		for (vector<int>::iterator i = puzz.begin(); i != puzz.end(); i++) {
			c++;
			if ((*i) != -1 && (*i) != 0)
				cout << (*i) << " ";
			else
				cout << -1 << " ";

			if (c == input_row) {
				cout << endl;
				c = 0;
			}
		}
		cout << endl;
	}
	return 0;
}

/* 테스트 케이스 실행
./a.out

3
8 8
0 33 35 0 0 -1 -1 -1
0 0 24 22 0 -1 -1 -1
0 0 0 21 0 0 -1 -1
0 26 0 13 40 11 -1 -1
27 0 0 0 9 0 1 -1
-1 -1 0 0 18 0 0 -1
-1 -1 -1 -1 0 7 0 0
-1 -1 -1 -1 -1 -1 5 0

10 10
-1 -1 -1 -1 0 53 -1 -1 -1 -1
-1 -1 -1 -1 0 0 -1 -1 -1 -1
-1 -1 56 0 0 0 30 0 -1 -1
-1 -1 0 0 0 0 0 0 -1 -1
-1 0 0 20 22 0 0 0 0 -1
-1 13 0 23 47 0 41 0 34 -1
0 0 11 18 0 0 0 42 35 37
0 0 0 0 5 3 1 0 0 0
-1 -1 -1 -1 0 0 -1 -1 -1 -1
-1 -1 -1 -1 7 0 -1 -1 -1 -1

4 6
-1 1 -1 -1
14 0 3 0
0 -1 0 6
0 -1 5 0
0 0 0 -1
10 -1 -1 -1

*/

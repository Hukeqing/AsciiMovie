//
// Created by HKQ on 5/1/2020.
//

#define SHOW_ON_WINDOWS

#include <bits/stdc++.h>
#include <unistd.h>

#ifdef SHOW_ON_WINDOWS

#include <windows.h>

#endif

using namespace std;

int get_num(const string &s) {
    char *ptr;
    return strtol(s.data(), &ptr, 10);
}

string in_file;
int per;

void get_args(int argc, char *argv[]) {
    int ch;
    while ((ch = getopt(argc, argv, "i:p:")) != -1) {
        switch (ch) {
            case 'i':
                in_file = string(optarg);
                break;
            case 'p':
                per = get_num(optarg);
                break;
            case '?':
                cout << "Unknown option: " << optarg << endl;
                exit(-1);
            default:
                break;
        }
    }
}

vector<string> movie;

void input() {
    ifstream file;
    int w, h;
    file.open(in_file, ios::in | ios::binary);
    if (file) {
        file >> w >> h;
        char a, b;
        unsigned char cnt;
        int curW = 0, curH = 0;
        movie.emplace_back();
        file.get(a);
        while (a != '\n')
            file.get(a);
        while (file.get(a)) {
            file.get(b);
            cnt = b;
            while (cnt) {
                if (cnt >= w - curW) {
                    movie.back() += string(w - curW, a) + '\n';
                    // NOLINTNEXTLINE
                    cnt -= static_cast<char>(w - curW);
                    curW = 0;
                    curH++;
                    if (curH == h) {
                        movie.emplace_back();
                        curH = 0;
                    }
                } else {
                    curW += cnt;
                    movie.back() += string(cnt, a);
                    cnt = 0;
                }
            }
        }
    }
}

int main(int argc, char** argv) {
#ifdef DEBUG
    in_file = "mov.asv";
    per = 30;
    freopen("out2.txt", "w", stdout);
#else
    get_args(argc, argv);
#endif
    input();
    movie.pop_back();
    int cnt = 0;
    system("cls");
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    auto start = clock();
    auto last = 0;
    while (cnt < movie.size()) {
        last = clock();
        if (last - start >= CLOCKS_PER_SEC / per) {
            SetConsoleCursorPosition(hOut, {0, 0});
            cout << movie[cnt] << endl;
            cout.flush();
            cnt++;
            start = last;
        }
    }
}

//
// Created by HKQ on 4/30/2020.
//

// #define NOT_STD_CPP

#include "image.h"
#include <pthread.h>
#include <unistd.h>

using namespace std;

int get_num(const string &s) {
    char *ptr;
    return strtol(s.data(), &ptr, 10);
}

string in_file, out_file, temp_file;
int scale = 30;
string c = "#@&$=+*-:.";
// NOLINTNEXTLINE
ofstream am_file;

void get_args(int argc, char *argv[]) {
    int ch;
    while ((ch = getopt(argc, argv, "i:o:t:s:c:")) != -1) {
        switch (ch) {
        case 'i':
            in_file = string(optarg);
            break;
        case 'o':
            out_file = string(optarg);
            break;
        case 't':
            temp_file = string(optarg);
            break;
        case 's':
            scale = get_num(optarg);
            break;
        case 'c':
            c = string(optarg);
            break;
        case '?':
            cout << "Unknown option: " << optarg << endl;
            exit(-1);
        default:
            break;
        }
    }
    if (in_file.empty()) {
        cout << "no input files" << endl;
        exit(-1);
    }
}

void out_size(int w, int h) {
    static bool flag = false;
    if (!flag)
        am_file << w << " " << h << endl;
    flag = true;
}

void out(char c) {
    static char last = -1;
    static unsigned char cnt = 0;
    if (c == -1) {
        if (cnt != 0) {
            am_file << last << cnt;
            return;
        }
    }
    if (c == last || last == -1) {
        last = c;
        if (cnt == UCHAR_MAX) {
            am_file << c << cnt;
            cnt = 0;
        }
        cnt++;
    } else {
        am_file << last << cnt;
        cnt = 1;
        last = c;
    }
}

void solve(Image &img) {
    out_size(img.width / scale, img.height / scale / 2);
    for (int j = 0; j < img.height / scale / 2; ++j) {
        for (int i = 0; i < img.width / scale; ++i) {
            int r = 0, g = 0, b = 0;
            for (int ii = 0; ii < scale; ++ii) {
                for (int jj = 0; jj < scale * 2; ++jj) {
                    auto tmp = img.get(i * scale + ii, j * scale * 2 + jj);
                    r += tmp.r;
                    g += tmp.g;
                    b += tmp.b;
                }
            }
            auto total = Color(r / scale / scale / 2, g / scale / scale / 2,
                               b / scale / scale / 2);
            auto res = total.getGreyValue();
            out(c[int(res * 1.0 / 255 * c.size())]);
        }
    }
}

void *python(void *argv) {
    system((R"(.\cv.exe )" + in_file + " " + temp_file).data());
    return nullptr;
}

int main(int argc, char *argv[]) {
#ifdef DEBUG
    in_file = R"(D:\Code\python\Image\source.mp4)";
    out_file = "mov.asv";
    temp_file = R"(D:\Code\python\Image\img)";
#else
    get_args(argc, argv);
#endif
    remove((temp_file + "\\cnt.txt").data());
    pthread_t pthread;
    pthread_create(&pthread, nullptr, python, nullptr);
    string basic = in_file;
    am_file.open(out_file, ios::binary | ios::out);
    int cnt = 1;
    ifstream cnt_file;
    cnt_file.open(temp_file + "\\cnt.txt");
    while (!cnt_file) {
        cnt_file.close();
        ifstream image(temp_file + "\\" + to_string(cnt) + ".jpg");
        while (image) {
            image.close();
            auto img =
                Image((temp_file + "\\" + to_string(cnt) + ".jpg").data());
            if (!img.error) {
                solve(img);
                remove((temp_file + "\\" + to_string(cnt) + ".jpg").data());
                cnt++;
            }
            image.open(temp_file + "\\" + to_string(cnt) + ".jpg");
        }
        image.close();
        cnt_file.open(temp_file + "\\cnt.txt");
    }
    int total;
    cnt_file >> total;
    cnt_file.close();
    while (cnt < total) {
        auto img = Image((temp_file + "\\" + to_string(cnt) + ".jpg").data());
        solve(img);
        remove((temp_file + "\\" + to_string(cnt) + ".jpg").data());
        cnt++;
    }
    out(-1);
    pthread_exit(nullptr);
    remove((temp_file + "\\cnt.txt").data());
}

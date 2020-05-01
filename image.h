//
// Created by HKQ on 3/9/2020.
//

#ifndef IMAGE_IMAGE_H
#define IMAGE_IMAGE_H

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STBI_ASSERT(x)

#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_image_resize.h"

#ifndef NOT_STD_CPP

#include <bits/stdc++.h>

#else

#include <map>
#include <iostream>
#include <iomanip>

#endif

#define PI (-acos(-1))

using namespace std;

struct Color {
    unsigned char r, g, b, a;

    explicit Color(unsigned char rr = 255, unsigned char gg = 255, unsigned char bb = 255, unsigned char aa = 255) : r(
            rr), g(gg), b(bb), a(aa) {}

    unsigned char getGreyValue() {
        return static_cast<unsigned char>(r * 0.299 + g * 0.587 + b * 0.114);
    }
};

struct HSVColor {
    double h, s, v;

    HSVColor() : h(0), s(100), v(100) {}

    explicit HSVColor(const Color &color) {
        unsigned char max_c = max(max(color.r, color.g), color.b);
        unsigned char min_c = min(min(color.r, color.g), color.b);
        v = max_c / 255.0;
        s = max_c == 0 ? 0 : (max_c - min_c) * 1.0 / max_c;
        h = 0;
        if (color.r == max_c) h = (color.g - color.b) * 60.0 / (max_c - min_c);
        else if (color.g == max_c) h = (color.b - color.r) * 60.0 / (max_c - min_c) + 120;
        else h = (color.r - color.g) * 60.0 / (max_c - min_c) + 240;
        if (h < 0) h += 360;
    }

    void toColor(Color &color) {
        if (abs(s) <= 1e-6) {
            color.r = color.g = color.b = (unsigned char) (v * 255);
        } else {
            double H = h / 60;
            int i = (int) H;
            double f = H - i;
            auto A = (unsigned char) (v * (1 - s) * 255);
            auto B = (unsigned char) (v * (1 - s * f) * 255);
            auto C = (unsigned char) (v * (1 - s * (1 - f)) * 255);
            auto V = (unsigned char) (v * 255);
            switch (i) {
                case 0:
                    color.r = V;
                    color.g = C;
                    color.b = A;
                    break;
                case 1:
                    color.r = B;
                    color.g = V;
                    color.b = A;
                    break;
                case 2:
                    color.r = A;
                    color.g = V;
                    color.b = C;
                    break;
                case 3:
                    color.r = A;
                    color.g = B;
                    color.b = V;
                    break;
                case 4:
                    color.r = C;
                    color.g = A;
                    color.b = V;
                    break;
                case 5:
                    color.r = V;
                    color.g = A;
                    color.b = B;
                    break;
                default:
                    break;
            }
        }
    }
};

pair<char, char> char2ff(unsigned char c) {
    return {(c >> 4u) >= 10 ? (c >> 4u) - 10 + 'A' : (c >> 4u) + '0',
            (c & 0xfu) >= 10 ? (c & 0xfu) - 10 + 'A' : (c & 0xfu) + '0'};
}

struct Image {
    unsigned char *data{};
    int width{}, height{}, nrChannels{};
    bool error;

    Image() : error(true) {}

    explicit Image(const char *file) {
        data = stbi_load(file, &width, &height, &nrChannels, 0);
        error = data == nullptr;
    }

    Image(int w, int h, int channel) : width(w), height(h), nrChannels(channel), error(false),
                                       data(new unsigned char[w * h * channel]) {}

    ~Image() {
        if (!error) delete[] data;
    }

    void open(const char *file) {
        if (!error) delete[] data;
        data = stbi_load(file, &width, &height, &nrChannels, 0);
        error = data == nullptr;
    }

    Color get(int w, int h) {
        assert(!error);
        assert(w < width && h < height);
        Color res;
        switch (nrChannels) {
            case 2:
                res.a = data[(h * width + w) * nrChannels + 1];
            case 1:
                res.r = data[(h * width + w) * nrChannels];
                res.g = data[(h * width + w) * nrChannels];
                res.b = data[(h * width + w) * nrChannels];
                break;
            case 4:
                res.a = data[(h * width + w) * nrChannels + 3];
            case 3:
                res.r = data[(h * width + w) * nrChannels + 0];
                res.g = data[(h * width + w) * nrChannels + 1];
                res.b = data[(h * width + w) * nrChannels + 2];
                break;
        }
        return res;
    }

    void change(int w, int h, const Color &c) {
        assert(!error);
        assert(w < width && h < height);
        switch (nrChannels) {
            case 2:
                data[(h * width + w) * nrChannels + 1] = c.a;
            case 1:
                data[(h * width + w) * nrChannels] = c.r;
                break;
            case 4:
                data[(h * width + w) * nrChannels + 3] = c.a;
            case 3:
                data[(h * width + w) * nrChannels + 0] = c.r;
                data[(h * width + w) * nrChannels + 1] = c.g;
                data[(h * width + w) * nrChannels + 2] = c.b;
                break;
        }
    }

    void resize(int w, int h) {
        auto *new_data = new unsigned char[w * h * nrChannels];
        int res = stbir_resize_uint8(data, width, height, 0, new_data, w, h, 0, nrChannels);
        if (res == 0) {
            error = true;
            return;
        }
        delete[] data;
        data = new_data;
        width = w;
        height = h;
    }

    void save_bmp(const char *filename) {
        assert(!error);
        stbi_write_bmp(filename, width, height, nrChannels, data);
    }

    void save_jpg(const char *filename, int quality) {
        assert(!error);
        stbi_write_jpg(filename, width, height, nrChannels, data, quality);
    }

    void save_png(const char *filename) {
        assert(!error);
        stbi_write_png(filename, width, height, nrChannels, data, nrChannels * width);
    }

    void to_grey() {
        assert(nrChannels >= 3);
        unsigned char *cur = data, *dist = data;
        int cnt = 0;
        nrChannels -= 2;
        while (cnt < width * height) {
            *cur = static_cast<unsigned char>((*dist) * 0.299 + (*(dist + 1)) * 0.587 + (*(dist + 2)) * 0.114);
            cur++;
            dist += 3;
            if (nrChannels == 2) {
                *cur = *dist;
                cur++;
                dist++;
            }
            cnt++;
        }
    }

    void histogramEqualization() {
        if (nrChannels >= 3) to_grey();
        map<unsigned char, int> tmp;
        for (int i = 0; i < width * height; ++i)
            tmp[data[i * nrChannels]]++;
        double step = width * height * 1.0 / 255;
        int cnt = 0;
        auto iter = tmp.begin();
        while (iter != tmp.end()) {
            cnt += iter->second;
            iter->second = cnt / step;
            iter++;
        }
        for (int i = 0; i < width * height; ++i)
            data[i * nrChannels] = (unsigned char) (tmp[data[i * nrChannels]]);
    }

    void threshold(unsigned char flag, unsigned char overflow = 255) {
        if (nrChannels >= 3) to_grey();
        for (int i = 0; i < width * height; ++i)
            data[i * nrChannels] = data[i * nrChannels] > flag ? overflow : 0;
    }

    void dft2() {
        assert(!error);
        if (nrChannels >= 3)
            to_grey();
        const double fixed_factor_for_axisX = (-2 * PI) / width;
        const double fixed_factor_for_axisY = (-2 * PI) / height;
        auto *tmp = new double[height * width];
        double max_double = 0;
        for (int u = 0; u < width; ++u) {
            for (int v = 0; v < height; ++v) {
                cout << u << " " << v << endl;
                complex<double> tmp_complex;
                for (int x = 0; x < width; ++x) {
                    for (int y = 0; y < height; ++y) {
                        double powerX = u * x * fixed_factor_for_axisX;
                        double powerY = v * y * fixed_factor_for_axisY;
                        tmp_complex += complex<double>(data[(y * width + x) * nrChannels + 0] * cos(powerX + powerY),
                                                       data[(y * width + x) * nrChannels + 0] * sin(powerX + powerY));
                    }
                }
                tmp[v * width + u] = abs(tmp_complex);
                max_double = max(max_double, tmp[v * width + u]);
            }
        }
        if (nrChannels == 2) {
            delete[] data;
            data = new unsigned char[height * width];
            nrChannels = 1;
        }
        for (int i = 0; i < width * height; ++i)
            data[i] = static_cast<unsigned char>(tmp[i] * 255 / max_double);
    }
};

#endif //IMAGE_IMAGE_H

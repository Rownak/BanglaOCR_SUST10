




#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <math.h>

using namespace std;
using namespace cv;
const string inpFile = "5.bmp";
const string outFile = "5thresh.bmp";

int otsu(const Mat &src) {
    auto *histogram = (int *)calloc(256, sizeof(int));
    const auto total = src.rows * src.cols;

    const auto *data = src.ptr<uchar>();
    for (auto i = 0; i < total; i++)
        histogram[*data++]++;

    auto sum = 0.f;
    for (auto i = 1; i < 256; i++)
        sum += histogram[i] * i;

    auto sumB = 0.f, wB = 0.f, wF = 0.f, max = 0.f;
    auto threshold1 = 0.f, threshold2 = 0.f, between = 0.f;
    for (auto i = 0; i < 256; i++) {
        auto hist_data = histogram[i];
        wB += hist_data;
        if (not wB) continue;
        wF = total - wB;
        if (not wF) break;

        sumB += hist_data * i;
        auto mB = sumB / wB;
        auto mF = (sum - sumB) / wF;
        between = wB * wF * pow(mB - mF, 2.f);
        if (between >= max) {
            threshold1 = i;
            if (between > max) threshold2 = i;
            max = between;
        }
    }

    free(histogram);
    return (threshold1 + threshold2) / 2.f;
}

int main() {

    auto sourceImg = imread(inpFile, 0);
    Mat thresedImg;
    int thresh;
    if(sourceImg.empty()) cout << "Image not found";

    else {
             thresh = otsu(sourceImg);
            cout << "otsu threshold value: " << thresh;
    }
    threshold(sourceImg, thresedImg, thresh, 255, THRESH_BINARY);
    imwrite(outFile,thresedImg);
    return 0;
}

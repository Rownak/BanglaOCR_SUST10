#include<stdio.h>
#include<iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
int c = 1;
const char* files[] = {"1.bmp"};
const char* outputFileName = "1_rotated.bmp";
void deskew(const char* filename, double angle)
{
  cv::Mat img = cv::imread(filename, 0);

  cv::bitwise_not(img, img);

  std::vector<cv::Point> points;
  cv::Mat_<uchar>::iterator it = img.begin<uchar>();
  cv::Mat_<uchar>::iterator end = img.end<uchar>();
  for (; it != end; ++it)
    if (*it)
      points.push_back(it.pos());

  cv::RotatedRect box = cv::minAreaRect(cv::Mat(points));
   cv::Mat rot_mat = cv::getRotationMatrix2D(box.center, angle, 1);
   cv::Mat rotated;
  cv::warpAffine(img, rotated, rot_mat, img.size(), cv::INTER_CUBIC);
  cv::Size box_size = box.size;
  if (box.angle < -45.)
    std::swap(box_size.width, box_size.height);
  cv::Mat cropped;
  cv::getRectSubPix(rotated, box_size, box.center, cropped);
    //cv::imshow("Original", img);
    //cv::imwrite("inverted2.bmp",img);
  //cv::imshow("Rotated", rotated);
    //cv::imwrite("Rotated2.bmp",rotated);
  //cv::imshow("Cropped", cropped);
  //cv::imwrite("Cropped2.bmp",cropped);
  threshold(rotated, rotated, 160, 255, THRESH_BINARY_INV);
  //cv::bitwise_not(cropped, cropped);
  cv::imwrite(outputFileName,rotated);
  //cv::waitKey(0);
}
void compute_skew(const char* filename)
{
   // Load in grayscale.
   cv::Mat src = cv::imread(filename, 0);
   cv::Size size = src.size();

    cv::bitwise_not(src, src);
    //compute long line of white pixels in the image
     std::vector<cv::Vec4i> lines;

//     find lines in the image . We use a powerful mathematical tool called the Hough transform.
// the main idea of the Hough transform is to use a 2D
//accumulator in order to count how many times a given line has been found in the image,
//the whole image is scanned and by a voting system the "best" lines are identified.
//We used a more efficient variant of the Standard Hough Transform (SHT) called the Probabilistic Hough Transform (PHT).
//In OpenCV the PHT is implemented under the name HoughLinesP.
//
//In addition to the standard parameters of the Hough Transform, we have two additional parameters:
//
//minLineLength = The minimum line length. Line segments shorter than that will be rejected.
//This is a great tool in order to prune out small residual lines.
//maxLineGap – The maximum allowed gap between points on the same line to link them.
//This could be interesting for a multi-columns text,
//for example we could choose to not link lines from different text columns.


    cv::HoughLinesP(src, lines, 1, CV_PI/180, 100, size.width / 2.f, 30);

//    We use a step size of 1 for ρ and π/180 for θ, the threshold (the minimum number of votes) is 100.
//minLineLength is width/2, this is not an unreasonable assumption if the text is well isolated.
//maxLineGap is 20, it seems a sound value for a gap.


// we simply calculate the angle between each line and the horizontal line using the atan2 math function and we compute the mean angle of all the lines.
//For debugging purposes we also draw all the lines in a new image called disp_lines and we display this image in a new window.

    cv::Mat disp_lines(size, CV_8UC1, cv::Scalar(0, 0, 0));
    double angle = 0.;
    unsigned nb_lines = lines.size();
    for (unsigned i = 0; i < nb_lines; ++i)
    {
        cv::line(disp_lines, cv::Point(lines[i][0], lines[i][1]),
                 cv::Point(lines[i][2], lines[i][3]), cv::Scalar(255, 0 ,0));
        angle += atan2((double)lines[i][3] - lines[i][1],
                       (double)lines[i][2] - lines[i][0]);
    }
    angle /= nb_lines; // mean angle, in radians.
    deskew(filename,angle * 180 / CV_PI);

    std::cout << "Image of 1 page of a story book scanned in 300 dpi  " << "\nthe skew angle : " << angle * 180 / CV_PI << std::endl;

    //cv::imshow(filename, disp_lines);
    //cv::imwrite("disp_lines2.bmp",disp_lines);
    //cv::waitKey(0);
    cv::destroyWindow(filename);
}




int main()
{
    double exec_time = (double)getTickCount();
	unsigned nb_files = sizeof(files) / sizeof(const char*);
	for (unsigned i = 0; i < nb_files; ++i)
		compute_skew(files[i]);
    exec_time = ((double)getTickCount() - exec_time) * 1000. / getTickFrequency();
    cout << "execution time: " << exec_time<< " milisec\n\n" ;
}

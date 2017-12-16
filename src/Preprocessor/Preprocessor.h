//
// Created by ASamir on 12/16/17.
//

#ifndef PRE_PROCESSING_PREPROCESSOR_H
#define PRE_PROCESSING_PREPROCESSOR_H

#include "../Pre_Processing.h"

#define OUTPUT_PATH "../output/"
#define IMAGE_GREY "grey.jpg"
#define IMAGE_PAPER_ONLY "paper.jpg"
#define IMAGE_WIENER "wiener.ppm"
#define IMAGE_SAUVOLA "sauvola.jpg"
#define IMAGE_INTERPOLATED "interpolated.jpg"

using namespace std;
using namespace cv;

class Preprocessor {
public:

    ///
    /// \param in
    /// \param out
    Preprocessor(string in) : image_path(in) {}

    ///
    /// \return
    void pre_process() {
        // Read image
        read_image();

        // Apply Canny edge and transformations to get the paper out of the surrounding background
        get_paper_text();

        // Apply Wiener filter
        apply_wiener_filter();

        // Apply Sauvola binarization
        apply_sauvola_binarization();

        // Apply background surface estimation
//        background_surface_estimation();

        // Calculate average distance sigma
//        calculate_average_distance();
    }

private:
    string image_path;
    ///<
    string output_path;
    ///<
    Mat image_original;
    ///<
    Mat image_original_transformed;
    ///<
    Mat1b image_grey; // Is(x, y)
    ///<
    Mat1b image_wiener; // I(x, y)
    ///<
    Mat1b image_sauvola; // S(x, y)
    ///<
    Mat1b image_interpolated; // B(x, y)
    ///<
    Mat1b image_final_threshold; // T(x`,y`)
    ///<
    float average_distance;

    void read_image() {
        image_original = imread(image_path);
        cvtColor(image_original, image_grey, COLOR_RGB2GRAY);
    }

    void get_paper_text() {
        Mat image_smoothed, image_blurred, image_edges, image_paper_text;

        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        vector<Point> original_corners, transformed_corners;

        double epsilon;

        // First step
        // Apply median blur
        medianBlur(image_grey, image_blurred, 15);

        // Canny detection
        Canny(image_blurred, image_edges, 50, 80);

        // Apply dilation
        dilate(image_edges, image_edges, Mat(), Point(-1, -1));
        dilate(image_edges, image_edges, Mat(), Point(-1, -1));

        // Second step
        // Get contours (connected components in the image)
        findContours(image_edges.clone(), contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE, Point(0, 0));

        // Sort the contours
        sort(contours.begin(), contours.end(), compareContourAreas);

        // Save contours as poly-polygons
        vector<Rect> boundRect(contours.size());
        vector<vector<Point> > contours_poly(contours.size());
        for (int i = 0; i < contours.size(); i++) {
            approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
            boundRect[i] = boundingRect(Mat(contours_poly[i]));
        }

        int max_index = int(contours.size()) - 1;

        // Get the original 4 corners
        epsilon = 0.1 * arcLength(contours[max_index], true);
        approxPolyDP(contours[max_index], original_corners, epsilon, true);
        cout << original_corners.size();

        // Return if the number of corners is not 4
        if (original_corners.size() != 4) {
            perror("The number of corners is not 4!\n");
            return;
        }

        // Draw the green rectangle around the original corners and save that corner
        vector<vector<Point> > t = vector<vector<Point> >(1, original_corners);
        drawContours(image_original, t, -1, Scalar(255, 255, 255), 2, 8);

        // Third step
        double width_a = sqrt((pow(abs(original_corners[3].x - original_corners[2].x), 2))
                              + (pow(abs(original_corners[3].y - original_corners[2].y), 2)));
        double width_b = sqrt((pow(abs(original_corners[1].x - original_corners[0].x), 2))
                              + (pow(abs(original_corners[1].y - original_corners[0].y), 2)));
        int max_width = max(int(width_a), int(width_b));

        double height_a = sqrt((pow(abs(original_corners[0].x - original_corners[3].x), 2))
                               + (pow(abs(original_corners[0].y - original_corners[3].y), 2)));
        double height_b = sqrt((pow(abs(original_corners[1].x - original_corners[2].x), 2))
                               + (pow(abs(original_corners[1].y - original_corners[2].y), 2)));
        int max_height = max(int(height_a), int(height_b));


        Mat transformed; // Anti-clock wise, Add the corners
        transformed_corners.push_back(Point(max_width - 1, 0));
        transformed_corners.push_back(Point(0, 0));
        transformed_corners.push_back(Point(0, max_height - 1));
        transformed_corners.push_back(Point(max_width - 1, max_height - 1));


        Point2f original_corners_arr[4];
        copy(original_corners.begin(), original_corners.end(), original_corners_arr);
        Point2f transformed_corners_arr[4];
        copy(transformed_corners.begin(), transformed_corners.end(), transformed_corners_arr);

        // Apply transformation to get the text paper
        transformed = getPerspectiveTransform(original_corners_arr, transformed_corners_arr);
        warpPerspective(image_original, image_paper_text, transformed, Point(max_width, max_height));
        image_original_transformed = image_paper_text.clone();
        cvtColor(image_original_transformed, image_grey, cv::COLOR_RGB2GRAY);

        // Sometimes the image is found in wrong orientation
        if (image_grey.cols > image_grey.rows) {
            cv::rotate(image_grey, image_grey, ROTATE_90_COUNTERCLOCKWISE);
        }

    }

    void apply_wiener_filter() {
        // Call to WienerFilter
        WienerFilter(image_grey, image_wiener, Size(5, 5));
        // Save as PPM to be used by Doxa library that doesn't support OpenCV
        imwrite(string(OUTPUT_PATH) + IMAGE_WIENER, image_wiener);
    }

    void apply_sauvola_binarization() {

        // Apply sauvola binarization.
        system("../src/imgtxtenh/build/imgtxtenh -k 0.2 -p ../output/wiener.ppm ../output/sauvola.jpg");

        image_sauvola = imread(string(OUTPUT_PATH) + IMAGE_SAUVOLA, CV_LOAD_IMAGE_GRAYSCALE);
    }

    void background_surface_estimation(int dx = 15, int dy = 15) { // The window size is used from another paper [17].
//        image_interpolated = image_grey.clone();
        int rows = image_sauvola.rows, cols = image_sauvola.cols;

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (image_sauvola.at<uchar>(i, j) != 0) {

                    float n = 0, d = 0;
                    for (int a = max(0, i - dx); a < (i + dx); ++a) {
                        for (int b = max(0, j - dy); b < (j + dy); ++b) {
//                            n += (image_grey.at<uchar>(a, b) * (1 - image_sauvola.at<uchar>(a, b)));
                            d += (1 - image_sauvola.at<uchar>(a, b));
                        }
                    }

                    image_interpolated.at<uchar>(i, j) = n / d;
                }
            }
        }

        // Save the interpolated image.
        imwrite(string(OUTPUT_PATH) + IMAGE_INTERPOLATED, image_interpolated);
    }

    void calculate_average_distance() {
        average_distance = 0;
        int rows = image_interpolated.rows, cols = image_interpolated.cols;
//        float n = 0, d = 1;
//        for (int i = 0; i < rows; i++) {
//            for (int j = 0; j < cols; j++) {
//                n  += (image_interpolated.at<uchar>(i, j) - image_wiener.at<uchar>(i, j));
//                d += image_sauvola.at<uchar>(i, j);
//            }
//        }
//        average_distance = n / d;
    }

    // Helper functions
    // comparison function object
    static bool compareContourAreas(vector<Point> contour1, vector<Point> contour2) {
        double i = fabs(contourArea(Mat(contour1)));
        double j = fabs(contourArea(Mat(contour2)));
        return (i < j);
    }

};


#endif //PRE_PROCESSING_PREPROCESSOR_H

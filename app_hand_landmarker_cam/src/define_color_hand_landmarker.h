/***********************************************************************************************************************
* Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : define_color_hand_landmarker.h
* Version      : 2.8.0
* Description  : RZ/V2H DRP-AI Sample Application for mediapipe HandLandmarker with MIPI/USB Camera
***********************************************************************************************************************/
#ifndef DEFINE_COLOR_H
#define DEFINE_COLOR_H
#include <opencv2/opencv.hpp>

/*****************************************
* color
******************************************/
/* face connection list */
const static std::vector<std::pair<uint32_t, uint32_t>> hand_connection = 
{ 
    /* Thumb */
    {0, 1}, {1, 2}, {2, 3}, {3, 4},
    /* Index finger */
    {0, 5}, {5, 6}, {6, 7}, {7, 8},
    /* Middle finger */
    {5, 9}, {9, 10}, {10, 11}, {11, 12},
    /* Ring finger */
    {9, 13}, {13, 14}, {14, 15}, {15, 16},
    /* Pinky */
    {13, 17}, {17, 18}, {18, 19}, {19, 20},
    /* Heel of the hand*/
    {0, 17},
};

const static cv::Scalar rect_color = cv::Scalar(0, 0, 255, 255);
const static cv::Scalar points_color = cv::Scalar(0, 255, 0, 255);
const static cv::Scalar links_color = cv::Scalar(0, 0, 0, 255);

#endif


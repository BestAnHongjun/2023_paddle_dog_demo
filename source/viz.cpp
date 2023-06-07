#include "viz.h"


const char* class_names[] = {
    "call", "like", "stop", "no_gesture"
};

const float color_list[4][3] =
{
    {0.000, 0.447, 0.741},
    {0.850, 0.325, 0.098},
    {0.929, 0.694, 0.125},
    {0.494, 0.184, 0.556}
};

void viz(cv::Mat &img, int cls, float score, float x, float y, float w, float h)
{
    cv::Scalar color = cv::Scalar(color_list[cls][0], color_list[cls][1], color_list[cls][2]);
    float c_mean = cv::mean(color)[0];
    cv::Scalar txt_color;
    if (c_mean > 0.5){
        txt_color = cv::Scalar(0, 0, 0);
    }else{
        txt_color = cv::Scalar(255, 255, 255);
    }

    cv::rectangle(img, cv::Rect(cv::Point(x, y), cv::Size(w, h)), color * 255, 2);

    char text[256];
    sprintf(text, "%s %.1f%%", class_names[cls], score * 100);

    int baseLine = 0;
    cv::Size label_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.4, 1, &baseLine);

    cv::Scalar txt_bk_color = color * 0.7 * 255;

    y = y + 1;
    if (y > img.rows)
        y = img.rows;

    cv::rectangle(img, cv::Rect(cv::Point(x, y), cv::Size(label_size.width, label_size.height + baseLine)),
                    txt_bk_color, -1);

    cv::putText(img, text, cv::Point(x, y + label_size.height),
                cv::FONT_HERSHEY_SIMPLEX, 0.4, txt_color, 1);

    return;
}
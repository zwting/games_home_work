#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

std::vector<cv::Point2f> control_points;

const int WIDTH = 700;
const int HEIGHT = 700;
const int POINT_RADIUS = 3;
int sel_idx = -1;

int try_select_point(int x, int y)
{
    int radius = POINT_RADIUS * 2;
    for(int i = 0;i < control_points.size(); ++i)
    {
        auto vec = control_points[i] - cv::Point2f(x, y);
        std::cout<<vec.dot(vec)<<std::endl;
        if (vec.x * vec.x + vec.y * vec.y <= POINT_RADIUS * POINT_RADIUS)
        {
            return i;
        }
    }
    return -1;
}

bool try_add_point(int x, int y)
{
    if (control_points.size() >= 4)
        return false;

    int dis = POINT_RADIUS * 2;
    for(int i = 0;i < control_points.size(); ++i)
    {
        auto vec = control_points[i] - cv::Point2f(x, y);
        if (vec.x * vec.x + vec.y * vec.y <= dis * dis)
        {
            return false;
        }
    }
    control_points.push_back(cv::Point2f(x, y));
    return true;
}

void mouse_handler(int event, int x, int y, int flags, void *userdata) 
{
    static bool is_mouse_down = false;
    switch (event)
    {
    case cv::EVENT_LBUTTONDOWN:
        is_mouse_down = true;
        sel_idx = try_select_point(x, y);
        break;
    case cv::EVENT_MOUSEMOVE:
        if (is_mouse_down)
        {
            if ( sel_idx >= 0)
            {
                control_points[sel_idx].x = x;
                control_points[sel_idx].y = y;
            }
        }
        break;
    case cv::EVENT_LBUTTONUP:
        is_mouse_down = false;
        if(sel_idx == -1 && try_add_point(x, y))
        {
            sel_idx = control_points.size() - 1;
        }
        break;
    case cv::EVENT_RBUTTONUP:
        sel_idx = -1;
        break;
    }
    // if (event == cv::EVENT_LBUTTONDOWN)
    // {
    //     if(control_points.size() < 4) 
    //     {
    //         std::cout << "Left button of the mouse is clicked - position (" << x << ", "
    //         << y << ")" << '\n';
    //         control_points.emplace_back(x, y);
    //     }
    // }     
}

void naive_bezier(const std::vector<cv::Point2f> &points, cv::Mat &window) 
{
    auto &p_0 = points[0];
    auto &p_1 = points[1];
    auto &p_2 = points[2];
    auto &p_3 = points[3];

    for (double t = 0.0; t <= 1.0; t += 0.001) 
    {
        auto point = std::pow(1 - t, 3) * p_0 + 3 * t * std::pow(1 - t, 2) * p_1 +
                 3 * std::pow(t, 2) * (1 - t) * p_2 + std::pow(t, 3) * p_3;

        window.at<cv::Vec3b>(point.y, point.x)[2] = 255;
    }
}

cv::Point2f recursive_bezier(const std::vector<cv::Point2f> &control_points, float t) 
{
    // TODO: Implement de Casteljau's algorithm
    std::vector<cv::Point2f> points = control_points;
    while(points.size() > 1)
    {
        std::vector<cv::Point2f> tmp_list;
        for(int i = 1; i < points.size(); ++i)
        {
            cv::Point2f vec = points[i] - points[i - 1];
            tmp_list.push_back(vec * t);
        }
        points = tmp_list;
    }
    return points[0];
}

void bezier(const std::vector<cv::Point2f> &control_points, cv::Mat &window) 
{
    // TODO: Iterate through all t = 0 to t = 1 with small steps, and call de Casteljau's 
    // recursive Bezier algorithm.
    float dt = 0.01;
    float t = 0;
    while (t < 1)
    {
        auto pos = recursive_bezier(control_points, t);
        int idx = static_cast<int>(pos.x) + static_cast<int>(pos.y) * WIDTH;
        // window[idx] = 
        t += dt;
    }

}

int main() 
{
    cv::Mat window = cv::Mat(WIDTH, HEIGHT, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(window, window, cv::COLOR_BGR2RGB);
    cv::namedWindow("Bezier Curve", cv::WINDOW_AUTOSIZE);

    cv::setMouseCallback("Bezier Curve", mouse_handler, nullptr);

    int key = -1;
    while (key != 27) 
    {
        memset(window.data, 0, WIDTH * HEIGHT * 3);
        int i = 0;
        for (auto &point : control_points) 
        {
            cv::Scalar color = (i == sel_idx)? cv::Scalar(0, 255, 0): cv::Scalar(255, 255, 255);
            cv::circle(window, point, POINT_RADIUS, color, POINT_RADIUS);
            i++;
        }

        if (control_points.size() == 4) 
        {
            naive_bezier(control_points, window);
            //   bezier(control_points, window);

            cv::imshow("Bezier Curve", window);
            cv::imwrite("my_bezier_curve.png", window);
            key = cv::waitKey(0);

            return 0;
        }

        cv::imshow("Bezier Curve", window);
        key = cv::waitKey(20);
    }

return 0;
}

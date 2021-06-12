//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include "global.hpp"
#include <eigen3/Eigen/Eigen>
#include <opencv2/opencv.hpp>
class Texture{
private:
    cv::Mat image_data;

public:
    Texture(const std::string& name)
    {
        image_data = cv::imread(name);
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
        width = image_data.cols;
        height = image_data.rows;
    }

    int width, height;

    Eigen::Vector3f getColor(float u, float v)
    {
        auto u_img = u * width;
        auto v_img = (1 - v) * height;
        auto color = image_data.at<cv::Vec3b>(v_img, u_img);
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }

    Eigen::Vector3f getColorBilinear(float u, float v)
    {
        u = std::min(std::max(0.0f, u), 1.0f);
        v = std::min(std::max(0.0f, v), 1.0f);

        auto u_img = u * width;
        auto v_img = (1 - v) * height;
        float u_min = std::floor(u_img);
        float u_max = std::min(std::ceil(u_img), static_cast<float>(width));
        float v_min = std::floor(v_img);
        float v_max = std::min(std::ceil(v_img), static_cast<float>(height));

        cv::Vec3b lt = image_data.at<cv::Vec3b>(v_max, u_min);
        cv::Vec3b rt = image_data.at<cv::Vec3b>(v_max, u_max);
        cv::Vec3b lb = image_data.at<cv::Vec3b>(v_min, u_min);
        cv::Vec3b rb = image_data.at<cv::Vec3b>(v_min, u_max);

        float factor_lr = (u_img - u_min) / (u_max - u_min);
        float factor_tb = (v_img - v_max) / (v_min - v_max);
        cv::Vec3b lt_rt_blend = lt * (1.0 - factor_lr) + rt * factor_lr;
        cv::Vec3b lb_rb_blend = lb * (1.0 - factor_lr) + rb * factor_lr;

        cv::Vec3b final_c = lt_rt_blend * (1.0 - factor_tb) + lb_rb_blend * factor_tb;
        return Eigen::Vector3f(final_c[0], final_c[1], final_c[2]);
    }

};
#endif //RASTERIZER_TEXTURE_H

#include <iostream>
#include <opencv2/opencv.hpp>

/*
void forEachAccess(cv::Mat &src)
{
    src.forEach<cv::Vec3b>([](float &v, const int * position) -> void {
          v=v>65500?0:v;
    });
}
*/

void imwrite_inv_depth(std::string png, const cv::Mat& src){
    /*
    NORM_MINMAX
    https://www.codetd.com/ja/article/7031167
    */
    CV_Assert(src.type() == CV_32FC1);

    const float far = 14., near = 4.;// 4.0m - 14.0m, for fountain-p11 dataset
    const float minv = 1./far, maxv = 1./near;// minv = 1./14., maxv = 1./4., for inv depth map;

    cv::Mat dst;
    /* imwrite inverse depth map, 8bit
    //// [far, near](=[1./14., 1./4.]) --> [0, 255](unsigned char 8bit)
    dst.create(src.rows, src.cols, CV_8UC1);     
    for(int v=0; v < src.rows; v++) {
        const float* src_ptr = src.ptr<float>(v); 
        std::uint8_t* dst_ptr = dst.ptr<std::uint8_t>(v);
        for(int u=0; u< src.cols; u++) {
            float inv_depth_val = src_ptr[u];
            if(inv_depth_val < minv) inv_depth_val = minv;// clip [far, near]
            else if(inv_depth_val > maxv) inv_depth_val = maxv; 
            dst_ptr[u] = static_cast<std::uint8_t>((inv_depth_val - minv) * 255. /(maxv - minv));// max=255, min=0
        }
    }
    */
    //// [far, near](=[1./14., 1./4.]) --> [0, 65535](unsigned short int 16bit)
    dst.create(src.rows, src.cols, CV_16UC1);     
    for(int v=0; v < src.rows; v++) {
        const float* src_ptr = src.ptr<float>(v); 
        std::uint16_t* dst_ptr = dst.ptr<std::uint16_t>(v);
        for(int u=0; u< src.cols; u++) {
            float inv_depth_val = src_ptr[u];
            if(inv_depth_val < minv) inv_depth_val = minv;// clip [far, near]
            else if(inv_depth_val > maxv) inv_depth_val = maxv; 
            dst_ptr[u] = static_cast<std::uint16_t>((inv_depth_val - minv) * 65535. /(maxv - minv));// max=65535, min=0
        }
    }
    const int width = 768, height = 512;
    cv::resize(dst, dst, cv::Size(width, height));            
    cv::imwrite(png, dst);

    // // // imwrite inv depth map as color map
    // cv::Mat color_mat;
    // cv::normalize(dst,color_mat,0.,255.,cv::NORM_MINMAX,CV_8U);// u16bit[0, 65535] -> u8bit[0,255]
    // cv::applyColorMap(color_mat, color_mat, cv::COLORMAP_JET);
    // cv::imwrite(png, color_mat);
}


int main (int argc, char* argv[])
{
    if (argc < 2){
        std::cout << "./build/main 0005/Depth0001.exr" << std::endl;        
        std::cerr << "argc: " << argc << "should be 2";
        return 1;
    }
    try
    {
        cv::Mat img_depth=cv::imread(argv[1], cv::IMREAD_ANYCOLOR|cv::IMREAD_ANYDEPTH);
        // cv::Mat img_depth=cv::imread(argv[1], cv::IMREAD_ANYDEPTH);
        if (img_depth.empty()) {
            std::cout << "failed to load image." << std::endl;
            return 1;
        }
        std::cout << "loaded image size: " << img_depth.size() << std::endl;
        //Rチャンネルだけ抽出．
        std::vector<cv::Mat> img_depth_split;
        cv::split(img_depth,img_depth_split);
        cv::Mat img_depth_disp_m = img_depth_split[0].clone();


        //やたら大きい値はデータが無い点．
        //今回は表示用途に0にしてしまう．
        // img_depth_disp_m.forEach([](float &v, const int *p){
        //     v=v>65500?0:v;
        // });
        // forEachAccess(img_depth_disp_m);
        for(int v=0; v<img_depth_disp_m.rows; v++) {
            for(int u=0; u<img_depth_disp_m.cols; u++) {
                const float val = img_depth_disp_m.at<float>(v,u);
                img_depth_disp_m.at<float>(v,u) = val>65500?0:val;
                img_depth_disp_m.at<float>(v,u) = 1./val;
            }
        }

        // std::cout << "img_depth_disp_m: " << img_depth_disp_m << std::endl;

        // cv::Mat img_depth_disp = cv::Mat(img_depth_disp_m.rows, img_depth_disp_m.cols, CV_8UC1);

        // //表示用に正規化
        // cv::convertScale(img_depth_disp_m, img_depth_disp, 255.0/img_depth_disp_m.max());

        // cv::imshow("img_depth",img_depth_disp);
        // cv::waitKey();


        // cv::normalize(img_depth_disp_m, img_depth_disp, 0, 255, cv::NORM_MINMAX, CV_8U);
        // cv::imwrite("inv_depth.png",img_depth_disp);//"Result.png", dst);// only blue color
        const std::string dst_name = "gt16bit.png";
        imwrite_inv_depth(dst_name, img_depth_disp_m);
    }
    catch(const cv::Exception& ex)
    {
        std::cout << "Error: " << ex.what() << std::endl;
    }
    return 0;
}

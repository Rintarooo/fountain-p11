

#include <iostream>
#include <opencv2/opencv.hpp>

// void forEachAccess(cv::Mat &src)
// {
//     src.forEach<cv::Vec3b>([](float &v, const int * position) -> void {
//           v=v>65500?0:v;
//     });
// }


int main (int argc, char* argv[])
{
    if (argc < 2){
        std::cerr << "argc: " << argc << "should be 2";
        return 1;
    }
    try
    {
        //読み出し．オプションをつけること．
        //cv::imread("0006_Normal.exr", 
        // cv::Mat img_nor=cv::imread(argv[1], cv::IMREAD_ANYCOLOR);
        // cv::Mat img_nor=cv::imread(argv[1], cv::IMREAD_COLOR);
        cv::Mat img_nor=cv::imread(argv[1], cv::IMREAD_UNCHANGED);
        if (img_nor.empty()) {
            std::cout << "failed to load image." << std::endl;
            return 1;
        }
        std::cout << "loaded image size: " << img_nor.size() << std::endl;
        //Rチrn ャンネルだけ抽出．
        // std::vector<cv::Mat> img_nor_split;
        // cv::split(img_nor,img_nor_split);
        std::cout << "orig img_nor_y: " << img_nor.at<cv::Vec3f>(1900, 500) << std::endl; 
        // std::cout << "normal image: " << img_nor << std::endl;
        cv::Mat img_nor_disp_m = img_nor.clone();//img_nor_split[0].clone();


        //やたら大きい値はデータが無い点．
        //今回は表示用途に0にしてしまう．
        // img_nor_disp_m.forEach([](float &v, const int *p){
        //     v=v>65500?0:v;
        // });
        // forEachAccess(img_nor_disp_m);
        std::cout << "type: " << img_nor_disp_m.type() << std::endl; 
        CV_Assert(img_nor_disp_m.type() == CV_32FC3);
        // CV_Assert(img_nor_disp_m.type() == CV_8UC3);

        // for(int v=0; v<img_nor_disp_m.rows; v++) {
        //     for(int u=0; u<img_nor_disp_m.cols; u++) {
        //         const float v0 = img_nor_disp_m.at<cv::Vec3f>(v, u)[0];
        //         img_nor_disp_m.at<cv::Vec3f>(v, u)[0] = v0>65500?0:v0;
        //          src.at<cv::Vec3f>(v, u)[0] = x;
        //          src.at<cv::Vec3f>(v, u)[1] = y;
        //         src.at<cv::Vec3f>(v, u)[2] = z;
        //     }
        // }


        cv::Mat img_nor_disp = cv::Mat(img_nor_disp_m.rows, img_nor_disp_m.cols, CV_8UC3);
        // [-1,1] --> [-0.5,0.5] --> [0,1]
        img_nor_disp = (0.5 * img_nor_disp_m) + cv::Scalar(cv::Vec3f(0.5, 0.5, 0.5));
        for(int v=0; v<img_nor_disp.rows; v++) {
            for(int u=0; u<img_nor_disp.cols; u++) {
                float nx = img_nor_disp.at<cv::Vec3f>(v, u)[0];
                float ny = img_nor_disp.at<cv::Vec3f>(v, u)[1];
                float nz = img_nor_disp.at<cv::Vec3f>(v, u)[2];
                if(nx < 0 || ny < 0 || nz < 0 || nx > 1 || ny > 1 || nz > 1) std::cerr << "range should be [0,1]" << std::endl;
                float length = nx * nx + ny * ny + nz * nz;
                // if(length > 1.2 || length < 0.8) std::cerr << "length: " << length << std::endl;
                nx = nx / length;
                ny = ny / length;
                nz = nz / length;
                img_nor_disp.at<cv::Vec3f>(v, u)[0] = nx;
                img_nor_disp.at<cv::Vec3f>(v, u)[1] = ny;
                img_nor_disp.at<cv::Vec3f>(v, u)[2] = nz;
            }
        }
        std::cout << "img_nor_y: " << img_nor_disp.at<cv::Vec3f>(1900, 500) << std::endl; 
        
        // //表示用に正規化
        // cv::convertScale(img_nor_disp_m, img_nor_disp, 255.0/img_nor_disp_m.max());

        // cv::imshow("img_nor",img_nor_disp);
        // cv::waitKey();
        cv::cvtColor(img_nor_disp, img_nor_disp, cv::COLOR_BGR2RGB);
        cv::normalize(img_nor_disp, img_nor_disp, 0, 255, cv::NORM_MINMAX, CV_8U);
        // cvtColor(img_nor_disp, img_nor_disp, CV_RGB); 
    

        cv::imwrite("img_normal.png",img_nor_disp);//"Result.png", dst);// only blue color
        
    }
    catch(const cv::Exception& ex)
    {
        std::cout << "Error: " << ex.what() << std::endl;
    }
    return 0;
}

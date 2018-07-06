#include <vector>
#include <ctime>
#include <time.h>
#include "opencv2/opencv.hpp"
#include "segmentimage.h"

using namespace std;
using namespace cv;
int main()
{
	Mat img = imread("/home/lk/project/segement-4-20/6.jpg");
	resize(img,img,Size(1920,1080),0,0,INTER_LINEAR);

	double time0=static_cast<double>(getTickCount());
	clock_t tt = clock();
	if (img.empty())
	{
		fprintf(stderr, "Can not load image %s\n");
		return -1;
	}
	picture_pro(img,3);
	imshow("image", img);
	Mat gray;
	cvtColor(img, gray, CV_BGR2GRAY);
	img.convertTo(img,CV_32FC3);

	float sigma = 0.5;
	float k = 500;
	int min_size = 100;
	int num_ccs;
	std::vector<Point2f> vp2f; //定义返回矩形框位置的向量
	Mat result = segment_image(img, sigma, k, min_size, &num_ccs, vp2f);
	for(int i=0; i<vp2f.size(); i++)   //打印矩形框角点
		std::cout<<"save point:"<< vp2f[i]<< std::endl;

	clock_t tt0 = clock() - tt;
	float process_time = (float)tt0 / CLOCKS_PER_SEC;
	cout << "get " << num_ccs << " components" << endl;

	cout << "process time: " << process_time<<endl;
	imshow("process result", result);

	cvtColor(gray, gray, CV_GRAY2BGR);
	imshow("overlay result", gray*0.25 + result*0.75);
	double time1=(static_cast<double>(getTickCount())-time0)/getTickFrequency();
	cout<<"RUN_time is :"<<time1<<endl;
	tt = clock()- tt;
	float process_time_t = (float)tt / CLOCKS_PER_SEC;
	cout<<"process_time_t is :"<<process_time_t<<endl;

	waitKey();

	return 0;

}

#include <cstdlib>

//#include "filter.h"
#include "segmentgraph.h"
#include "segmentimage.h"
#include "time.h"
#include<vector>
using namespace std;

// random color
rgb random_rgb(){
  rgb c;
  double r;
  srand(unsigned(time(NULL)));
  c.r = rand()%256;
  c.g = rand()%256;
  c.b = rand()%256;

  return c;
}

//// dissimilarity measure between pixels
//static inline float diff(image<float> *r, image<float> *g, image<float> *b,
//           int x1, int y1, int x2, int y2)
//{
//  return sqrt(square(imRef(r, x1, y1)-imRef(r, x2, y2)) +
//        square(imRef(g, x1, y1)-imRef(g, x2, y2)) +
//        square(imRef(b, x1, y1)-imRef(b, x2, y2)));
//}

/*
 * Segment an image
 *
 * Returns a color image representing the segmentation.
 *
 * im: image to segment.
 * sigma: to smooth the image.
 * c: constant for treshold function.
 * min_size: minimum component size (enforced by post-processing stage).
 * num_ccs: number of connected components in the segmentation.
 */


//, vector<Point2f> &vp2f

void picture_pro(Mat &mat,int k)//改变图片尺寸
{
    //imshow("original picture",mat);
    resize(mat,mat,Size(mat.cols/k,mat.rows/k),0,0,INTER_LINEAR);
    //imshow("the processed picture",mat);
    //cvWaitKey(0);
}



Mat segment_image(Mat &img, float sigma, float c, int min_size,
              int *num_ccs, vector<Point2f> &vp2f)
{
  int width = img.size().width;
  int height = img.size().height;
  //Mat img_(height, width, CV_8UC3); //using u8 will produce too many component
  Mat img_(height, width, CV_32FC3);

  // gaussian blur
  int kSize = cvCeil(4*sigma)+1;
  GaussianBlur(img, img_, Size(kSize, kSize), sigma, sigma);

  // build graph
  edge *edges = new edge[width*height*4];
  int num = 0;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      if (x < width-1) //right
      {
        edges[num].a = y * width + x;
        edges[num].b = y * width + (x+1);


        //edges[num].w = diff(rgbs, x, y, x + 1, y);
        //edges[num].w = norm(img_.at<Vec3b>(y, x),img_.at<Vec3b>(y,x + 1));//by deafualt,normType=NORM_L2
        edges[num].w = norm(img_.at<Vec3f>(y, x), img_.at<Vec3f>(y, x + 1));//by deafualt,normType=NORM_L2
        num++;
      }

      if (y < height-1)//bottom
      {
        edges[num].a = y * width + x;
        edges[num].b = (y+1) * width + x;

        //edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x, y+1);
        edges[num].w = norm(img_.at<Vec3f>(y, x), img_.at<Vec3f>(y + 1, x));
        num++;
      }

      if ((x < width-1) && (y < height-1)) {//bottom right
        edges[num].a = y * width + x;
        edges[num].b = (y+1) * width + (x+1);

        //edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x+1, y+1);
        edges[num].w = norm(img_.at<Vec3f>(y, x), img_.at<Vec3f>(y + 1, x + 1));

        num++;
      }

      if ((x < width-1) && (y > 0))//up right
      {
        edges[num].a = y * width + x;// find the component id to which they are subject
        edges[num].b = (y-1) * width + (x+1);
        //edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x+1, y-1);
        edges[num].w = norm(img_.at<Vec3f>(y, x), img_.at<Vec3f>(y - 1, x + 1));

        num++;
      }
    }
  }

  // segment the graph to sub graphs
  /*
  * width*height: component number before mergence
  * num :         component number after mergence
  * c:            correspond to the k in fomula (5)
  */
  universe *u = segment_graph(width*height, num, edges, c);
  // post process small components

  for (int i = 0; i < num; i++) {
    int a = u->find(edges[i].a);
    int b = u->find(edges[i].b);
    if ((a != b) && ((u->size(a) < min_size) || (u->size(b) < min_size)))
      u->join(a, b); // as we have ranked the edges by weights so ,
                     // if the compent'size is less than the threshold it will merge with the most similar component
  }
  delete [] edges;
  *num_ccs = u->num_sets();

  Mat output(height,width,CV_8UC3);

  // pick random colors for each component
  //rgb *colors = new rgb[width*height];
  //for (int i = 0; i < width*height; i++)
  //  colors[i] = random_rgb();
  Mat colors(1, num, CV_8UC3);
  randu(colors, Scalar::all(0), Scalar::all(255));
  std::map<int, std::vector<Point> > pointsMap;

  int min = 99999999, max = 0;
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
		int comp = u->find(y * width + x);
		Point point;
		point.x = x;
		point.y = y;
		auto iter = pointsMap.find(comp);
		if (iter != pointsMap.end()) {
			pointsMap[comp].push_back(point);
		} else {
			std::vector<Point> points;
			points.push_back(point);
			pointsMap[comp] = points;
		}
		//if(u->elts[comp].rank>2)
		output.at<Vec3b>(y, x) = colors.at<Vec3b>(0,comp);

	}
  }

 // std::vector<Point2f> vp2f;
  for (auto iter = pointsMap.begin(); iter != pointsMap.end(); iter++) {
	  auto points = iter->second;
	  int length = points.size();

	  //面积阈值的筛选,如果相机有调整的话注意调整
      if (length >= 1000 && length <= 20000) {
		  RotatedRect rectPoint = minAreaRect(points);
		  Point2f fourPoint2f[4];

		  rectPoint.points(fourPoint2f);

		  //通过矩形的宽高比来过滤
		  int width = rectPoint.size.width;
		  int height = rectPoint.size.height;
		  float ratio;
		  if (width > height) {
			  ratio = (float)height / (float)width;
		  } else {
			  ratio = (float)width / (float)height;
		  }
		  /*if (ratio >= 0.5 || ratio <= 0.3) {
			  continue;
		  }*/

		  for (int i = 0; i < 3; i++)  
		  {  
			  line(output, fourPoint2f[i], fourPoint2f[i + 1]  
                      , Scalar(255, 255, 255), 3);

		  }  

		  line(output, fourPoint2f[0], fourPoint2f[3]  
				  , Scalar(255, 255, 255), 3);
     //      printf("point %d  %d  %d  %d\n",fourPoint2f[0],fourPoint2f[1],fourPoint2f[2],fourPoint2f[3]);

		  std::cout<<length<<std::endl;
          for(int i=0; i<4; i++)
              vp2f.push_back(fourPoint2f[i]);
       //   std::cout<<fourPoint2f[0].x<<","<<fourPoint2f[0].y<<std::endl;
       //   std::cout<<fourPoint2f[3].x<<","<<fourPoint2f[3].y<<std::endl;
       //   std::cout<<"length:"<<vp2f.size()<<std::endl;

	  }

  }
 // for(int i=0; i<vp2f.size(); i++)   //打印矩形框角点
 //     std::cout<<"save point:"<< vp2f[i]<< std::endl;
  delete u;

  return output;
}

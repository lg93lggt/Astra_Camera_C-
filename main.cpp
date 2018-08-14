#include <iostream>
#include <opencv2/opencv.hpp>
#include <OpenNI.h>
using namespace cv;


#define WINDOW_NAME  "DisplayWindow"           //为窗口标题定义的宏

Point draw_point;        // 扫描按下的键的坐标
bool mouse_signal=false;   // 扫描按下的键的标志

void on_MouseHandle(int event, int x, int y, int flags, void *param){
	switch( event)
	{
		//鼠标移动消息
	case EVENT_LBUTTONDOWN: 
		{
		  mouse_signal=true;
		  draw_point.x=x;
		  draw_point.y=y;	
// 		  std::cout<<x<<"  "<<y<<std::endl;
		}  
		break;
	default :
	  break;

	}
}
int main(int argc, char **argv) {
    openni::Status rc = openni::STATUS_OK;
    openni::Device device;
    openni::VideoStream depth, color;
    cv::Mat color_image=cv::Mat(480,640,CV_8UC3);
    
    namedWindow(WINDOW_NAME);
    
    setMouseCallback(WINDOW_NAME,on_MouseHandle,(void*)&color_image);
    
//     检查摄像头并创建获取图像的数据流
    const char* deviceURI = openni::ANY_DEVICE;
    if (argc > 1)
    {
	    deviceURI = argv[1];
    }

    rc = openni::OpenNI::initialize();
    std::cout<<"After initialization: "<<openni::OpenNI::getExtendedError()<<std::endl;
    rc = device.open(deviceURI);
    if (rc != openni::STATUS_OK)
    {
	std::cout<<"Device open failed: "<<openni::OpenNI::getExtendedError()<<std::endl;
	openni::OpenNI::shutdown();
	return 1;
    }
    rc = depth.create(device, openni::SENSOR_DEPTH);
    if (rc == openni::STATUS_OK)
    {
	rc = depth.start();
	if (rc != openni::STATUS_OK)
	{
	    std::cout<<"Couldn't find depth stream: "<<openni::OpenNI::getExtendedError()<<std::endl;
	    depth.destroy();
	}
    }
    else
    {
	std::cout<<"Couldn't find depth stream: "<<openni::OpenNI::getExtendedError()<<std::endl;
    }
    
    
    rc = color.create(device, openni::SENSOR_COLOR);
    if (rc == openni::STATUS_OK)
    {
	rc = color.start();
	if (rc != openni::STATUS_OK)
	{
	    std::cout<<"Couldn't find color stream: "<<openni::OpenNI::getExtendedError()<<std::endl;
	    color.destroy();
	}
    }
    else
    {
	std::cout<<"Couldn't find color stream: "<<openni::OpenNI::getExtendedError()<<std::endl;
    }
    std::cout<<"valid judge"<<std::endl;

    if (!depth.isValid() || !color.isValid())
    {
	    std::cout<<"No valid streams. Exiting: "<<openni::OpenNI::getExtendedError()<<std::endl;
	    openni::OpenNI::shutdown();
	    return 2;
    }
    std::cout<<"over"<<std::endl;
    openni::VideoMode colorVideoMode=color.getVideoMode();
    openni::VideoMode depVideoMode=depth.getVideoMode();
  
//     输出彩色和深度图像的分辨率和数据格式
    std::cout<<colorVideoMode.getResolutionX()<<"   "<<colorVideoMode.getResolutionY()<<"   "<<colorVideoMode.getFps()<<"   "<<colorVideoMode.getPixelFormat()<<std::endl;
    std::cout<<depVideoMode.getResolutionX()<<"   "<<depVideoMode.getResolutionY()<<"   "<<depVideoMode.getFps()<<"   "<<depVideoMode.getPixelFormat()<<std::endl;
    
    
    openni::VideoStream** m_streams;
    m_streams = new openni::VideoStream*[2];
    m_streams[0] = &depth;
    m_streams[1] = &color;
    while(1)
    {
//       获取图像
	int changedIndex;
	rc = openni::OpenNI::waitForAnyStream(m_streams, 2, &changedIndex);
	if (rc != openni::STATUS_OK)
	{
	    std::cout<<"Wait failed "<<std::endl;
	    return -1;
	}
	
	openni::VideoFrameRef m_depthFrame;
	openni::VideoFrameRef m_colorFrame;
	   
	color.readFrame(&m_colorFrame);
	depth.readFrame(&m_depthFrame);
	if(!m_colorFrame.isValid()||!m_depthFrame.isValid())
	{
	  std::cout<<"get image error"<<std::endl;
	}	
	
	cv::Mat dep_image=cv::Mat(480,640,CV_16UC1);
	cv::Mat dep_image_c2=cv::Mat::zeros(480,640,CV_8UC3);
	
	const openni::RGB888Pixel* pImageRow = (const openni::RGB888Pixel*)m_colorFrame.getData();
	
	for(int row=0;row<480;row++)
	{
	  uchar *data=color_image.ptr<uchar>(row);
	  for(int col=0;col<640;col++)
	  {
	    int col_temp=col*3;
	    data[col_temp]=pImageRow->b;col_temp++;
	    data[col_temp]=pImageRow->g;col_temp++;
	    data[col_temp]=pImageRow->r;
	    pImageRow++;
	  }
	}
	cv::imshow(WINDOW_NAME,color_image);
	int max_vla=0;
	const openni::DepthPixel* pDepthRow = (const openni::DepthPixel*)m_depthFrame.getData();
	for(int row=0;row<480;row++)
	{
	  unsigned short *data=dep_image.ptr<unsigned short>(row);
	  unsigned char *data_c2=dep_image_c2.ptr<unsigned char>(row);
	  
	  for(int col=0;col<640;col++)
	  {
	     unsigned short dep_data_temp=(*pDepthRow);
	     data[col]=dep_data_temp;
	     int col_temp=3*col;
	     data_c2[col_temp]=dep_data_temp;col_temp++;
	     data_c2[col_temp]=dep_data_temp>>8;	    
	     pDepthRow++;
	  }    
	}
	cv::imshow("dep_image",dep_image);
	cv::imshow("dep_image_c2",dep_image_c2);
	if(mouse_signal)
	{
	    mouse_signal=false;
	    int xxx=draw_point.x*3;
	    std::cout<<(int)color_image.ptr<unsigned char>(draw_point.y)[xxx]<<"  "<<(int)color_image.ptr<unsigned char>(draw_point.y)[xxx+1]<<"  ";
	    std::cout<<(int)color_image.ptr<unsigned char>(draw_point.y)[xxx+2];
	    std::cout<<"  "<<dep_image.ptr<unsigned short>(draw_point.y)[draw_point.x]<<std::endl;
	}
// 	cv::imwrite("dep.jpeg",dep_image_c2);
	

	if(cv::waitKey(30)==27)
	{
	  
	  break;
	}
      
    }
    
    return 0;
}

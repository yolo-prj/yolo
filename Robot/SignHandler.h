#ifndef __SINGHANDLER_CLASS__
#define __SINGHANDLER_CLASS__


#include <boost/thread.hpp>
#include <signal.h>



#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>



using namespace cv;
using namespace std;



class CSignHandler
{
public:
	CSignHandler() {}
	~CSignHandler() {}
	virtual void onSignAction(int id, string action)
	{
	}
	
};


class CSignImage
{
public : 
	Mat m_img;
	string name;
	string m_actionname;
	int m_id;

	CSignHandler * m_handler;

public:

	CSignImage(int id, string filename, string action, CSignHandler * handler);


	int GetDiff(Mat & cam_sign);

	void CallHandler();

	~CSignImage();


private:
	CSignImage();
	
};



#endif

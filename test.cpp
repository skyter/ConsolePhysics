#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <curses.h>
#include <sys/time.h>

using namespace std;
using namespace cv;

Size gPhySize(100, 100);
Size gConSize(100, 40);

double getSec (void)

{
	struct timeval tv;
	double ret;

	gettimeofday(&tv, NULL);
	ret = tv.tv_sec;
	ret += (double)tv.tv_usec / 1000000;
	//fprintf (stderr, "%lu %lu %.17g\n", tv.tv_sec, tv.tv_usec, ret);
	return ret;
}

Point2f getPT (Point2f pt)
{
	Point2f npt;
	npt.x = pt.x * gConSize.width / gPhySize.width;
	npt.y = pt.y * gConSize.height / gPhySize.height;
	npt.y = gConSize.height - npt.y;
	if (npt.x > gConSize.width)
		npt.x = gConSize.width;
	if (npt.y > gConSize.height)
		npt.y = gConSize.height;
	if (npt.x < 0)
		npt.x = 0;
	if (npt.y < 0)
		npt.y = 0;
	return npt;
}

void DrawPos (Point2f org, Point2f pos)
{
	Point op, np;
	op = getPT (org);
	np = getPT (pos);
	move (op.y, op.x);
	echochar (' ');
	move (np.y, np.x);
	echochar ('A');
	move (gConSize.height, 0);
	//cout << "org" << org << " now " << pos << " op " << op << " np " << np << endl;// %.17g now %.17g", org, pos);
	move (gConSize.height, 0);
}

int main()
{
	Point2f pos(0,0);
	double grav = 9.8;
	double damper = 1;
	double up = 100;
	double nowV = 0;
	initscr();
	cbreak();
	nonl();
	noecho();
	intrflush(stdscr,FALSE);
	keypad(stdscr,TRUE);
	curs_set(0);
	timeout(0);

#if 0
	move(y,x)       將游標移動至 x,y 的位置
	getyx(win,y,x)  得到目前游標的位置
	(請注意! 是 y,x 而不是 &y,&x 

	clear()
	erase()         將整個螢幕清除
	(請注意配合 refresh() 使用)
#endif

#if 0
	for (int x=0; x < 100; x++) {
		for (int y=0; y < 20; y++) {
			move (y, x);
			echochar ('A');
			usleep (10000);
			move (y, x);
			echochar (' ');
		}
	}
#endif

	while (1) {
		static double lastT = 0;
		usleep(10000);
		if (lastT == 0) {
			/* Init */
			lastT = getSec();
			DrawPos(pos, pos);
			continue;
		}

		Point2f org = pos;
		double nowT = getSec();
		double diffT = nowT - lastT;
		double accele = -grav;
		int ch;

		lastT = nowT;
		ch = getch();
		if (ch == KEY_UP) {
			accele += up;
			//move (gConSize.height, 0);
		}
		else if (ch == 'q') {

			break;
		} else {
			move (gConSize.height + 1, 0);
		}


		pos.y += nowV * diffT + accele * diffT * diffT * 0.5; /* V0 * t + 1/2 * a * t ^ 2 */
		nowV += accele * diffT;
		if (pos.y > gPhySize.height) {
			pos.y = gPhySize.height;
			nowV = 0;
		}
		if (pos.y < 0) {
			pos.y = 0;
			nowV = 0;
		}
		
		DrawPos(org, pos);
		move (gConSize.height + 1, 20);
		char buf[256];
		snprintf (buf, sizeof (buf), "V=%.6g POS=(%.6g, %.6G)", nowV, pos.x, pos.y);
		addstr (buf);
		if (ch == KEY_UP) {
			move (gConSize.height + 1, 0);
			addstr ("UP");
		}
		else if (ch == 'q') {

		} else {
			move (gConSize.height + 1, 0);
			addstr ("  ");
		}
		continue;
	}



	endwin();
	getSec();
	getSec();
	return 0;
}


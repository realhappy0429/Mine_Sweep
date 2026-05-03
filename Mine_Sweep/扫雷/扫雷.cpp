#include <iostream>
#include<vector>
#include<random>
#include<easyx.h>
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")

#include <iomanip>  //控制对齐
using namespace std;
//定义行列常量
#define COL 10
#define ROW 10

//地雷数
const int MineNum = 10;

void boom(vector<vector<int>>& map, int r, int c);


//定义图片资源
IMAGE imgs[13];
//加载资源
void loadResources() {
	for (int i = 0; i < 13; i++) {
		char imgPath[50] = { 0 };
		sprintf_s(imgPath, "./images/%d.png", i);
		loadimage(&imgs[i], imgPath);
	}
}



//展示地图
void show(vector<vector<int>>& map) {

	cout << right;
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			cout <<setw(3) <<map[i][k];
		}
		cout << endl;
	}
}
//初始化
void init(vector<vector<int>>& map) {

	loadResources();
	//随机设置雷的位置
	for (int i = 0; i < MineNum; i++) {
		int r = rand() % ROW;
		int c = rand() % COL;
		while(map[r][c] == -1) {
			r = rand() % ROW;
			c = rand() % COL;
		}
		map[r][c] = -1;
	}
	//对雷附近的九宫格都+1
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			if (map[i][k] == -1) {
				for (int r = i - 1; r < i + 2; r++) {
					for (int c = k - 1; c < k + 2; c++) {
						if ((r>=0&&r<ROW&&c>=0&&c<COL)&&map[r][c] != -1) {
							map[r][c]++;
						}
					}
				}
			}
		}
	}

	//加密格子
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			map[i][k] += 10;
		}
	}
}

//绘制
void draw(vector<vector<int>>& map) {
	//进行贴图
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {

			int x = i * 60;
			int y = k * 60;

			if (map[i][k] >= 0&&map[i][k]<=8) {
				putimage(y, x, &imgs[map[i][k]]);
			}
			else if(map[i][k]==-1) {
				putimage(y, x, &imgs[9]);
			}
			else if (map[i][k] <=18 && map[i][k] >=9) {
				putimage(y, x, &imgs[10]);
			}
			else if (map[i][k]>=29&&map[i][k]<=38) {
				putimage(y, x, &imgs[11]);
			}
			else if (map[i][k]==-2) {
				putimage(y, x, &imgs[12]);
			}
		}
	}
}

//鼠标操作
void mousePro(ExMessage* msg, vector<vector<int>>& map) {

	int c = msg->x / 60;
	int r = msg->y / 60;
	//左键打开
	if (msg->message == WM_LBUTTONDOWN) {

		if (map[r][c] >=9 && map[r][c] <= 18) {
			map[r][c] -=10;
			//空白格传播
			boom(map, r, c);
			system("cls");
			show(map);
		}
	}
	//右键标记
	if (msg->message == WM_RBUTTONDOWN) {

		if (map[r][c] >= 29 && map[r][c] <= 38) {
			map[r][c] -= 20;
			system("cls");
			show(map);
		}
		else if (map[r][c] >=9 && map[r][c] <= 18) {
			map[r][c] += 20;
			system("cls");
			show(map);
		}
	}
}

//点击空白格，连环爆开,当前点击的坐标r,c
void boom(vector<vector<int>>& map, int r, int c) {
	//判断是否为空白
	if (map[r][c] == 0) {
		for (int i = r - 1; i < r + 2; i++) {
			for (int k = c - 1; k < c + 2; k++) {
				if ((i >= 0 && i < ROW && k >= 0 && k < COL)) {
					if (map[i][k] >=10 && map[i][k] <= 18) {
						map[i][k] -= 10;
						//继续递归调用
						boom(map, i, k);
					}
				}
			}
		}
	}
}

//判断游戏结束

int gameOver(vector<vector<int>>& map,int r,int c) {

	//点到雷，输了
	if (map[r][c] == -1) {
		map[r][c] =-2;
		return -1;
	}
	//点完格子
	int cnt = 0;
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			if (map[i][k] >= 0 && map[i][k] <= 8) {
				cnt++;
			}
		}
	}
	if (cnt == ROW * COL - MineNum) {
		return 1;
	}
	return 0;

}

int main()
{
	//创建窗口
	initgraph(600,600);
	//播放音乐bgm
	mciSendString("open ./images/start.mp3 alias bgm", NULL, 0, NULL);
	mciSendString("play bgm", NULL, 0, NULL);
	//设置随机化种子
	srand((unsigned)time(nullptr));
	//扫雷地图
	vector<vector<int>> map(ROW, vector<int>(COL,0));
	init(map);
	//show(map);
	while (true) {

		//处理消息
		ExMessage msg;
		while (peekmessage(&msg,EX_MOUSE)) {
			switch (msg.message) {
			case WM_LBUTTONDOWN:
				mousePro(&msg, map);
				//得到游戏返回值
				{
					int ret = gameOver(map, msg.y / 60, msg.x / 60);
					if (ret == -1) {
						draw(map);
						MessageBox(GetHWnd(), "游戏结束——你输了！", "提示", MB_OK);
						exit(0);
					}
					else if (ret == 1) {
						MessageBox(GetHWnd(), "恭喜你——你赢了！", "提示", MB_OK);
						exit(0);
					}
				}
				break;
			case WM_RBUTTONDOWN:
				mousePro(&msg, map);
				break;
			}
		}
		draw(map);
	}

	system("pause");
	closegraph();
	
	return 0;
}


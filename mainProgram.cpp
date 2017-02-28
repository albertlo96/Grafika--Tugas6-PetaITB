#include "Point.h"
#include "Color.h"
#include "Screen.h"
#include "LineDrawer.h"
#include "shape.h"
#include "Shapeclip.h"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <termios.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

//KEYPRESS HANDLER==============================
static struct termios old, news;

vector<Shape> readBangunanAndJalan(string path){
	int pertama = 0;
	ifstream infile;
	string sline;
	infile.open(path); // open file bangunan
	vector<Shape> VP; //Vector of shapes
	vector<Point> vec_temp; //Vector of points
	
	if (!infile.good()){
		cout << "File not found";
	} else {
		while(!infile.eof()){
	        getline(infile, sline); // save satu baris ke sline
	        if (infile.eof()){
				break;
			}
	        if((sline.length()<=4) && (pertama != 0)){
				Shape tempShape(vec_temp, Color(255,255,255));
				VP.push_back(tempShape);
				vec_temp.clear();
			} else {
				if (pertama == 0){
					pertama = 1;
				} else {
					stringstream lineStream(sline);
					// Read an integer at a time from the line
					int x;
					int y;
					lineStream >> x;
					lineStream >> y;
					Point temp(x,y);
					vec_temp.push_back(temp);
				}
			}
		}
	}
	//elemen terakhir
	Shape tempShape(vec_temp, Color(255,255,255));
	VP.push_back(tempShape);
	
	infile.close(); // close file
	return VP;
}

vector<Shapeclip> readBangunanAndJalanClip(string path){
	int pertama = 0;
	ifstream infile;
	string sline;

	infile.open(path); // open file bangunan
	vector<Shapeclip> VP; //Vector of vector of points
	vector<Point> vec_temp;
	
	
	
	
	if (!infile.good()){
		cout << "File not found";
	} else {
		while(!infile.eof()){
	        getline(infile, sline); // save satu baris ke sline
	        if (infile.eof()){
				break;
			}
	        if((sline.length()<=4) && (pertama != 0)){
				Shapeclip tempShape(vec_temp, Color(255,255,255));
				VP.push_back(tempShape);
				vec_temp.clear();
			} else {
				if (pertama == 0){
					pertama = 1;
				} else {
					stringstream lineStream(sline);
					// Read an integer at a time from the line
					int x;
					int y;
					lineStream >> x;
					lineStream >> y;
					Point temp(x,y);
					vec_temp.push_back(temp);
				}
			}
		}
	}
	//elemen terakhir
	Shapeclip tempShape(vec_temp, Color(255,255,255));
	VP.push_back(tempShape);
	
	infile.close(); // close file
	return VP;
}
	
/*Init Termios*/
void initTermios(int echo) 
{
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  news = old; /* make new settings same as old settings */
  news.c_lflag &= ~ICANON; /* disable buffered i/o */
  news.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
  tcsetattr(0, TCSANOW, &news); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) 
{
  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo) 
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char getch(void) 
{
  return getch_(0);
}
//=============================================

Screen screen;
LineDrawer linedrawer;
LineDrawer linedrawer2;

//MAIN=========================================
int main(){
	linedrawer.setView(Point(2,2) , Point(screen.getWidth()/2-20, screen.getHeight()-30));
	linedrawer2.setView(Point(770,100) , Point(930, 280));
	/* MEMBUAT KOTAK*/
	vector<Point> body;
	body.push_back(Point(20,50));
	body.push_back(Point(80,50));
	body.push_back(Point(80,110));
	body.push_back(Point(20,110));
	Shape kotak(body, Color(255,255,255));
	kotak.draw();
	
	//Vector of Vector of Bangunan
	vector<Shape> vec_bangunan = readBangunanAndJalan("DataGambar/dataBangunan.txt"); 
	vector<Shapeclip> vec_bangunanClip = readBangunanAndJalanClip("DataGambar/dataBangunan.txt"); 
	vector<Shape> vec_jalan = readBangunanAndJalan("DataGambar/dataPohon.txt");

	
	
	for(int i = 0; i < vec_bangunan.size(); i++){
		vec_bangunan[i].draw();
	}
	int titikAwalX = 730;
	int titikAwalY = 0;
	int scalefactor = 2;
	for(int i = 0; i < vec_bangunanClip.size(); i++){
		for(int j=0; j<vec_bangunanClip[i].edges.size(); j++){
			vec_bangunanClip[i].edges[j].x *= 2;
			vec_bangunanClip[i].edges[j].y *= 2;
		}	
	}
	
	for(int i = 0; i < vec_bangunanClip.size(); i++){
		vec_bangunanClip[i].moveBy(750, 0);
		vec_bangunanClip[i].draw();	
	}
	
	/*GERAKAN KOTAK*/
    while(1){
        int c = getch();
		int n;
        int yl, yr;
		switch(c){
			case 'W':
				case 'w':
					for(int i = 0; i < vec_bangunan.size(); i++){
						vec_bangunan[i].erase();
					}
					kotak.moveBy(0, -10);
					
					for(int i = 0; i < vec_bangunanClip.size(); i++){
						vec_bangunanClip[i].erase();
					}
					titikAwalX += 0;
					titikAwalY += 10; 
					for(int i = 0; i < vec_bangunanClip.size(); i++){
						vec_bangunanClip[i].moveBy(0, 10*scalefactor);
						vec_bangunanClip[i].draw();
					}
	
					for(int i = 0; i < vec_bangunan.size(); i++){
						vec_bangunan[i].draw();
						
					}
					break;
			case 'A':
				case 'a':
					for(int i = 0; i < vec_bangunan.size(); i++){
						vec_bangunan[i].erase();
					}
					kotak.moveBy(-10, 0);
					for(int i = 0; i < vec_bangunanClip.size(); i++){
						vec_bangunanClip[i].erase();
					}
					
					titikAwalX += 10;
					titikAwalY -= 0; 
					for(int i = 0; i < vec_bangunanClip.size(); i++){
						vec_bangunanClip[i].moveBy(10*scalefactor, 0);
						vec_bangunanClip[i].draw();
					}
					
					for(int i = 0; i < vec_bangunan.size(); i++){
						vec_bangunan[i].draw();
					}
					break;
			case 's':
				case 'S':
					for(int i = 0; i < vec_bangunan.size(); i++){
						vec_bangunan[i].erase();
					}
					kotak.moveBy(0, 10);
					titikAwalX += 0;
					titikAwalY -= 10; 
					for(int i = 0; i < vec_bangunanClip.size(); i++){
						vec_bangunanClip[i].erase();
					}
					
					for(int i = 0; i < vec_bangunanClip.size(); i++){
						vec_bangunanClip[i].moveBy(0, -10*scalefactor);
						vec_bangunanClip[i].draw();
					}
					for(int i = 0; i < vec_bangunan.size(); i++){
						vec_bangunan[i].draw();
					}
					break;
			case 'D':
				case 'd':
					for(int i = 0; i < vec_bangunan.size(); i++){
						vec_bangunan[i].erase();
					}
					kotak.moveBy(10, 0);
					for(int i = 0; i < vec_bangunanClip.size(); i++){
						vec_bangunanClip[i].erase();
					}
					titikAwalX -= 10;
					titikAwalY -= 0; 
					for(int i = 0; i < vec_bangunanClip.size(); i++){
						vec_bangunanClip[i].moveBy(-10*scalefactor, 0);
						vec_bangunanClip[i].draw();
					}
					
					for(int i = 0; i < vec_bangunan.size(); i++){
						vec_bangunan[i].draw();
					}
					break;
			case 'I':
				case 'i':
					scalefactor += 0.5;
					for(int i = 0; i < vec_bangunan.size(); i++){
						vec_bangunan[i].erase();
					}
					
					for(int i = 0; i < vec_bangunanClip.size(); i++){
						vec_bangunanClip[i].erase();
					}	
								
					for(int i = 0; i < vec_bangunanClip.size(); i++){
						for(int j=0; j<vec_bangunanClip[i].edges.size(); j++){
							vec_bangunanClip[i].edges[j].x *= 1.5;
							vec_bangunanClip[i].edges[j].y *= 1.5;
						}	
							
					}
					
					for(int i = 0; i < vec_bangunanClip.size(); i++){
						vec_bangunanClip[i].moveBy(scalefactor*10-titikAwalX, scalefactor*10-titikAwalY);
						vec_bangunanClip[i].draw();
					}
					
					for(int i = 0; i < vec_bangunan.size(); i++){
						vec_bangunan[i].draw();
					}
					
				break;
				case 'O':
					case 'o':
						scalefactor -= 0.5;
						for(int i = 0; i < vec_bangunan.size(); i++){
							vec_bangunan[i].erase();
						}
						
						for(int i = 0; i < vec_bangunanClip.size(); i++){
							vec_bangunanClip[i].erase();
						}	
									
						for(int i = 0; i < vec_bangunanClip.size(); i++){
							for(int j=0; j<vec_bangunanClip[i].edges.size(); j++){
								vec_bangunanClip[i].edges[j].x *= 0.5;
								vec_bangunanClip[i].edges[j].y *= 0.5;
							}	
								
						}
						
						for(int i = 0; i < vec_bangunanClip.size(); i++){
							vec_bangunanClip[i].moveBy(scalefactor*10+titikAwalX, scalefactor*10+titikAwalY);
							vec_bangunanClip[i].draw();
						}
						
						for(int i = 0; i < vec_bangunan.size(); i++){
							vec_bangunan[i].draw();
						}
					
				break;
			case 45:
				cout << "- was pressed \n";
				break;
			default:
				break;
		}
	}
	return 0;
}

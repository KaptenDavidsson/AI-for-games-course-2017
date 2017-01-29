#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstdint>
#include <tuple>
#include <algorithm>
#include <array>
#include <vector>
#include <time.h>
#include "glut.h"


GLubyte ubImage[65536];
const int img_width = 800;
const int img_height = 800;
const int width = 40;
const int height = 40;
std::array<std::array<double, width>, height> maze = {};
std::tuple<int, int>walls[width][height] = {};
std::tuple<int, int> dirs[4];
std::array<std::array<double, 4>, 4> markov = {};

static void DrawRectangle(int x, int y, int w, int h) {
	glRasterPos3f(-0.5 + 1.0/width*x, 0.5 - 1.0/height*y, -1);
	glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, ubImage);
}

static void CreateImage() {
	DrawRectangle(0, height-1, width*(img_width/(2*width)), 1);
	DrawRectangle(width, height-1, 1, height*(img_height / (2 * height)));

	for (int x=0; x < width; x++) {
		for (int y=0; y < height; y++) {
			int l = std::get<0>(walls[x][y]);
			int t = std::get<1>(walls[x][y]);
			if (l == 1) {
				DrawRectangle(x,y, 1, (img_width / (2 * width)));
			}
			if (t == 1) {
				DrawRectangle(x,y-1, (img_height / (2 * height)), 1);
			}
		}
	}
}


//Return the directions in a random order based on the markov matrix
static std::vector<std::tuple<int, int>> SampleDir(std::array<double, 4> prob, std::vector<std::tuple<int, int>> newDirs)
{
	double prob_sum = 0;
	for (int i = 0; i < prob.size(); i++) {
		prob_sum += prob[i];
	}

	int r = rand() % int(prob_sum * 100);
	if (r < prob[0] * 100) {
		newDirs.push_back(dirs[0]);
		prob[0] = 0;
	}
	else if (r < (prob[0] + prob[1]) * 100) {
		newDirs.push_back(dirs[1]);
		prob[1] = 0;
	}
	else if (r < (prob[0] + prob[1] + prob[2]) * 100) {
		newDirs.push_back(dirs[2]);
		prob[2] = 0;
	}
	else {
		newDirs.push_back(dirs[3]);
		prob[3] = 0;
	}

	if (newDirs.size() < 4) {
		newDirs = SampleDir(prob, newDirs);
	}

	return newDirs;
}

static std::vector<std::tuple<int, int>> ShuffleDirs(std::tuple<int, int> prevDir)
{
	int index = std::distance(dirs, std::find(dirs, dirs + 4, prevDir));
	std::array<double, 4> prob = markov[index];
	std::vector<std::tuple<int, int>> newDirs = {};
	newDirs = SampleDir(prob, newDirs);

	return newDirs;
}

//Improved DFS maze generation algorithm
static void GenerateMaze(int x, int y, std::tuple<int, int> prevDir) {
	maze[x][y] = 1;

	std::vector<std::tuple<int, int>> newDirs = ShuffleDirs(prevDir);

	for (int i=0; i < 4; i++) {
		std::tuple<int, int> current_dir = newDirs[i];

		int xx = x + std::get<0>(current_dir);
		int yy = y + std::get<1>(current_dir);
		if (xx >= 0 && xx < width && yy >= 0 && yy < height && maze[xx][yy] != 1) {
			if (xx > x) {
				walls[xx][yy] = std::tuple<int,int> { 0, std::get<1>(walls[xx][yy]) };
			}
			if (xx < x) {
				walls[x][y] = std::tuple<int, int> { 0, std::get<1>(walls[x][y]) };
			}
			if (yy > y) {
				walls[xx][yy] = std::tuple<int, int> { std::get<0>(walls[xx][yy]), 0 };
			}
			if (yy < y) {
				walls[x][y] = std::tuple<int, int> { std::get<0>(walls[x][y]), 0 };
			}

			GenerateMaze(xx, yy, current_dir);
		}
	}
}

//Define and pick a markov matrix
static std::array<std::array<double, 4>, 4> PickMarkov(int i) {
	std::vector<std::array<std::array<double, 4>, 4>> markovs = {};

	markovs.push_back( { { { .91, .03, .03, .03 },
						   { .03, .91, .03, .03 },
						   { .03, .03, .91, .03 },
						   { .03, .03, .03, .91 } } });

	markovs.push_back({ { { .01, .33, .33, .33 },
						  { .33, .01, .33, .33 },
						  { .33, .33, .01, .33 },
						  { .33, .33, .33, .01 } } });

	markovs.push_back({ { { .91, .03, .03, .03 },
						  { .91, .03, .03, .03 },
						  { .91, .03, .03, .03 },
						  { .91, .03, .03, .03 } } });


	markovs.push_back({ { { .03, .91, .03, .03 },
						  { .03, .03, .91, .03 },
						  { .03, .03, .03, .91 },
						  { .91, .03, .03, .03 } } });

	return markovs[i];
}

static void InitMaze() 
{
	dirs[0] = std::tuple<int, int> { 1, 0 };
	dirs[1] = std::tuple<int, int> { 0, -1 };
	dirs[2] = std::tuple<int, int> { -1, 0 };
	dirs[3] = std::tuple<int, int> { 0, 1 };

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < width; y++) {
			walls[x][y] = std::tuple<int, int>{ 1, 1 };
		}
	}

	srand(time(NULL));
	markov = PickMarkov(rand() % 4);
}

static void InitGL(void)
{
	GLsizei imgWidth = 800;
	gluPerspective(90.0, 1.0, 0.8, 300.0);
	GLubyte *img = ubImage;
	for (int j = 0; j < 50 * imgWidth; j++) {
		*img++ = 0xff;
	}
}

//Exit with escape
static void Key(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
	}
}

static void Draw(void)
{
	CreateImage();
	glFlush();
}

int main(int argc, char **argv)
{
	InitMaze();
	GenerateMaze(0, 0, std::tuple<int, int> { 1, 0 });
	
	glutInitWindowSize(img_width, img_height);
	glutInit(&argc, argv);
	glutCreateWindow("Markov Maze");

	InitGL();
	glutKeyboardFunc(Key);
	glutDisplayFunc(Draw);
	glutMainLoop();
	return 0;             
}

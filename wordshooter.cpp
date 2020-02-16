#ifndef WORD_SHOOTER_CPP
#define WORD_SHOOTER_CPP

//#include <GL/gl.h>
//#include <GL/glut.h>
#include <iostream>
#include<string>
#include<cmath>
#include<fstream>
#include "util.h"
using namespace std;
#define MAX(A,B) ((A) > (B) ? (A):(B)) // defining single line functions....
#define MIN(A,B) ((A) < (B) ? (A):(B))
#define ABS(A) ((A) < (0) ? -(A):(A))
#define FPS 10

string * dictionary;
int dictionarysize = 370099;
#define KEY_ESC 27 // A
#define KEY_ENTER 13 // enter key
#define KEY_ONE 49 // 1
#define KEY_TWO 50 // 2
#define KEY_THREE 51 // 3

// 20,30,30
const int bradius = 30; // ball radius in pixels...

int width = 930, height = 660;
int byoffset = bradius;

int nxcells = (width - bradius) / (2 * bradius);
int nycells = (height - byoffset /*- bradius*/) / (2 * bradius);
int nfrows = 2; // initially number of full rows //
int score = 0;
int **board; // 2D-arrays for holding the data...
int bwidth = 130;
int bheight = 10;
int bsx, bsy;
const int nalphabets = 26;
enum alphabets {
	AL_A, AL_B, AL_C, AL_D, AL_E, AL_F, AL_G, AL_H, AL_I, AL_J, AL_K, AL_L, AL_M, AL_N, AL_O, AL_P, AL_Q, AL_R, AL_S, AL_T, AL_U, AL_W, AL_X, AL_y, AL_Z
};
GLuint texture[nalphabets];
GLuint tid[nalphabets];
string tnames[] = { "a.bmp", "b.bmp", "c.bmp", "d.bmp", "e.bmp", "f.bmp", "g.bmp", "h.bmp", "i.bmp", "j.bmp",
"k.bmp", "l.bmp", "m.bmp", "n.bmp", "o.bmp", "p.bmp", "q.bmp", "r.bmp", "s.bmp", "t.bmp", "u.bmp", "v.bmp", "w.bmp",
"x.bmp", "y.bmp", "z.bmp" };
GLuint mtid[nalphabets];
int awidth = 60, aheight = 60; // 60x60 pixels cookies...


//USED THIS CODE FOR WRITING THE IMAGES TO .bin FILE
void RegisterTextures_Write()
//Function is used to load the textures from the
// files and display
{
	// allocate a texture name
	glGenTextures(nalphabets, tid);
	vector<unsigned char> data;
	ofstream ofile("image-data.bin", ios::binary | ios::out);
	// now load each cookies data...

	for (int i = 0; i < nalphabets; ++i) {

		// Read current cookie

		ReadImage(tnames[i], data);
		if (i == 0) {
			int length = data.size();
			ofile.write((char*)&length, sizeof(int));
		}
		ofile.write((char*)&data[0], sizeof(char) * data.size());

		mtid[i] = tid[i];
		// select our current texture
		glBindTexture(GL_TEXTURE_2D, tid[i]);

		// select modulate to mix texture with color for shading
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// when texture area is small, bilinear filter the closest MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
		// when texture area is large, bilinear filter the first MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if wrap is true, the texture wraps over at the edges (repeat)
		//       ... false, the texture ends at the edges (clamp)
		bool wrap = true;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			wrap ? GL_REPEAT : GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			wrap ? GL_REPEAT : GL_CLAMP);

		// build our texture MIP maps
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, awidth, aheight, GL_RGB,
			GL_UNSIGNED_BYTE, &data[0]);
	}
	ofile.close();

}
void RegisterTextures()
/*Function is used to load the textures from the
* files and display*/
{
	// allocate a texture name
	glGenTextures(nalphabets, tid);

	vector<unsigned char> data;
	ifstream ifile("image-data.bin", ios::binary | ios::in);

	if (!ifile) {
		cout << " Couldn't Read the Image Data file ";
		//exit(-1);
	}
	// now load each cookies data...
	int length;
	ifile.read((char*)&length, sizeof(int));
	data.resize(length, 0);
	for (int i = 0; i < nalphabets; ++i) {
		// Read current cookie
		//ReadImage(tnames[i], data);
		/*if (i == 0) {
		int length = data.size();
		ofile.write((char*) &length, sizeof(int));
		}*/
		ifile.read((char*)&data[0], sizeof(char)* length);

		mtid[i] = tid[i];
		// select our current texture
		glBindTexture(GL_TEXTURE_2D, tid[i]);

		// select modulate to mix texture with color for shading
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// when texture area is small, bilinear filter the closest MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
		// when texture area is large, bilinear filter the first MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if wrap is true, the texture wraps over at the edges (repeat)
		//       ... false, the texture ends at the edges (clamp)
		bool wrap = true;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			wrap ? GL_REPEAT : GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			wrap ? GL_REPEAT : GL_CLAMP);

		// build our texture MIP maps
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, awidth, aheight, GL_RGB,
			GL_UNSIGNED_BYTE, &data[0]);
	}
	ifile.close();
}
void DrawAlphabet(const alphabets &cname, int sx, int sy, int cwidth = 60,
	int cheight = 60)
	/*Draws a specfic cookie at given position coordinate
	* sx = position of x-axis from left-bottom
	* sy = position of y-axis from left-bottom
	* cwidth= width of displayed cookie in pixels
	* cheight= height of displayed cookiei pixels.
	* */
{
	float fwidth = (float)cwidth / width * 2, fheight = (float)cheight
		/ height * 2;
	float fx = (float)sx / width * 2 - 1, fy = (float)sy / height * 2 - 1;

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mtid[cname]);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(fx, fy);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(fx + fwidth, fy);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(fx + fwidth, fy + fheight);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(fx, fy + fheight);
	glEnd();

	glColor4f(1, 1, 1, 1);

	//	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//glutSwapBuffers();
}
int GetAlphabet() {
	return GetRandInRange(1, 26);
}

void Pixels2Cell(int px, int py, int & cx, int &cy) {
}
void Cell2Pixels(int cx, int cy, int & px, int &py)
// converts the cell coordinates to pixel coordinates...
{
}
void DrawShooter(int sx, int sy, int cwidth = 60, int cheight = 60)

{
	float fwidth = (float)cwidth / width * 2, fheight = (float)cheight
		/ height * 2;
	float fx = (float)sx / width * 2 - 1, fy = (float)sy / height * 2 - 1;

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, -1);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(fx, fy);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(fx + fwidth, fy);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(fx + fwidth, fy + fheight);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(fx, fy + fheight);
	glEnd();

	glColor4f(1, 1, 1, 1);

	//	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//glutSwapBuffers();
}


//------------------------------------------------------------------------------------------------------------------------

bool time_up = false, fill_coll = false, cleared_board = false;
int mouse_x = 0, mouse_y = 0, paddle_x_change, paddle_y_change;// variables for our mouse coordinates and change in x, y values used to increment paddle x and y to animate them
int msec = 0, wsec = 0, countdown = 120; // variables to set up our timers
float angle; // calculates angle to determine trajectory of ball and if it should stick diagonally
int difficulty = 3;
int index_cell; // stores index of cell in which paddle ball is placed
bool click = false, left_side = false, word_formed_ver = false,word_formed_rhor = false, word_formed_lhor = false;
bool word_formed_ldiag = false, word_formed_rdiag = false, game_start = false, game_end = false;
int collide = 10; // collision variable toggles between 10 and 5 (used to be a bool changed it to int for debugging purposes code was working so did not change it back)
int paddle_ball = 30, paddle_x = width / 2, paddle_y = 10; // variables for the ball at the bottom of our screen
int cells[135][4]; // our board array has 4 attributes x, y coordinates, alphabet number and an exist value(basically a boolean)
int alphas[600]; // hold all our alphabets values set to 600 as assumed that balls used in 2 min period will be less than this
int x_hl, x_hr, x_dl, x_dr, x_v; // initial x positions of formed words to be displayed
int y_hl, y_hr, y_dl, y_dr, y_v; // initial y positions of formed words to be displayed
int vertical, horizontal_left, horizontal_right, diagonal_left, diagonal_right; // length of strings formed when collision detected
string ver, h_left, h_right, d_left, d_right; // strings of balls after collision detected

// this function returns a character value when integers 0-25 are passed through it
char int_to_alpha(int n){
    switch(n){
        case 0:
            return 'a';
        case 1:
            return 'b';
        case 2:
            return 'c';
        case 3:
            return 'd';
        case 4:
            return 'e';
        case 5:
            return 'f';
        case 6:
            return 'g';
        case 7:
            return 'h';
        case 8:
            return 'i';
        case 9:
            return 'j';
        case 10:
            return 'k';
        case 11:
            return 'l';
        case 12:
            return 'm';
        case 13:
            return 'n';
        case 14:
            return 'o';
        case 15:
            return 'p';
        case 16:
            return 'q';
        case 17:
            return 'r';
        case 18:
            return 's';
        case 19:
            return 't';
        case 20:
            return 'u';
        case 21:
            return 'v';
        case 22:
            return 'w';
        case 23:
            return 'x';
        case 24:
            return 'y';
        case 25:
            return 'z';
        default:
            cout << "error" << endl;
            break;
    }
    return '^';
}

//this function flips the string so we can check to see if flipped string is a word
string flipper(int n, const string &s){
    string temp = "";
    n--;
    for(; n >= 0; n--){
        temp += s[n];
    }
    return temp;
}

// this function checks how many balls at left, right, both diagonals and top of newly placed ball 
void check_balls_length(){
    vertical = 1, horizontal_left = 1, horizontal_right = 1, diagonal_left = 1, diagonal_right = 1;
    ver = int_to_alpha(cells[index_cell][2]), h_left = int_to_alpha(cells[index_cell][2]);
    h_right = int_to_alpha(cells[index_cell][2]), d_left = int_to_alpha(cells[index_cell][2]);
    d_right = int_to_alpha(cells[index_cell][2]);
    for(int i = 1; i < 9; i++){
        if(index_cell - (15 * i) >= 0){
            if(cells[index_cell -(15 * i)][3] == 1){
                vertical++;
                ver += int_to_alpha(cells[index_cell -(15 * i)][2]);
            }
            else{
                break;
            }
        }else{
            break;
        }
    }
    int left_diagonal_x = cells[index_cell][0];
    for(int i = 1; i < 9; i++){
        left_diagonal_x -= 60;
        if(index_cell - (15 * i) >= 0 && left_diagonal_x >= 10){
            if(cells[(index_cell -(15 * i)) - i][3] == 1 && cells[(index_cell -(15 * i)) - i][0] == left_diagonal_x){
                diagonal_left++;
                d_left += int_to_alpha(cells[(index_cell -(15 * i)) - i][2]);
            }
            else{
                break;
            }
        }else{
            break;
        }
    }
    int right_diagonal_x = cells[index_cell][0];
    for(int i = 1; i < 9; i++){
        right_diagonal_x += 60;
        if(index_cell - (15 * i) >= 0 && right_diagonal_x <= 850){
            if(cells[(index_cell -(15 * i)) + i][3] == 1 && cells[(index_cell -(15 * i)) + i][0] == right_diagonal_x){
                diagonal_right++;
                d_right += int_to_alpha(cells[(index_cell -(15 * i)) + i][2]);
            }else{
                break;
            }
        }else{
            break;
        }
    }
    int hor_left = cells[index_cell][0];
    for(int i = 1; i < 9; i++){
        hor_left -= 60;
        if(hor_left >= 10){
            if(cells[index_cell - i][3] == 1 && cells[index_cell - i][0] == hor_left){
                horizontal_left++;
                h_left += int_to_alpha(cells[index_cell - i][2]);
            }else{
                break;
            }
        }else{
            break;
        }
    }
    int hor_right = cells[index_cell][0];
    for(int i = 1; i < 9; i++){
        hor_right += 60;
        if(hor_right <= 850){
            if(cells[index_cell + i][3] == 1 && cells[index_cell + i][0] == hor_right){
                horizontal_right++;
                h_right += int_to_alpha(cells[index_cell + i][3]);
            }else{
                break;
            }
        }else{
            break;
        }
    }
    cout << "vertical ball length: " << vertical << " string formed: " << ver << " inverted: " << flipper(vertical, ver) << endl;
    cout << "horizontal right ball length: " << horizontal_right  << " string formed: " << h_right << " inverted: " << flipper(
            horizontal_right, h_right) << endl;
    cout << "horizontal left ball length: " << horizontal_left << " string formed: " << h_left << " inverted: " << flipper(
            horizontal_left, h_left) << endl;
    cout << "diagonal left ball length: " << diagonal_left << " string formed: " << d_left << " inverted: " << flipper(
            diagonal_left, d_left) << endl;
    cout << "diagonal right ball length: " << diagonal_right << " string formed: " << d_right << " inverted: " << flipper(
            diagonal_right, d_right) << endl;
    x_hl = cells[index_cell][0], x_hr = cells[index_cell][0], x_dl = cells[index_cell][0], x_dr = cells[index_cell][0], x_v = cells[index_cell][0];
    y_hl = cells[index_cell][1], y_hr = cells[index_cell][1], y_dl = cells[index_cell][1], y_dr = cells[index_cell][1], y_v = cells[index_cell][1];
    wsec = 0;
}

// this function sets up our 'bank' of alphabet values for our balls
void genRandomAlphas(){
    for(int i = 0; i < 600; i++){
        alphas[i] = GetAlphabet();
    }
}

// this function initializes our game board made up of cells first 2 rows heve x,y coords, integer representing alphabets
// and an exist value which we toggle between 0 & 1 to let our program know if a ball exists in that cell
void gen_cells(){
    int y_pos = height - 90, x_pos, j = 0, index = 0;
    for(int i = 0; i < 9; i++, y_pos -= 60){
        x_pos = 10;
        for(j = 0; j < 15; j++, x_pos += 60, index++){
            cells[index][0] = x_pos;
            cells[index][1] = y_pos;
            if(index < 30){
                cells[index][2] = alphas[index];
                cells[index][3] = 1;
            }
        }
    }
}

// computes angle used for diagonal calculation
void gen_angle(){
    float x, y;
    if(mouse_x < paddle_x){
        x = paddle_x - mouse_x;
        y = mouse_y - paddle_y;
        angle = Rad2Deg(atan(y / x));
        left_side = true;
    }
    else if(mouse_x > paddle_x){
        x = mouse_x - paddle_x;
        y = mouse_y - paddle_y;
        angle = Rad2Deg(atan(y / x));
        left_side = false;
    }
    else{
        angle = 90;
    }
}

// generates values to be used to increment paddle ball
void gen_change(){
    paddle_x_change = (mouse_x - paddle_x) / 17;
    paddle_y_change = (mouse_y - paddle_y) / 17;
}

// check where in the cellspace the paddle ball is performs collision detection after which it places ball in cell if colliding
void place_in_cell(){
    for(int i = 0; i < 135; i++){
        if((paddle_y - cells[i][1] <= 59 && paddle_y - cells[i][1] >= 0) && (paddle_x - cells[i][0] <= 59 && paddle_x - cells[i][0] >= 0) && cells[i][3] != 1){
            cells[i][2] = alphas[paddle_ball];
            index_cell = i;
            break;
        }
    }
    if(index_cell > 14){
        if(cells[index_cell - 15][3] == 1){
            cells[index_cell][3] = 1;
            collide = 5;
        }
    }
    else{
        cells[index_cell][3] = 1;
        collide = 5;
    }
    if(index_cell % 15 != 0){
        if(left_side){
            if(((cells[(index_cell - 15) - 1][3] == 1) && (angle >= 25 && angle <= 75)) || ((cells[index_cell - 1][3] == 1)&& (angle >= 0 && angle <= 65))){
                cells[index_cell][3] = 1;
                collide = 5;
            }
        }
        else{
            if(((cells[(index_cell - 15) + 1][3] == 1) && (angle >= 25 && angle <= 75)) || ((cells[index_cell + 1][3] == 1)&& (angle >= 0 && angle <= 65))){
                cells[index_cell][3] = 1;
                collide = 5;
            }
        }
    }
    else if((index_cell + 1) % 15 == 0){
        if(((cells[(index_cell - 15) - 1][3] == 1) && (angle >= 25 && angle <= 75)) || ((cells[index_cell - 1][3] == 1)&& (angle >= 0 && angle <= 65))){
                cells[index_cell][3] = 1;
                collide = 5;
            }
        }
    else if(index_cell % 15 == 0){
        if(((cells[(index_cell - 15) + 1][3] == 1) && (angle >= 25 && angle <= 75)) || ((cells[index_cell + 1][3] == 1)&& (angle >= 0 && angle <= 65))){
                cells[index_cell][3] = 1;
                collide = 5;
            }
        }
        
    
}

//shortens word if word not found in dictionary
void shorten_word(int &n, string &s){
    string temp = "";
    n--;
    for(int i = 0; i < n; i++){
        temp += s[i];
    }
    s = temp;
}

//checks all directions to see if word forming in dictionary
void check_for_words(){
    word_formed_ver = false, word_formed_rhor = false, word_formed_lhor = false, word_formed_ldiag = false, word_formed_rdiag = false;
    int i;
    string flip;
    while(vertical >= difficulty){
        for(i = 0; i < dictionarysize; i++){
            if(ver == dictionary[i]){
                word_formed_ver = true;
                score += 10;
                cout << "word formed vertical " << ver << ", line " << i + 1 << endl;
                break;
            }
        }
        if(word_formed_ver){
            break;
        }
        else{
            flip = flipper(vertical, ver);
            for(i = 0; i < dictionarysize; i++){
                if(flip == dictionary[i]){
                    word_formed_ver = true;
                    score += 10;
                    cout << "word formed vertical " << flip << ", line " << i + 1 << endl;
                    break;
                }
            }
        }
        if(word_formed_ver) {
            ver = flip;
            break;
        }
        else{
            shorten_word(vertical, ver);
        }
    }
    while(horizontal_left >= difficulty){
        for(i = 0; i < dictionarysize; i++){
            if(h_left == dictionary[i]){
                cout << "word formed left horizontal " << h_left << ", line " << i + 1 << endl;
                word_formed_lhor = true;
                score += 10;
                break;
            }
        }
        if(word_formed_lhor){
            break;
        }
        else{
            flip = flipper(horizontal_left, h_left);
            for(i = 0; i < dictionarysize; i++){
                if(flip == dictionary[i]){
                    cout << "word formed left horizontal " << flip << ", line " << i + 1 << endl;
                    word_formed_lhor = true;
                    score += 10;
                    break;
                }
            }
        }
        if(word_formed_lhor) {
            h_left = flip;
            break;
        }
        else{
            shorten_word(horizontal_left, h_left);
        }
    }
    while(horizontal_right >= difficulty){
        for(i = 0; i < dictionarysize; i++){
            if(h_right == dictionary[i]){
                cout << "word formed right horizontal " << h_right << ", line " << i + 1 << endl;
                word_formed_rhor = true;
                score += 10;
                break;
            }
        }
        if(word_formed_rhor){
            break;
        }
        else{
            flip = flipper(horizontal_right, h_right);
            for(i = 0; i < dictionarysize; i++){
                if(flip == dictionary[i]){
                    cout << "word formed right horizontal " << flip << ", line " << i + 1 << endl;
                    score += 10;
                    word_formed_rhor = true;
                    break;
                }
            }
        }
        if(word_formed_rhor) {
            h_right = flip;
            break;
        }
        else{
            shorten_word(horizontal_right, h_right);
        }
    }
    while(diagonal_left >= difficulty){
        for(i = 0; i < dictionarysize; i++){
            if(d_left == dictionary[i]){
                cout << "word formed left diagonal " << d_left << ", line " << i + 1 << endl;
                score += 10;
                word_formed_ldiag = true;
                break;
            }
        }
        if(word_formed_ldiag){
            break;
        }
        else{
            flip = flipper(diagonal_left, d_left);
            for(i = 0; i < dictionarysize; i++){
                if(flip == dictionary[i]){
                    cout << "word formed left diagonal " << flip << ", line " << i + 1 << endl;
                    score += 10;
                    word_formed_ldiag = true;
                    break;
                }
            }
        }
        if(word_formed_ldiag) {
            d_left = flip;
            break;
        }
        else{
            shorten_word(diagonal_left, d_left);
        }
    }
    while(diagonal_right >= difficulty){
        for(i = 0; i < dictionarysize; i++){
            if(d_right == dictionary[i]){
                cout << "word formed right diagonal " << d_right << ", line " << i + 1 << endl;
                word_formed_rdiag = true;
                score += 10;
                break;
            }
        }
        if(word_formed_rdiag){
            break;
        }
        else{
            flip = flipper(diagonal_right, d_right);
            for(i = 0; i < dictionarysize; i++){
                if(flip == dictionary[i]){
                    cout << "word formed right diagonal " << flip << ", line " << i + 1 << endl;
                    word_formed_rdiag = true;
                    score += 10;
                    break;
                }
            }
        }
        if(word_formed_rdiag) {
            d_right = flip;
            break;
        }
        else{
            shorten_word(diagonal_right, d_right);
        }
    }

}

// bursts word
void popper(){
    if(word_formed_ver){
        cells[index_cell][3] = 0;
        for(int i = 1; i < vertical; i++){
            cells[index_cell -(15 * i)][3] = 0;
        }
    }
    if(word_formed_lhor){
        cells[index_cell][3] = 0;
        for(int i = 1; i < horizontal_left; i++){
            cells[index_cell - i][3] = 0;
        }
    }
    if(word_formed_rhor){
        cells[index_cell][3] = 0;
        for(int i = 1; i < horizontal_right; i++){
            cells[index_cell + i][3] = 0;
        }
    }
    if(word_formed_ldiag){
        cells[index_cell][3] = 0;
        for(int i = 1; i < diagonal_left; i++){
            cells[(index_cell -(15 * i)) - i][3] = 0;
        }
    }
    if(word_formed_rdiag){
        cells[index_cell][3] = 0;
        for(int i = 1; i < diagonal_right; i++){
            cells[(index_cell -(15 * i)) + i][3] = 0;
        }
    }
}

// drops letter which are not connected to anything else
void dropper(){
    for(int i = 15; i < 135; i++){
        if(i % 15 == 0){
            if(cells[i - 15][3] == 0 && cells[(i - 15) + 1][3] == 0 && cells[i + 1][3] == 0){
                cells[i][3] = 0;
            }
        }
        else if((i + 1) % 15 == 0){
            if(cells[i - 15][3] == 0 && cells[(i - 15) - 1][3] == 0 && cells[i - 1][3] == 0){
                cells[i][3] = 0;
            }
        }
        else{
            if(cells[i - 15][3] == 0 && cells[(i - 15) + 1][3] == 0 && cells[i + 1][3] == 0 && cells[(i - 15) - 1][3] == 0 && cells[i - 1][3] == 0){
                cells[i][3] = 0;
            }
        }
    }
}

//checks if board empty for victory condition
void check_board_empty(){
    bool empty = true;
    for(int i = 0; i < 135; i++){
        if(cells[i][3] == 1){
            empty = false;
            break;
        }
    }
    if(empty){
        cleared_board = true;
        game_end = true;
    }
}

bool check_on_start = true; // ensures following function only runs once
string start_words[10]; // stores start words
int start_word_coords[10][2]; // stores their co-ordinates
// checks board for words on start
void check_board_on_start(){
    for(int i = 0; i < 10; i++){
        start_words[i] = "_";
    }
    int j, k, i, wordlen, start_word_i = 0;
    string word;
    bool wordf;
    for(j = 0; j < 27; j++){
        if(j < 7){
            wordlen = 9;
            wordf = false;
            while(wordlen >= 3){
                word = "";
                for(k = 0; k < wordlen; k++){
                    word += int_to_alpha(cells[j + k][2]);
                }
                for(i = 0; i < dictionarysize; i++){
                    if(word == dictionary[i]){
                        wordf = true;
                        break;
                    }
                }
                if(wordf){
                    cout << word << endl;
                    start_words[start_word_i] = word;
                    cout << j << endl;
                    i = j;
                    start_word_coords[start_word_i][0] = cells[i][0];
                    start_word_coords[start_word_i][1] = cells[i][1];
                    j += wordlen;
                    cout << j << endl;
                    for(; i < j; i++){
                        cells[i][3] = 0;
                    }
                    start_word_i++;
                    break;
                }
                wordlen--;
            }

        }
        else if( j == 13){
            j = 15;
        }
        else{
            wordlen = 15 - j;
            wordf = false;
            while(wordlen >= 3){
                word = "";
                for(k = 0; k < wordlen; k++){
                    word += int_to_alpha(cells[j + k][2]);
                }
                for(i = 0; i < dictionarysize; i++){
                    if(word == dictionary[i]){
                        wordf = true;
                        break;
                    }
                }
                if(wordf){
                    cout << word << endl;
                    start_words[start_word_i] = word;
                    cout << j << endl;
                    i = j;
                    start_word_coords[start_word_i][0] = cells[i][0];
                    start_word_coords[start_word_i][1] = cells[i][1];
                    j += wordlen;
                    cout << j << endl;
                    for(; i < j; i++){
                        cells[i][3] = 0;
                    }
                    start_word_i++;
                    break;
                }
                wordlen--;
            }
        }
    }


}

// displays word popped from cellspace using drawstring
void display_word(int &x, int &y, string s, string k){

    int delta_x = 0, delta_y = 0;
    if(k == "left"){
        delta_x = -10;
        delta_y = 0;
    }
    else if(k == "right"){
        delta_x = 10;
        delta_y = 0;
    }
    else if(k == "up"){
        delta_x = 0;
        delta_y = 10;
    }
    else if(k == "d right"){
        delta_x = 10;
        delta_y = 10;
    }
    else if(k == "d left"){
        delta_x = -10;
        delta_y = 10;
    }
    DrawString(x, y, width, height, s, colors[BLUE_VIOLET]);
    x += delta_x;
    y += delta_y;
}

// handles movement
void move(){
    paddle_x += paddle_x_change;
    paddle_y += paddle_y_change;
    if(paddle_x >= width - 60|| paddle_x <= 10){
        paddle_x_change *= -1;
        left_side = !left_side;
        if(left_side){
            cout << " moving towards left side" << endl;
        }
        else{
            cout << " moving towards right side" << endl;
        }
    }
    if(collide == 5 || paddle_y >= height){
        cout << "Impact! x coordinate: " << cells[index_cell][0] << ", y coordinate: " << cells[index_cell][1] << endl;
        cout << "paddle y: " << paddle_y << endl;
        if(paddle_y <= 130){
            fill_coll = true;
            game_end = true;
        }
        collide = 10;
        check_balls_length();
        check_for_words();
        popper();
        dropper();
        check_board_empty();
        paddle_x_change = 0;
        paddle_y_change = 0;
        paddle_ball += 1;
        paddle_x = width / 2;
        paddle_y = 10;
        click = false;
    }
    glutPostRedisplay();
}

int fader = 0; //fades 'press enter' colour
int start_word_displayer = 0; // displays words popped at beginning
/*
* Main Canvas drawing function.
* */
void DisplayFunction() {
	// set the background color using function glClearColor.
	// to change the background play with the red, green and blue values below.
	// Note that r, g and b values must be in the range [0,1] where 0 means dim red and 1 means pure red and so on.
	//#if 0
	glClearColor(1/*Red Component*/, 1.0/*Green Component*/,
		1.0/*Blue Component*/, 0 /*Alpha component*/); // Red==Green==Blue==1 --> White Colour
	glClear(GL_COLOR_BUFFER_BIT); //Update the colors

	//write your drawing commands here or call your drawing functions...
	for(int i = 0; i < 135; i++){
        if(cells[i][3] == 1){
            DrawAlphabet((alphabets)cells[i][2], cells[i][0], cells[i][1], awidth, aheight);
        }
    }

    DrawAlphabet((alphabets)alphas[paddle_ball], paddle_x, paddle_y, awidth, aheight);
    DrawAlphabet((alphabets)alphas[paddle_ball + 1], 790, 10, awidth, aheight);
    DrawString(780, 90, width, height + 5, "NEXT BALL", colors[BLUE_VIOLET]);

    if(game_start && check_on_start && countdown < 120){
        check_board_on_start();
        dropper();
        check_on_start = false;
        for(int i = 0; i < 10; i++){
            if(start_words[i] != "_"){
                cout << start_words[i] << " " << start_word_coords[i][0] << " " << start_word_coords[i][1] << " ";
            }
        }
        cout << endl;
    }
    if(start_word_displayer <= 30){
        for(int i = 0; i < 10; i++){
            if(start_words[i] != "_"){
                DrawString(start_word_coords[i][0] + 90, start_word_coords[i][1], width + 5, height, start_words[i], colors[BLUE_VIOLET]);
                start_word_coords[i][1] -= 10;
            }
        }
    }
    start_word_displayer++;

    if(click){
        move();
        if(paddle_y > 100){
            place_in_cell();
        }

    }
    if(wsec < 21 && word_formed_ver){
        display_word(x_v, y_v, ver, "up");
    }
    if(wsec < 21 && word_formed_lhor){
        display_word(x_hl, y_hl, h_left, "left");
    }
    if(wsec < 21 && word_formed_rhor){
        display_word(x_hr, y_hr, h_right, "right");
    }
    if(wsec < 21 && word_formed_ldiag){
        display_word(x_dr, y_dr, d_left, "d left");
    }
    if(wsec < 21 && word_formed_rdiag){
        display_word(x_dl, y_dl, d_right, "d right");
    }
    wsec++;

    

	DrawString(40, height - 20, width, height, "Score " + Num2Str(score), colors[BLUE_VIOLET]);
	DrawString(width / 2 - 30, height - 25, width, height, "Time Left:" + Num2Str(countdown) + " secs", colors[RED]);
    if(!game_start){
        fader++;
        DrawCircle(0, 0 , 2, colors[WHITE]);
        DrawAlphabet((alphabets)22, 140, height / 2, awidth, aheight);
        DrawAlphabet((alphabets)14, 200, height / 2, awidth, aheight);
        DrawAlphabet((alphabets)17, 260, height / 2, awidth, aheight);
        DrawAlphabet((alphabets)3, 320, height / 2, awidth, aheight);
        DrawAlphabet((alphabets)18, 440, height / 2, awidth, aheight);
        DrawAlphabet((alphabets)7, 500, height / 2, awidth, aheight);
        DrawAlphabet((alphabets)14, 560, height / 2, awidth, aheight);
        DrawAlphabet((alphabets)14, 620, height / 2, awidth, aheight);
        DrawAlphabet((alphabets)19, 680, height / 2, awidth, aheight);
        DrawAlphabet((alphabets)4, 740, height / 2, awidth, aheight);
        DrawAlphabet((alphabets)17, 800, height / 2, awidth, aheight);
        if(fader < 8){
            DrawString(width / 2 - 75, (height / 2) - 50, width, height,
                       "press enter to begin.", colors[BLACK]);
        }
        else if (fader < 10){
            DrawString(width / 2 - 75, (height / 2) - 50, width, height,
                       "press enter to begin.", colors[WHITE]);
        }
        else{
            fader = 0;
        }
        DrawString(width / 2 - 50, (height / 2) - 100, width, height,
                   "CONTROLS:", colors[BLACK]);
        DrawString(width / 2 - 95, (height / 2) - 150, width, height,
                   "point & click to shoot", colors[BLACK]);
        DrawString(width / 2 - 75, (height / 2) - 200, width, height,
                   "escape key to exit", colors[BLACK]);
        DrawString(185, 50, width, height,
                   "1st semester project by M. Bilal Zubairi - i170194 - CS(B)", colors[BLACK]);
        DrawString(width / 2 - 100, height - 100, width, height, "CHOOSE DIFFICULTY", colors[BLACK]);
        if(difficulty == 2){
        	DrawString(width / 2 - 165, height - 150, width, height, "NORMAL-min w len 3(press 2)", colors[BLACK]);
        	DrawString(10, height - 150, width, height, "EASY-min w len 2(press 1)", colors[RED]);
        	DrawString(width / 2 + 170, height - 150, width, height, "HARD-min w len 4(press 3)", colors[BLACK]);
		}
		else if(difficulty == 3){
        	DrawString(width / 2 - 165, height - 150, width, height, "NORMAL-min w len 3(press 2)", colors[RED]);
        	DrawString(10, height - 150, width, height, "EASY-min w len 2(press 1)", colors[BLACK]);
        	DrawString(width / 2 + 170, height - 150, width, height, "HARD-min w len 4(press 3)", colors[BLACK]);
		}
		else if(difficulty == 4){
        	DrawString(width / 2 - 165, height - 150, width, height, "NORMAL-min w len 3(press 2)", colors[BLACK]);
        	DrawString(10, height - 150, width, height, "EASY-min w len 2(press 1)", colors[BLACK]);
        	DrawString(width / 2 + 170, height - 150, width, height, "HARD-min w len 4(press 3)", colors[RED]);
		}
    }
    if(countdown == 0){
        time_up = true;
        game_end = true;
    }
    if(game_end){
        DrawCircle(0, 0 , 2, colors[WHITE]);
        DrawAlphabet((alphabets)6, 140, height / 2, awidth, aheight);
        DrawAlphabet((alphabets)0, 200, height / 2, awidth, aheight);
        DrawAlphabet((alphabets)12, 260, height / 2, awidth, aheight);
        DrawAlphabet((alphabets)4, 320, height / 2, awidth, aheight);
        DrawAlphabet((alphabets)14, 440, height / 2, awidth, aheight);
        DrawAlphabet((alphabets)21, 500, height / 2, awidth, aheight);
        DrawAlphabet((alphabets)4, 560, height / 2, awidth, aheight);
        DrawAlphabet((alphabets)17, 620, height / 2, awidth, aheight);
        DrawString(360, (height / 2) - 100, width, height, "Score " + Num2Str(score), colors[BLUE_VIOLET]);
        if(time_up){
        DrawString(360, (height / 2) - 200, width, height, "You ran out of time!", colors[BLUE_VIOLET]);
        }
        else if(cleared_board){
            DrawString(280, (height / 2) - 200, width, height, "Congratulations! you cleared the board!", colors[BLUE_VIOLET]);
        }
        else{
            DrawString(280, (height / 2) - 200, width, height, "better luck next time! you filled a column.", colors[BLUE_VIOLET]);
        }
    }

	// #----------------- Write your code till here ----------------------------#
	//DO NOT MODIFY THESE LINES
	DrawShooter((width / 2) - 35, 0, bwidth, bheight);
	glutSwapBuffers();
	//DO NOT MODIFY THESE LINES..
}

/* Function sets canvas size (drawing area) in pixels...
*  that is what dimensions (x and y) your game will have
*  Note that the bottom-left coordinate has value (0,0) and top-right coordinate has value (width-1,height-1)
* */
void SetCanvasSize(int width, int height) {
	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1); // set the screen size to given width and height.*/
}

/*This function is called (automatically) whenever any non-printable key (such as up-arrow, down-arraw)
* is pressed from the keyboard
*
* You will have to add the necessary code here when the arrow keys are pressed or any other key is pressed...
*
* This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
* program coordinates of mouse pointer when key was pressed.
*
* */

void NonPrintableKeys(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT /*GLUT_KEY_LEFT is constant and contains ASCII for left arrow key*/) {
		// what to do when left key is pressed...

	}
	else if (key == GLUT_KEY_RIGHT /*GLUT_KEY_RIGHT is constant and contains ASCII for right arrow key*/) {

	}
	else if (key == GLUT_KEY_UP/*GLUT_KEY_UP is constant and contains ASCII for up arrow key*/) {
	}
	else if (key == GLUT_KEY_DOWN/*GLUT_KEY_DOWN is constant and contains ASCII for down arrow key*/) {
	}

	/* This function calls the Display function to redo the drawing. Whenever you need to redraw just call
	* this function*/
	/*
	glutPostRedisplay();
	*/
}
/*This function is called (automatically) whenever your mouse moves witin inside the game window
*
* You will have to add the necessary code here for finding the direction of shooting
*
* This function has two arguments: x & y that tells the coordinate of current position of move mouse
*
* */

void MouseMoved(int x, int y) {
	//If mouse pressed then check than swap the balls and if after swaping balls dont brust then reswap the balls

}

/*This function is called (automatically) whenever your mouse button is clicked witin inside the game window
*
* You will have to add the necessary code here for shooting, etc.
*
* This function has four arguments: button (Left, Middle or Right), state (button is pressed or released),
* x & y that tells the coordinate of current position of move mouse
*
* */

void MouseClicked(int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON) // dealing only with left button
	{
		if (state == GLUT_UP)
		{

            if(!click && game_start && !game_end){
                mouse_x = x;
                mouse_y = height - y;
                cout << "mouse x: " << mouse_x << ", mouse y: " << mouse_y << endl;
                click = true;
                gen_angle();
                cout << "angle: " << angle;
                if(left_side){
                    cout << " moving towards left side" << endl;
                }
                else{
                    cout << " moving towards right side" << endl;
                }
                gen_change();
                cout << "change in x: " << paddle_x_change << ", change in y: " << paddle_y_change << endl;
            }
		}
	}
	else if (button == GLUT_RIGHT_BUTTON) // dealing with right button
	{

	}
	glutPostRedisplay();
}
/*This function is called (automatically) whenever any printable key (such as x,b, enter, etc.)
* is pressed from the keyboard
* This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
* program coordinates of mouse pointer when key was pressed.
* */
void PrintableKeys(unsigned char key, int x, int y) {
	if (key == KEY_ESC/* Escape key ASCII*/) {
		exit(1); // exit the program when escape key is pressed.
	}
    if (key == KEY_ENTER/* Escape key ASCII*/) {
        game_start = true; // starts game.
    }
    if(!game_start){
    	if(key == KEY_ONE){
    		difficulty = 2;
    	}
    	if(key == KEY_TWO){
    		difficulty = 3;
    	}
    	if(key == KEY_THREE){
    		difficulty = 4;
    	}
    }
}

/*
* This function is called after every 1000.0/FPS milliseconds
* (FPS is defined on in the beginning).
* You can use this function to animate objects and control the
* speed of different moving objects by varying the constant FPS.
*
* */
void Timer(int m) {
	if(game_start){
        msec++;
        if(msec % 10 == 0 && msec != 0){
            countdown--;
        }
    }

	glutPostRedisplay();
	glutTimerFunc(1000.0/FPS, Timer, 0);
}

/*
* our gateway main function
* */
int main(int argc, char*argv[]) {
	InitRandomizer(); // seed the random number generator...

	//Dictionary for matching the words. It contains the 370099 words.
	dictionary = new string[dictionarysize]; 
	ReadWords("words_alpha.txt", dictionary); // dictionary is an array of strings
	//print first 5 words from the dictionary
	for(int i=0; i < 5; ++i)
		cout<< " word "<< i << " =" << dictionary[i] <<endl;

	//Write your code here for filling the canvas with different Alphabets. You can use the Getalphabet function for getting the random alphabets
    genRandomAlphas();
    gen_cells();

	glutInit(&argc, argv); // initialize the graphics library...
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // we will be using color display mode
	glutInitWindowPosition(50, 50); // set the initial position of our window
	glutInitWindowSize(width, height); // set the size of our window
	glutCreateWindow("i170194's ITC Word Shooter"); // set the title of our game window
	//SetCanvasSize(width, height); // set the number of pixels...

	// Register your functions to the library,
	// you are telling the library names of function to call for different tasks.
	RegisterTextures();
	glutDisplayFunc(DisplayFunction); // tell library which function to call for drawing Canvas.
	glutSpecialFunc(NonPrintableKeys); // tell library which function to call for non-printable ASCII characters
	glutKeyboardFunc(PrintableKeys); // tell library which function to call for printable ASCII characters
	glutMouseFunc(MouseClicked);
	glutPassiveMotionFunc(MouseMoved); // Mouse

	//// This function tells the library to call our Timer function after 1000.0/FPS milliseconds...
	glutTimerFunc(1000.0/FPS, Timer, 0);

	//// now handle the control to library and it will call our registered functions when
	//// it deems necessary...

	glutMainLoop();

	return 1;
}
#endif /* */

#include <Windows.h>
#include <iostream>
#include <thread>
#include <math.h>

#include "SerialPort.h"
#include "OpenGL.h"

using namespace std;

OpenGL * Graphics;
bool GraphicsSetFlag = false;
bool CalibreSetFlag = false;
point cursure;


void SetGraphics(int argc, char *argv[]){
	cursure.x = cursure.y = cursure.z = 0.0;
	Graphics = new OpenGL(argc, argv);
}

#define h 22.0
#define w 38.0
#define D (double)

double d1, d2, d3;
double d1a = 1 << 30, d2a = 1 << 30, d3a = 1 << 30;

double R(double data){
	return (10230.0 / (data + 0.001) - 10);
}
double distance(double r){
	return pow(((r + 0.0593) / 0.0016) + (0.0995 / 0.0032) * (0.0995 / 0.0032), 0.5) - 0.0995 / 0.0032;
}

double X2(double y, double z){
	return -h*h + 2.0 * h*y - y*y + d1*pow((d1*pow(z, 0.5)), 1.0 / 3.0)*pow(z, 0.5) - z*z;
}
double X(double y, double z){
	return pow(X2(y, z), 0.5);
}
double Y(double z){
	return (h*h - d1*pow((d1*pow(z, 0.5)), 1.0 / 3.0)*pow(z, 0.5) + d2*pow((d2*pow(z, 0.5)), 1.0 / 3.0)*pow(z, 0.5)) / (2.0 * h);
}
double F(double z){
	double x, y;
	y = Y(z);
	x = X(y, z);
	return (pow((x - w)*(x - w) + y*y + z + z, 3.0 / 4.0) / pow(z, 0.5));
}
double nearestZ(){
	double nearest, difference, min_difference = d3;
	for (double z = 20.0; z < 90.0; z += 0.1){
		difference = abs(F(z) - d3);
		if (difference < min_difference){
			min_difference = difference;
			nearest = z;
		}
	}
	return nearest;
}

double XS(){
	return (d2*d2 - d3*d3 + w*w) / (2.0*w);
}
double YS(){
	return (d2*d2 - d1*d1 + h*h) / (2.0*h);
}
double ZS(double x, double y){
	return pow(abs(d2*d2 - x*x - y*y), 0.5);
}

double XI(){
	double d, p, q;
	d = d1; p = d2; q = d3;
	return (d*p*p*q*q) / (d*d*p*p + d*d*q*q + p*p*q*q);
}
double YI(){
	double d, p, q;
	d = d1; p = d2; q = d3;
	return (d*d*p*q*q) / (d*d*p*p + d*d*q*q + p*p*q*q);
}
double ZI(){
	double d, p, q;
	d = d1; p = d2; q = d3;
	return (d*d*p*p*q) / (d*d*p*p + d*d*q*q + p*p*q*q);
}

double XIF(){
	return d1 / pow((1 + pow((d1 / d2), 4.0) + pow((d1 / d3), 4.0)), (3.0 / 4.0));
}
double YIF(double x_amount){
	return x_amount*pow((d1 / d2), 2.0);
}
double ZIF(double x_amount){
	return x_amount*pow((d1 / d3), 2.0);
}

int counter = 100;
point possition(int *sensor_data){
	point p;
	d1 = distance(R(sensor_data[0]));
	d2 = distance(R(sensor_data[1]));
	d3 = distance(R(sensor_data[2]));

	d1 = (d1*d1a) / pow(d1a*d1a - d1*d1, 0.5);
	d2 = (d2*d2a) / pow(d2a*d2a - d2*d2, 0.5);
	d3 = (d3*d3a) / pow(d3a*d3a - d3*d3, 0.5);

	p.x = XIF();
	p.y = YIF(p.x);
	p.z = ZIF(p.x);

	if (counter-- == 0){
		cout << p.x << ", " << p.y << ", " << p.z << endl;
		cout << d1 << ", " << d2 << ", " << d3 << endl;
		counter = 100;
	}
	return p;
}

void GetPoint(){
	int sensor[3];
	bool button_state;
	point cut;
	cut.x = -1;
	cut.y = -1;
	cut.z = -1;
	SerialPort DataPort;
	unsigned char start_command = 's';
	DataPort.Write(start_command);
	cout << "Connected!" << endl;
	Sleep(2);
	unsigned char data[4];
	int i, j = 0;
	while (1){
		for (i = 0; i < 4; i++){
			data[i] = DataPort.Read();
		}
		for (i = 0; i < 3; i++){
			sensor[i] = data[i] + ((data[3] >> (2 * i)) % 4) * 256;
		}
		button_state = (data[3] >> 6) % 2;
		if (CalibreSetFlag){
			if (j == 0){
				d1a = d2a = d3a = 0.0;
			}
			d1a += distance(R(sensor[0]));
			d2a += distance(R(sensor[1]));
			d3a += distance(R(sensor[2]));
			if (++j == 100){
				CalibreSetFlag = false;
				j = 0;
				d1a /= 100.0;
				d2a /= 100.0;
				d3a /= 100.0;
				cout << "Calibrated!" << endl;
				cout << d1a << ", " << d2a << ", " << d3a << endl;
			}
		}
		else{
			cursure = possition(sensor);
			if (button_state){
				Graphics->DrawPoint(cut);
			}
			else{
				Graphics->DrawPoint(cursure);
			}
		}
	}
}

int main(int argc, char *argv[]) {
	thread GraphicsHandle(SetGraphics, argc, argv);

	// wait for graphic setup
	while (!GraphicsSetFlag);
	
	thread Scanner(GetPoint);

	GraphicsHandle.join();
	Scanner.join();
}
#include<iostream>
#include<cmath>

using namespace std;

double Radius(double x, double y);
double Theta (double x, double y);
class Cartesian;
class Polar;

class Polar {
    public:
        double a;
        double t;

        Polar(float newa, float newt) { 
            a = newa;
            t = newt;
        }

        Cartesian toCartesian();
};


class Cartesian {
    public: 
        double x;
        double y; 
        
        Cartesian(float newx, float newy) { 
            x = newx;
            y = newy;
        } 

        Polar toPolar();
}; 

Cartesian Polar::toCartesian() {
            return Cartesian(a*cos(t),a*sin(t));
}

Polar Cartesian::toPolar() {
    return Polar(Radius(x,y), Theta(x,y));
}



double Theta(double x, double y){
	double Thta;
	Thta = atan(y/x);
	return Thta;
}


double Radius(double x, double y){
	double rad;
	rad = sqrt((pow(x,2))+(pow(y,2)));
	return rad;
}	


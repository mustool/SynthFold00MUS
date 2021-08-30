/*

Copyright MUS 2020

This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.


*/

#ifndef PANNER_H_INCLUDED
#define PANNER_H_INCLUDED

#define PI       3.14159265358979323846
#define DEG_TO_RAD (PI/180.0)


//#include "panner.cpp"

//float ofDegToRad(float degrees);
float ofDegToRad(float degrees) {
    return degrees * DEG_TO_RAD;
}

struct StereoFrame{
    float leftSample;
    float rightSample;
};

//-------------------------------------------------------------------

class Panner{
public:
    StereoFrame pan(float input, float position){
        StereoFrame currentFrame;
        if(position > 1.0){
            position = 1.0;
        };
        if(position < -1.0){
            position = -1.0;
        };

        float theta = position * 45;
        theta = ofDegToRad(theta);

        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        currentFrame.leftSample = input * constant * (sinTheta - cosTheta);
        currentFrame.rightSample = input * constant * (sinTheta + cosTheta);


        return currentFrame;
    }
private:
    float constant = (sqrt(2)/(float)2);
};

#endif

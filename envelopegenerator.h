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


#ifndef ENVELOPEGENERATOR_H_INCLUDED
#define ENVELOPEGENERATOR_H_INCLUDED

#include <stdio.h>
//#include "ofMain.h"


class EnvelopeGenerator
{
public:
    EnvelopeGenerator(float initAttack = 200, float initDecay = 40, float initSustain = 0.7, float initRelease = 800, float SR = 44100){
        attackTime = initAttack;
        decayTime = initDecay;
        sustainLevel = initSustain;
        releaseTime = initRelease;

        sampleRate = SR;

        calculateInc(ATTACK);
        calculateInc(DECAY);
        calculateInc(RELEASE);

        currentValue = 0.0;

        state = OFF;
        gate = 0;

    }

    void process(){

      switch (state) {
      case OFF:
          currentValue = 0.0;
          break;
      case ATTACK:
          currentValue += attackInc;
          if(currentValue >= 1.0){
              currentValue = 1.0;
              state = DECAY;
          }
          break;
      case DECAY:
          currentValue -= decayInc;
          if(currentValue <= sustainLevel){
              currentValue = sustainLevel;
              state = SUSTAIN;
          }
          break;
      case SUSTAIN:
          currentValue = sustainLevel;
          break;
      case RELEASE:
          currentValue -= releaseInc;
          if(currentValue <= 0.0){
              currentValue = 0.0;
              state = OFF;
          }
          break;
      //default:

          //cout << "no such state, EG" << endl;
          //break;
      }

    };

    float getValue(){
        return currentValue;
    }

    void setGate(int newGate){
        int prevGate = gate;
        gate = newGate;
        if(prevGate == 0 && gate == 1){state = ATTACK;}
        if(prevGate == 1 && gate == 0){state = RELEASE;}
    }

    int getState(){
        return state;
    }

    void setAttack(float newAttack){
        attackTime = newAttack;
        calculateInc(ATTACK);
    }

    void setDecay(float newDecay){
        decayTime = newDecay;
        calculateInc(DECAY);
    }

    void setSustain(float newSustain){
        sustainLevel = newSustain;
    }

    void setRelease(float newRelease){
        releaseTime = newRelease;
        calculateInc(RELEASE);
    }

private:
    float attackTime, decayTime, sustainLevel, releaseTime;
    double attackInc, decayInc, releaseInc;
    float currentValue;
    int gate;
    double sampleRate;

    int state;
    enum states{
        OFF,
        ATTACK,
        DECAY,
        SUSTAIN,
        RELEASE
    };

    void calculateInc(int whichState){
        switch(whichState){
        case ATTACK:
            attackInc = 1/(sampleRate * (attackTime * 0.001));
            break;
        case DECAY:
            decayInc = (1 - sustainLevel)/(sampleRate * (decayTime * 0.001));
            break;
        case RELEASE:
            releaseInc = sustainLevel/(sampleRate * (releaseTime * 0.001));
            break;
        }
    }
};

#endif // ENVELOPEGENERATOR_H

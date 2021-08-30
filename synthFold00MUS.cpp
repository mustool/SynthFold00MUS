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


#include <lv2synth.hpp>
#include <cstdlib>
#include "envelopegenerator.h"
#include <lv2lib00_MUS/mainMUS.h>
#include "Panner.h"


//// detta är det första försöket jag gör, att skriva en synt
//// MED envelope

class SynthFold00MUSVoice : public LV2::Voice {

public:


  SynthFold00MUSVoice(double rate)
    : m_key(LV2::INVALID_KEY), m_rate(rate), m_period(10), m_counter(0) {


      iv = 1.0 / 128.0;

      env = new EnvelopeGenerator(10, 40, 0.7, 800, rate);
      main_drive_env = new EnvelopeGenerator(10, 40, 0.7, 800, rate);


      for( int i = 0; i < 4; ++i){

        osc[i] = new WaveTableSine00(rate);
        fmult[i] = 1.0;
        folder[i] = new Folder00();
        folder[i]->setClip(4.0);
        smoothdrive[i] = new Smooth();
        smoothdrive[i]->setSmooth(0.999);
        smoothpanner[i] = new Smooth();
        smoothpanner[i]->setSmooth(0.999);
        smoothmuls[i] = new Smooth();
        smoothmuls[i]->setSmooth(0.999);
        /*smoothpanrate[i] = new Smooth();
        smoothpanrate[i]->setSmooth(0.999);
        smoothpanamt[i] = new Smooth();
        smoothpanamt[i]->setSmooth(0.999);*/

        panlfo[i] = new WaveTableSine00(rate);
        drivelfo[i] = new WaveTableSine00(rate);
      }


      gate = 0;

      smoothamp = new Smooth();

      smoothamp->setSmooth(0.999);

      crv = 1.0;

      freq = 0.0;


      smoothpregain = new Smooth();
      smoothpregain->setSmooth(0.999);
      smoothvol = new Smooth();
      smoothvol->setSmooth(0.999);

      smooth_drive_amt = new Smooth();
      smooth_drive_amt->setSmooth(0.999);



  }


  void on(unsigned char key, unsigned char velocity) {




    m_key = key;

    freq = mtof((float)m_key);
    //printf("%f\n", fmult[0]);



    mul = velocity * iv;

    //osc->setMul(velocity * iv);

    gate = 1;

  }


  void off(unsigned char velocity) {

    gate = 0;
    m_key = LV2::INVALID_KEY;

  }


  unsigned char get_key() const {
    return m_key;
  }


  void render(uint32_t from, uint32_t to) {

    float atk, dky, sus, rel;

    atk = *p(3);
    dky = *p(4);
    sus = *p(5);
    sus = sus < 0 ? 0 : sus;
    sus = sus > 1 ? 1 : sus;
    rel = *p(6);

    crv = *p(7);
    crv = crv < 1 ? 1 : crv;
    crv = crv > 5 ? 5 : crv;


    env->setAttack(atk);
    env->setDecay(dky);
    env->setSustain(sus);
    env->setRelease(rel);

    float drive_atk, drive_dky, drive_sus, drive_rel, drive_amt;

    drive_atk = *p(52);
    drive_dky = *p(53);
    drive_sus = *p(54);
    drive_sus = drive_sus < 0 ? 0 : drive_sus;
    drive_sus = drive_sus > 1 ? 1 : drive_sus;
    drive_rel = *p(55);
    drive_amt = *p(56);




    main_drive_env->setAttack(drive_atk);
    main_drive_env->setDecay(drive_dky);
    main_drive_env->setSustain(drive_sus);
    main_drive_env->setRelease(drive_rel);

    float drive[4];
    float localpan[4];
    float localmul[4];
    float ct[4];
    float ft[4];
    float mastercoarse;
    float masterfine;

    float panlforate[4];
    float panlfoamt[4];

    mastercoarse = midiratio(trunc(*p(34)));
    masterfine = midiratio(*p(35));

    float drivelforate[4];
    float drivelfoamt[4];

    for(int i = 0; i < 4; ++i){

      fmult[i] = floor(*p(i + 8));
      //fmult[i] = floor(fmult[i];
      fmult[i] = fmult[i] < 1 ? 1 : fmult[i];
      fmult[i] = fmult[i] > 15 ? 15 : fmult[i];

      drive[i] = *p(i + 12);

      localpan[i] = *p(i + 16);
      localpan[i] = localpan[i] < -1 ? -1 : localpan[i];
      localpan[i] = localpan[i] > 1 ? 1 : localpan[i];

      localmul[i] = *p(i + 20);
      localmul[i] = localmul[i] < 0 ? 0 : localmul[i];
      localmul[i] = localmul[i] > 1 ? 1 : localmul[i];

      ct[i] = midiratio(trunc(*p(i + 26)));

      ft[i] = midiratio(*p(i + 30));

      panlforate[i] = *p(i + 36);
      panlfoamt[i] = *p(i + 40);

      drivelforate[i] = *p(i + 44);
      drivelfoamt[i] = *p(i + 48);



    }

    float pregain = *p(24);
    pregain = pregain < -24 ? -24 : pregain;
    pregain = pregain > 24 ? 24 : pregain;

    float mainvol = *p(25);
    mainvol = mainvol < -24 ? -24 : mainvol;
    mainvol = mainvol > 0 ? 0 : mainvol;


    int off = 0;
    if (m_key == LV2::INVALID_KEY) off = 1;


    for (uint32_t i = from; i < to; ++i) {


      env->setGate(gate);

      main_drive_env->setGate(gate);

      env->process();
      main_drive_env->process();

      //osc->setMul(smoothamp->tick(mul));

      //float s = osc->ar();
      envVal = env->getValue();
      float main_drive_env_val = main_drive_env->getValue();

      main_drive_env_val *= drive_amt;
      main_drive_env_val = (0.0 - drive_amt) + main_drive_env_val;
      main_drive_env_val = dbamp(smooth_drive_amt->tick(main_drive_env_val));

      // sätt kurvan
      envVal = pow(envVal, crv);
      // denna är ju alltid frågan om man vill ha med...

      // oscillator
      float left = 0.0;
      float right = 0.0;

      //osc[0]->setFreq(freq);

      float pre_gain = smoothpregain->tick(dbamp(pregain));
      float main_vol = smoothvol->tick(dbamp(mainvol));

      for(int j = 0; j < 4; ++j){

        osc[j]->setFreq(freq * fmult[j] * ct[j] * ft[j]
           * mastercoarse * masterfine);

        panlfo[j]->setFreq(panlforate[j]);
        //osc[j]->setMul(1.0);

        drivelfo[j]->setFreq(drivelforate[j]);

        float drivelfoval = drivelfo[j]->ar() + 1.0;
        drivelfoval *= 0.5;
        drivelfoval *= drivelfoamt[j];
        drivelfoval = 1.0 - drivelfoval;

        float monovoice = tanh(main_drive_env_val * pre_gain * folder[j]->ar(drivelfoval *
          mul * osc[j]->ar() *
        smoothdrive[j]->tick(drive[j])))
        * smoothmuls[j]->tick(localmul[j]);

        StereoFrame panned = panner[j].pan(monovoice * envVal * 0.1,
          smoothpanner[j]->tick(localpan[j])
          + (panlfo[j]->ar() * panlfoamt[j])); // panorerings-lfo

        left += panned.leftSample;
        right += panned.rightSample;

      }

      //StereoFrame panned = panner.pan(s * envVal * 0.1, 1.0);

      //s = osc[0]->ar();

      //float left = panned.leftSample;
      //float right = panned.rightSample;

      //float out = s ;
      p(1)[i] += left * main_vol;
      p(2)[i] += right * main_vol;


      if(off == 1){

        if(gate == 0 && envVal < 0.0001) return;

      }


    }


  }


protected:

  unsigned char m_key;
  double m_rate;
  uint32_t m_period;
  uint32_t m_counter;
  float m_envelope;
  //EnvAD* env;
  float envVal;
  EnvelopeGenerator* env;
  WaveTableSine00* osc[4];
  int gate;
  float iv;
  Smooth* smoothamp;
  float mul;
  float crv;
  float freq;
  float fmult[4];
  Folder00* folder[4];
  Smooth* smoothdrive[4];
  //StereoFrame frame;
  Panner panner[4];
  Smooth* smoothpanner[4];
  Smooth* smoothmuls[4];
  Smooth* smoothpregain;
  Smooth* smoothvol;
  //Smooth* smoothpanrate[4];
  //Smooth* smoothpanamt[4];
  WaveTableSine00* panlfo[4];
  WaveTableSine00* drivelfo[4];
  EnvelopeGenerator* main_drive_env;
  Smooth* smooth_drive_amt;

};


class SynthFold00MUS : public LV2::Synth<SynthFold00MUSVoice, SynthFold00MUS> {
public:

  SynthFold00MUS(double rate)
    : LV2::Synth<SynthFold00MUSVoice, SynthFold00MUS>(57, 0) {

      // antal new SynthFold00MUSVoice(rate) är ANTAL RÖSTER!!!!
    add_voices(new SynthFold00MUSVoice(rate), new SynthFold00MUSVoice(rate), new SynthFold00MUSVoice(rate), new SynthFold00MUSVoice(rate),
  new SynthFold00MUSVoice(rate), new SynthFold00MUSVoice(rate), new SynthFold00MUSVoice(rate), new SynthFold00MUSVoice(rate),
new SynthFold00MUSVoice(rate), new SynthFold00MUSVoice(rate), new SynthFold00MUSVoice(rate), new SynthFold00MUSVoice(rate),
new SynthFold00MUSVoice(rate), new SynthFold00MUSVoice(rate), new SynthFold00MUSVoice(rate), new SynthFold00MUSVoice(rate));
    add_audio_outputs(1, 2);

  }





//protected:

};


static int _ = SynthFold00MUS::register_class("http://musens.pluggar/lv2/lv2pftci/synthFold00MUS");

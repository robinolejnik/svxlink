#include <iostream>
#include <iomanip>
#include <string>

#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include <AsyncCppApplication.h>
#include <AsyncAudioIO.h>
#include "AfskModulator.h"
using namespace std;
using namespace Async;

void setNibble(vector<bool> &bits, unsigned char value, bool invert = true) {
  value &= 0xf;
  if(invert) {
    bits.push_back((value>>0) & 1);
    bits.push_back((value>>1) & 1);
    bits.push_back((value>>2) & 1);
    bits.push_back((value>>3) & 1);
  }
  else {
    bits.push_back((value>>3) & 1);
    bits.push_back((value>>2) & 1);
    bits.push_back((value>>1) & 1);
    bits.push_back((value>>0) & 1);
  }
}

int main(int argc, const char **argv) {
  unsigned sample_rate = 16000;

  CppApplication app;

  AudioIO::setSampleRate(sample_rate);
  AudioIO::setBlocksize(256);
  AudioIO::setBlockCount(4);

  AudioIO audio_out("alsa:plughw:1", 0);
  AfskModulator fsk_mod(1800, 1200, 1200, -3, sample_rate);
  fsk_mod.registerSink(&audio_out);

  if (!audio_out.open(AudioIO::MODE_WR)) {
    cerr << "*** ERROR: Could not open audio device for writing\n";
    exit(1);
  }

  vector<bool> bits;

  unsigned int fms = 0x6902008a;
  unsigned int status = 0xf;
  unsigned char bs = 1;
  unsigned int dir = 1;
  unsigned int tk = 3;

  tk--;
  setNibble(bits, 0x7, false);
  setNibble(bits, 0xf, false);
  setNibble(bits, 0xf, false);
  setNibble(bits, 0x1, false);
  setNibble(bits, 0xa, false);

  setNibble(bits, fms>>28); // BOS
  setNibble(bits, fms>>24); // Land
  setNibble(bits, fms>>20); // Ort
  setNibble(bits, fms>>16); // Ort
  setNibble(bits, fms>>12); // Kfz
  setNibble(bits, fms>>8);  // Kfz
  setNibble(bits, fms>>4);  // Kfz
  setNibble(bits, fms>>0);  // Kfz
  setNibble(bits, status);
  setNibble(bits, (bs<<3) | (dir<<2) | tk, false);

  //0100
  bits.push_back(false);
  bits.push_back(true);
  bits.push_back(false);
  bits.push_back(false);

  //1100
  bits.push_back(true);
  bits.push_back(true);
  bits.push_back(false);
  bits.push_back(false);

  fsk_mod.sendBits(bits);
  app.exec();

  return 0;
}

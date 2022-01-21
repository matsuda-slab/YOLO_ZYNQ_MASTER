#ifndef _VIS_MODULE
#define _VIS_MODULE
#include <math.h>
#include <cstdlib>      // srand,rand
float fix_to_float(short input){
  //////////////////////////////////////////////
  //0,1,~ ,15 bit 2の補数
  //0~7bit:整数
  //8~15bit:小数
  //////////////////////////////////////////////

  int bit_width = 16;
  int input_shift_bit;
  float output = 0.0;
  for(int i = 0; i < bit_width; i++){
    input_shift_bit = (input >> i) & 1;
    if(input_shift_bit && i < bit_width / 2){
      output += powf(2.0,(-(bit_width/2 - i)));
    }
    else if(input_shift_bit && bit_width/2 <= i){
      if(i-bit_width/2 == 7)
        output += powf(2.0, i-bit_width/2) * -1.0;
      else
        output += powf(2.0, i-bit_width/2);
    }
  }
  return output;
}

int make_color(int id){
  std::srand(id);
  unsigned int color = rand() % 255;
  if(color>=255){
    color = 255;
  }
  return (int)color;
}
#endif

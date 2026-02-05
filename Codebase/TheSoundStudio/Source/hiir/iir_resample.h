/*==============
 iir_resample.h
 MAA 10/31/2013
 ==============*/

#ifndef IIR_RESAMPLE_H
#define IIR_RESAMPLE_H

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "PolyphaseIir2Designer.h"
#include "Upsampler2xFpu.h"
#include "Downsampler2xFpu.h"

#define NUM_COEFS     8
#define TRANSITION_BW 0.1

/*==================
 class IIR_Resampler
 ==================*/

 class IIR_Resampler
{
public:

 IIR_Resampler(){}
 virtual ~IIR_Resampler(){}

 virtual void proc(float *dest, float *src, int Nin) = 0;

protected:

 void calc_coefs(double *coef_arr, double transition_bw, int N)
 {
//  int k;

//  printf("Calculating %d filter coefficients...\n", N);
//  fflush(stdout);

  hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw
  (
   &coef_arr[0],
   N,
   transition_bw
  );

//  for (k = 0; k < N; k++)
//   printf("%g, ", coef_arr[k]);

//  printf("\n");

//  printf ("Done.\n");

/*
  const double stopband_at =
   hiir::PolyphaseIir2Designer::compute_atten_from_order_tbw
   (
    N,
    transition_bw
   );
*/

//  printf("Transition bandwidth: %f\n", transition_bw);
//  printf("Stopband attenuation: %f dB\n", stopband_at);
 }
};

/*==================
 class IIR_Upsample1
 ==================*/

 class IIR_Upsample1 : public IIR_Resampler
{
public:

 void proc(float *dest, float *src, int Nin)
 {
  int n;
  for (n = 0; n < Nin; n++)
  {
   dest[n] = src[n];
  }
 }
};

/*==================
 class IIR_Upsample2
 ==================*/

 class IIR_Upsample2 : public IIR_Resampler
{
private:

 hiir::Upsampler2xFpu <NUM_COEFS> *uspl;
 double *coef_arr;
 double transition_bw;

public:

 IIR_Upsample2()
 {
  uspl          = new hiir::Upsampler2xFpu <NUM_COEFS>;
  coef_arr      = new double [NUM_COEFS];
  transition_bw = TRANSITION_BW;

  calc_coefs(coef_arr, transition_bw, NUM_COEFS);

  uspl->set_coefs(coef_arr);
  uspl->clear_buffers();
 }

 ~IIR_Upsample2()
 {
  delete uspl;
  delete [] coef_arr;
 }

 void proc(float *dest, float *src, int Nin)
 {
  uspl->process_block(dest, src, Nin);
 }
};

/*==================
 class IIR_Upsample4
 ==================*/

 class IIR_Upsample4 : public IIR_Resampler
{
private:

 hiir::Upsampler2xFpu <NUM_COEFS> *uspl_1;
 hiir::Upsampler2xFpu <NUM_COEFS> *uspl_2;
 double *coef_arr;
 double transition_bw;

public:

 IIR_Upsample4()
 {
  uspl_1        = new hiir::Upsampler2xFpu <NUM_COEFS>;
  uspl_2        = new hiir::Upsampler2xFpu <NUM_COEFS>;
  coef_arr      = new double [NUM_COEFS];
  transition_bw = TRANSITION_BW;

  calc_coefs(coef_arr, transition_bw, NUM_COEFS);

  uspl_1->set_coefs(coef_arr);
  uspl_1->clear_buffers();

  uspl_2->set_coefs(coef_arr);
  uspl_2->clear_buffers();
 }

 ~IIR_Upsample4()
 {
  delete uspl_1;
  delete uspl_2;
  delete [] coef_arr;
 }

 void proc(float *dest, float *src, int Nin)
 {
  float *dest1 = new float [Nin*2];

  uspl_1->process_block(dest1, src,   Nin);
  uspl_2->process_block(dest,  dest1, Nin*2);

  delete [] dest1;
 }
};

/*==================
 class IIR_Upsample8
 ==================*/

 class IIR_Upsample8 : public IIR_Resampler
{
private:

 hiir::Upsampler2xFpu <NUM_COEFS> *uspl_1;
 hiir::Upsampler2xFpu <NUM_COEFS> *uspl_2;
 hiir::Upsampler2xFpu <NUM_COEFS> *uspl_3;
 double *coef_arr;
 double transition_bw;

public:

 IIR_Upsample8()
 {
  uspl_1        = new hiir::Upsampler2xFpu <NUM_COEFS>;
  uspl_2        = new hiir::Upsampler2xFpu <NUM_COEFS>;
  uspl_3        = new hiir::Upsampler2xFpu <NUM_COEFS>;
  coef_arr      = new double [NUM_COEFS];
  transition_bw = TRANSITION_BW;

  calc_coefs(coef_arr, transition_bw, NUM_COEFS);

  uspl_1->set_coefs(coef_arr);
  uspl_1->clear_buffers();

  uspl_2->set_coefs(coef_arr);
  uspl_2->clear_buffers();

  uspl_3->set_coefs(coef_arr);
  uspl_3->clear_buffers();
 }

 ~IIR_Upsample8()
 {
  delete uspl_1;
  delete uspl_2;
  delete uspl_3;
  delete [] coef_arr;
 }

 void proc(float *dest, float *src, int Nin)
 {
  float *dest1 = new float [Nin*2];
  float *dest2 = new float [Nin*4];

  uspl_1->process_block(dest1, src,   Nin);
  uspl_2->process_block(dest2, dest1, Nin*2);
  uspl_3->process_block(dest,  dest2, Nin*4);

  delete [] dest1;
  delete [] dest2;
 }
};

/*====================
 class IIR_Downsample1
 ====================*/

 class IIR_Downsample1 : public IIR_Resampler
{
public:

 void proc(float *dest, float *src, int Nin)
 {
  int n;
  for (n = 0; n < Nin; n++)
  {
   dest[n] = src[n];
  }
 };
};

/*====================
 class IIR_Downsample2
 ====================*/

 class IIR_Downsample2 : public IIR_Resampler
{
private:

 hiir::Downsampler2xFpu <NUM_COEFS> *dspl;
 double *coef_arr;
 double transition_bw;

public:

 IIR_Downsample2()
 {
  dspl          = new hiir::Downsampler2xFpu <NUM_COEFS>;
  coef_arr      = new double [NUM_COEFS];
  transition_bw = TRANSITION_BW;

  calc_coefs(coef_arr, transition_bw, NUM_COEFS);

  dspl->set_coefs(coef_arr);
  dspl->clear_buffers();
 }

 ~IIR_Downsample2()
 {
  delete dspl;
  delete [] coef_arr;
 }

 void proc(float *dest, float *src, int Nin)
 {
  dspl->process_block(dest, src, Nin/2);
 }
};

/*====================
 class IIR_Downsample4
 ====================*/

 class IIR_Downsample4 : public IIR_Resampler
{
private:

 hiir::Downsampler2xFpu <NUM_COEFS> *dspl_1;
 hiir::Downsampler2xFpu <NUM_COEFS> *dspl_2;
 double *coef_arr;
 double transition_bw;

public:

 IIR_Downsample4()
 {
  dspl_1        = new hiir::Downsampler2xFpu <NUM_COEFS>;
  dspl_2        = new hiir::Downsampler2xFpu <NUM_COEFS>;
  coef_arr      = new double [NUM_COEFS];
  transition_bw = TRANSITION_BW;

  calc_coefs(coef_arr, transition_bw, NUM_COEFS);

  dspl_1->set_coefs(coef_arr);
  dspl_1->clear_buffers();

  dspl_2->set_coefs(coef_arr);
  dspl_2->clear_buffers();
 }

 ~IIR_Downsample4()
 {
  delete dspl_1;
  delete dspl_2;
  delete [] coef_arr;
 }

 void proc(float *dest, float *src, int Nin)
 {
  float *dest1 = new float[Nin/2];

  dspl_1->process_block(dest1, src,   Nin/2);
  dspl_2->process_block(dest,  dest1, Nin/4);

  delete [] dest1;
 }
};

/*====================
 class IIR_Downsample8
 ====================*/

 class IIR_Downsample8 : public IIR_Resampler
{
private:

 hiir::Downsampler2xFpu <NUM_COEFS> *dspl_1;
 hiir::Downsampler2xFpu <NUM_COEFS> *dspl_2;
 hiir::Downsampler2xFpu <NUM_COEFS> *dspl_3;
 double *coef_arr;
 double transition_bw;

public:

 IIR_Downsample8()
 {
  dspl_1        = new hiir::Downsampler2xFpu <NUM_COEFS>;
  dspl_2        = new hiir::Downsampler2xFpu <NUM_COEFS>;
  dspl_3        = new hiir::Downsampler2xFpu <NUM_COEFS>;
  coef_arr      = new double [NUM_COEFS];
  transition_bw = TRANSITION_BW;

  calc_coefs(coef_arr, transition_bw, NUM_COEFS);

  dspl_1->set_coefs(coef_arr);
  dspl_1->clear_buffers();

  dspl_2->set_coefs(coef_arr);
  dspl_2->clear_buffers();

  dspl_3->set_coefs(coef_arr);
  dspl_3->clear_buffers();
 }

 ~IIR_Downsample8()
 {
  delete dspl_1;
  delete dspl_2;
  delete dspl_3;
  delete [] coef_arr;
 }

 void proc(float *dest, float *src, int Nin)
 {
  float *dest1 = new float[Nin/2];
  float *dest2 = new float[Nin/4];

  dspl_1->process_block(dest1, src,   Nin/2);
  dspl_2->process_block(dest2, dest1, Nin/4);
  dspl_3->process_block(dest,  dest2, Nin/8);

  delete [] dest1;
  delete [] dest2;
 }
};

/*==================
 class IIR_Upsampler
 ==================*/

 class IIR_Upsampler
{
private:
 IIR_Resampler *up1, *up2, *up4, *up8;
 IIR_Resampler *upsampler;
 int            L;

public:

 IIR_Upsampler(int _L)
 {
  up1 = new IIR_Upsample1;
  up2 = new IIR_Upsample2;
  up4 = new IIR_Upsample4;
  up8 = new IIR_Upsample8;

  L = _L;

  calc_params();
 }

 void set_scale(float v) {if (L != (int) v) {L = (int) v; calc_params();}}

 void calc_params(void)
 {
  switch(L)
  {
   case 1:
    upsampler = up1;
    break;
   case 2:
    upsampler = up2;
    break;
   case 4:
    upsampler = up4;
    break;
   case 8:
    upsampler = up8;
    break;
   default:
    // error
    L = 1;
    upsampler = up1;
    break;
  }
 }

 ~IIR_Upsampler()
 {
  delete upsampler;
 }

 void proc(float *dest, float *src, int Nin)
 {
  upsampler->proc(dest, src, Nin);
 }
};

/*====================
 class IIR_Downsampler
 ====================*/

 class IIR_Downsampler
{
private:

 IIR_Resampler *down1, *down2, *down4, *down8;
 IIR_Resampler *downsampler;
 int            M;

public:

 IIR_Downsampler(int _M)
 {
  down1 = new IIR_Downsample1;
  down2 = new IIR_Downsample2;
  down4 = new IIR_Downsample4;
  down8 = new IIR_Downsample8;

  M = _M;

  calc_params();
 }

 ~IIR_Downsampler()
 {
  delete down1;
  delete down2;
  delete down4;
  delete down8;
 }

 void set_scale(float v) {if (M != (int) v) {M = (int) v; calc_params();}}

 void calc_params(void)
 {
  switch (M)
  {
   case 1:
    downsampler = down1;
    break;
   case 2:
    downsampler = down2;
    break;
   case 4:
    downsampler = down4;
    break;
   case 8:
    downsampler = down8;
    break;
   default:
    // error
    M = 1;
    downsampler = down1;
    break;
  }
 }

 void proc(float *dest, float *src, int Nin)
 {
  downsampler->proc(dest, src, Nin);
 }
};

#endif // IIR_RESAMPLE_H

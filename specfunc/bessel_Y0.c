/* Author:  G. Jungman
 * RCS:     $Id$
 */
#include <config.h>
#include <gsl_math.h>
#include <gsl_errno.h>
#include "bessel_amp_phase.h"
#include "gsl_sf_chebyshev.h"
#include "gsl_sf_bessel.h"


/*-*-*-*-*-*-*-*-*-*-*-* Private Section *-*-*-*-*-*-*-*-*-*-*-*/

/* based on SLATEC besy0, 1980 version, w. fullerton */

/* chebyshev expansions

 series for by0        on the interval  0.	    to  1.60000d+01
					with weighted error   1.20e-17
					 log weighted error  16.92
			       significant figures required  16.15
				    decimal places required  17.48
*/

static double by0_data[13] = {
  -0.011277839392865573,
  -0.12834523756042035,
  -0.10437884799794249,
   0.023662749183969695,
  -0.002090391647700486,
   0.000103975453939057,
  -0.000003369747162423,
   0.000000077293842676,
  -0.000000001324976772,
   0.000000000017648232,
  -0.000000000000188105,
   0.000000000000001641,
  -0.000000000000000011
};
static gsl_sf_cheb_series by0_cs = {
  by0_data,
  12,
  -1, 1,
  (double *)0,
  (double *)0,
  8
};


/*-*-*-*-*-*-*-*-*-*-*-* (semi)Private Implementations *-*-*-*-*-*-*-*-*-*-*-*/

int gsl_sf_bessel_Y0_impl(const double x, double * result)
{
  const double two_over_pi = 2.0/M_PI;
  const double ln_half     = -M_LN2;
  const double x_small     = 2.0 * GSL_SQRT_MACH_EPS;
  const double xmax        = 1.0/GSL_MACH_EPS;

  if (x <= 0.0) {
    *result = 0.0;
    return GSL_EDOM;
  }
  else if(x < x_small){
    double J0;
    int stat_J0 = gsl_sf_bessel_J0_impl(x, &J0);
    *result = two_over_pi*(ln_half + log(x))*J0
	      + 0.375 + gsl_sf_cheb_eval(&by0_cs, -1.0);
    return stat_J0;
  }
  else if(x < 4.0) {
    double J0;
    int stat_J0 = gsl_sf_bessel_J0_impl(x, &J0);
    *result = two_over_pi*(ln_half + log(x))*J0
	      + 0.375 + gsl_sf_cheb_eval(&by0_cs, 0.125*x*x-1.0);
    return stat_J0;
  }
  else if(x < xmax) {
    double z     = 32.0/(x*x) - 1.0;
    double ampl  = (0.75 + gsl_sf_cheb_eval(&_bessel_amp_phase_bm0_cs, z)) / sqrt(x);
    double theta = x - M_PI_4 + gsl_sf_cheb_eval(&_bessel_amp_phase_bth0_cs, z) / x;
    *result = ampl * sin (theta);
    return GSL_SUCCESS;
  }
  else {
    *result = 0.0;
    return GSL_EUNDRFLW;
  }
}


/*-*-*-*-*-*-*-*-*-*-*-* Functions w/ Error Handling *-*-*-*-*-*-*-*-*-*-*-*/

int gsl_sf_bessel_Y0_e(const double x, double * result)
{
  int status = gsl_sf_bessel_Y0_impl(x, result);
  if(status != GSL_SUCCESS) {
    GSL_ERROR("gsl_sf_bessel_Y0_e", status);
  }
  return status;
}


/*-*-*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*-*/

double gsl_sf_bessel_Y0(const double x)
{
  double y;
  int status = gsl_sf_bessel_Y0_impl(x, &y);
  if(status != GSL_SUCCESS) {
    GSL_WARNING("gsl_sf_bessel_Y0", status);
  }
  return y;
}

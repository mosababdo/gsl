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


/* based on SLATEC besj0, 1977 version, w. fullerton */

/* chebyshev expansions for Bessel functions

 series for bj0        on the interval  0.	    to  1.60000d+01
					with weighted error   7.47e-18
					 log weighted error  17.13
			       significant figures required  16.98
				    decimal places required  17.68

*/

static double bj0_data[13] = {
   0.100254161968939137, 
  -0.665223007764405132, 
   0.248983703498281314, 
  -0.0332527231700357697,
   0.0023114179304694015,
  -0.0000991127741995080,
   0.0000028916708643998,
  -0.0000000612108586630,
   0.0000000009838650793,
  -0.0000000000124235515,
   0.0000000000001265433,
  -0.0000000000000010619,
   0.0000000000000000074,
};
static gsl_sf_cheb_series bj0_cs = {
  bj0_data,
  12,
  -1, 1,
  (double *)0,
  (double *)0,
  9
};


/*-*-*-*-*-*-*-*-*-*-*-* (semi)Private Implementations *-*-*-*-*-*-*-*-*-*-*-*/

int gsl_sf_bessel_J0_impl(const double x, double * result)
{
  const double xmin = 2.0 * GSL_SQRT_MACH_EPS;
  const double xmax = 1.0/GSL_SQRT_MACH_EPS;
  double y = fabs(x);

  if(y < xmin) {
    *result = 1.0;
  }
  else if(y <= 4.0) {
    *result = gsl_sf_cheb_eval(&bj0_cs, 0.125*y*y - 1.0);
  }
  else if (y < xmax) {
    double z     = 32.0/(y*y) - 1.0;
    double ampl  = (0.75 + gsl_sf_cheb_eval(&_bessel_amp_phase_bm0_cs, z)) / sqrt(y);
    double theta = y - M_PI_4 + gsl_sf_cheb_eval(&_bessel_amp_phase_bth0_cs, z) / y;
    *result = ampl * cos(theta);
  }
  else {
    double M     = gsl_sf_bessel_asymp_Mnu(0., y);
    double theta = gsl_sf_bessel_asymp_thetanu(0., y);
    *result = M * cos(theta);
  }
  return GSL_SUCCESS;
}


/*-*-*-*-*-*-*-*-*-*-*-* Functions w/ Error Handling *-*-*-*-*-*-*-*-*-*-*-*/

int gsl_sf_bessel_J0_e(const double x, double * result)
{
  int status = gsl_sf_bessel_J0_impl(x, result);
  if(status != GSL_SUCCESS) {
    GSL_ERROR("gsl_sf_bessel_J0_e", status);
  }
  return status;
}


/*-*-*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*-*/

double gsl_sf_bessel_J0(const double x)
{
  double y;
  int status = gsl_sf_bessel_J0_impl(x, &y);
  if(status != GSL_SUCCESS) {
    GSL_WARNING("gsl_sf_bessel_J0", status);
  }
  return y;
}

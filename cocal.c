#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 1e3
#define C 299792.458
#define Tyr 977.8

#define KV \
	X(double, H0, 67.0) \
	X(double, Om, 0.3) \
	X(double, Ol, 0.7) \

#include "kv.h"

void
usage(char *name) {
#ifndef VERSION
#define VERSION "0.0"
#endif
	fprintf(stderr,
		"Cosmology Calculator from Ned Wright (www.astro.ucla.edu/~wright)\n"
		"\nUsage: %s [-h] z\n"
		"\tcosmology configuration is read from standard input\n"
		"\nOptions:\n"
		"\t-h\t help and quit\n"
		"\nArguments:\n"
		"\tz\tredshift\n" "\nVersion: %s\n", name, VERSION);
}

int
main(int argc, char *argv[]) {
	int _;
	if(argc != 2 || strncmp("-h", argv[1], 2) == 0
	   || (_ = getc(stdin)) == EOF || ungetc(_, stdin) == EOF) {
		usage(argv[0]);
		return 1;
	}

	kv cosmo = kv_init();
	kv_read(stdin, &cosmo);

	double z = atof(argv[1]);                 // redshift

	double DA;                                // angular size distance
	double DA_Gyr;
	double DA_Mpc;
	double DCMR = 0.0;                        // comoving radial distance in units of C/H0
	double DCMR_Gyr;
	double DCMR_Mpc;
	double DCMT;
	double DL = 0.0;                          // luminosity distance
	double DL_Gyr = 0.0;
	double DL_Mpc = 0.0;
	double DTT = 0.0;                         // time from z to now in units of 1/H0
	double DTT_Gyr;
	double kpc_DA;                            // physical scale per angular size distance

	double h = cosmo.H0 / 100.0;
	double mM;                                // distance modulus, m-M

	double VCM;                               // volume comoving
	double V_Gpc;

	double a;                                 // 1/(1+z), the scale factor of the Universe
	double adot;
	double age = 0.0;                         // age of Universe in units of 1/H0
	double age_Gyr;
	double az = 1.0 / (1.0 + z);              // expansion factor 1 / (1 + z)
	double zage;                              // age of Universe at redshift z in units of 1/H0
	double zage_Gyr;

	double Or = 4.165e-5 / (h * h);           // Omega radiation; includes 3 massless neutrino species, T0 = 2.72528
	double Ok = 1 - cosmo.Om - Or - cosmo.Ol; // Omega curvaturve = 1-Omega(total)

	double x, y, ratio;                       // numerical variables

	for(int i = 0; i < N; i++) {
		a = az * (i + 0.5) / N;
		adot = sqrt(Ok + (cosmo.Om / a) + (Or / (a * a)) + (cosmo.Ol * a * a));
		age += 1.0 / adot;
	}

	zage = az * age / N;
	zage_Gyr = (Tyr / cosmo.H0) * zage;

	for(int i = 0; i < N; i++) {
		a = az + (1.0 - az) * (i + 0.5) / N;
		adot = sqrt(Ok + (cosmo.Om / a) + (Or / (a * a)) + (cosmo.Ol * a * a));
		DTT += 1.0 / adot;
		DCMR += 1.0 / (a * adot);
	}

	DTT = (1.0 - az) * DTT / N;
	DCMR = (1.0 - az) * DCMR / N;
	age = DTT + zage;
	age_Gyr = age * (Tyr / cosmo.H0);
	DTT_Gyr = (Tyr / cosmo.H0) * DTT;
	DCMR_Gyr = (Tyr / cosmo.H0) * DCMR;
	DCMR_Mpc = (C / cosmo.H0) * DCMR;

	// tangential comoving distance
	x = sqrt(fabs(Ok)) * DCMR;
	if(x > 0.1) {
		if(Ok > 0)
			ratio = 0.5 * (exp(x) - exp(-x)) / x;
		else
			ratio = sin(x) / x;
	} else {
		y = x * x;
		if(Ok < 0)
			y = -y;
		ratio = 1.0 + y / 6.0 + y * y / 120.0;
	}

	DCMT = ratio * DCMR;
	DA = az * DCMT;
	DA_Mpc = (C / cosmo.H0) * DA;
	kpc_DA = DA_Mpc / 206.264806;
	DA_Gyr = (Tyr / cosmo.H0) * DA;
	DL = DA / (az * az);
	DL_Mpc = (C / cosmo.H0) * DL;
	DL_Gyr = (Tyr / cosmo.H0) * DL;
	mM = 5.0 * log10(DL_Mpc * 1000000) - 5.0;

	// comoving volume computation
	x = sqrt(fabs(Ok)) * DCMR;
	if(x > 0.1) {
		if(Ok > 0)
			ratio = (0.125 * (exp(2.0 * x) - exp(-2.0 * x)) - x / 2.0) / (x * x * x / 3.0);
		else
			ratio = (x / 2.0 - sin(2.0 * x) / 4.0) / (x * x * x / 3.0);
	} else {
		y = x * x;
		if(Ok < 0)
			y = -y;
		ratio = 1.0 + y / 5.0 + (2.0 / 105.0) * y * y;
	}

	VCM = ratio * DCMR * DCMR * DCMR / 3.0;
	V_Gpc = 4.0 * M_PI * pow((0.001 * C / cosmo.H0), 3.0) * VCM;

	printf(
		"t0    = %12.3f  # age of the Universe [Gyr]\n"
		"tz    = %12.3f  # time at redshift z [Gyr]\n"
		"DLTT  = %12.3f  # time from redshift z [Gyr]\n"
		"DCMR  = %12.3f  # comoving radial distance [Mpc]\n"
		"DCMR  = %12.3f  # comoving radial distance [Gly]\n"
		"DA    = %12.3f  # angular size distance [Mpc]\n"
		"DA    = %12.3f  # angular size distance [Gly]\n"
		"scale = %12.3f  # scale [kpc/\"]\n"
		"V     = %12.3f  # volume [Gpc^3]\n"
		"DL    = %12.3f  # luminosity distance [Mpc]\n"
		"DL    = %12.3f  # luminosity distance [Gly]\n"
		"mM    = %12.3f  # distance modulus m-M\n",
		age_Gyr, zage_Gyr, DTT_Gyr, DCMR_Mpc, DCMR_Gyr, DA_Mpc, DA_Gyr, kpc_DA, V_Gpc,
		DL_Mpc, DL_Gyr, mM
	);

	return 0;
}

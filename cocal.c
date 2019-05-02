#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <err.h>

#define N 1e3
#define M_PI 3.14159265358979323846
#define C 299792.458
#define Tyr 977.8

void
usage(char *name) {
	fprintf(stderr,
		"Cosmology Calculator from Ned Wright (www.astro.ucla.edu/~wright)\n"
		"\nUsage: %s [-h] z H0 O_m O_l\n"
		"\nOptions:\n"
		"  -h        display help and quit\n"
		"\nArguments:\n"
		"  z         redshift\n"
		"  H0        Hubble constant\n"
		"  O_m       matter density\n"
		"  O_l       cosmological constant density\n"
		"\nVersion: %s\n", name, VERSION);
	exit(1);
}

int
main(int argc, char *argv[]) {
	double z, H0, Om, Ol;
	double Or, Ok, DTT, DTT_Gyr, age, age_Gyr, zage, zage_Gyr, DCMR,
		DCMR_Mpc, DCMR_Gyr, DA, DA_Mpc, DA_Gyr, kpc_DA, DL, DL_Mpc,
		DL_Gyr, V_Gpc, a, az, adot, h, ratio, x, y, DCMT, VCM, mM;

	if((argc == 1) || (!strncmp("-h", argv[1], 3))) {
		usage(argv[0]);
	} else {
		if(argc == 5) {
			z = atof(argv[1]);
			H0 = atof(argv[2]);
			Om = atof(argv[3]);
			Ol = atof(argv[4]);
		} else {
			usage(argv[0]);
		}
	}

	Or = 0.0;	// Omega(radiation)
	Ok = 0.0;	// Omega curvaturve = 1-Omega(total)
	DTT = 0.5;	// time from z to now in units of 1/H0
	DTT_Gyr = 0.0;	// value of DTT in Gyr
	age = 0.5;	// age of Universe in units of 1/H0
	age_Gyr = 0.0;	// value of age in Gyr
	zage = 0.1;	// age of Universe at redshift z in units of 1/H0
	zage_Gyr = 0.0;	// value of zage in Gyr
	DCMR = 0.0;	// comoving radial distance in units of c/H0
	DCMR_Mpc = 0.0;
	DCMR_Gyr = 0.0;
	DA = 0.0;	// angular size distance
	DA_Mpc = 0.0;
	DA_Gyr = 0.0;
	kpc_DA = 0.0;
	DL = 0.0;	// luminosity distance
	DL_Mpc = 0.0;
	DL_Gyr = 0.0;	// DL in units of billions of light years
	mM = 0.0;	// distance modulus, m-M
	V_Gpc = 0.0;
	a = 1.0;	// 1/(1+z), the scale factor of the Universe
	az = 0.5;	// 1/(1+z(object))

	adot = 0.0;
	h = H0 / 100.0;
	Or = 4.165e-5 / (h * h);	// includes 3 massless neutrino species, T0 = 2.72528
	Ok = 1 - Om - Or - Ol;
	az = 1.0 / (1.0 + 1.0 * z);
	age = 0.0;

	for(int i = 0; i < N; i++) {
		a = az * (i + 0.5) / N;
		adot = sqrt(Ok + (Om / a) + (Or / (a * a)) + (Ol * a * a));
		age = age + 1.0 / adot;
	}

	zage = az * age / N;
	zage_Gyr = (Tyr / H0) * zage;
	DTT = 0.0;
	DCMR = 0.0;

	for(int i = 0; i < N; i++) {
		a = az + (1.0 - az) * (i + 0.5) / N;
		adot = sqrt(Ok + (Om / a) + (Or / (a * a)) + (Ol * a * a));
		DTT = DTT + 1.0 / adot;
		DCMR = DCMR + 1.0 / (a * adot);
	}

	DTT = (1.0 - az) * DTT / N;
	DCMR = (1.0 - az) * DCMR / N;
	age = DTT + zage;
	age_Gyr = age * (Tyr / H0);
	DTT_Gyr = (Tyr / H0) * DTT;
	DCMR_Gyr = (Tyr / H0) * DCMR;
	DCMR_Mpc = (C / H0) * DCMR;

	// tangential comoving distance
	ratio = 1.00;
	x = sqrt(fabs(Ok)) * DCMR;
	if(x > 0.1) {
		if(Ok > 0) {
			ratio = 0.5 * (exp(x) - exp(-x)) / x;
		} else {
			ratio = sin(x) / x;
		}
	} else {
		y = x * x;
		if(Ok < 0) {
			y = -y;
		}
		ratio = 1.0 + y / 6.0 + y * y / 120.0;
	}

	DCMT = ratio * DCMR;
	DA = az * DCMT;
	DA_Mpc = (C / H0) * DA;
	kpc_DA = DA_Mpc / 206.264806;
	DA_Gyr = (Tyr / H0) * DA;
	DL = DA / (az * az);
	DL_Mpc = (C / H0) * DL;
	DL_Gyr = (Tyr / H0) * DL;
	mM = 5.0 * log10(DL_Mpc * 1000000) - 5.0;

	// comoving volume computation
	ratio = 1.00;
	x = sqrt(fabs(Ok)) * DCMR;
	if(x > 0.1) {
		if(Ok > 0) {
			ratio = (0.125 * (exp(2.0 * x) - exp(-2.0 * x)) -
				 x / 2.0) / (x * x * x / 3.0);
		} else {
			ratio = (x / 2.0 -
				 sin(2.0 * x) / 4.0) / (x * x * x / 3.0);
		}
	} else {
		y = x * x;
		if(Ok < 0) {
			y = -y;
		}
		ratio = 1.0 + y / 5.0 + (2.0 / 105.0) * y * y;
	}
	VCM = ratio * DCMR * DCMR * DCMR / 3.0;
	V_Gpc = 4.0 * M_PI * pow((0.001 * C / H0), 3.0) * VCM;

	//dsv output
	printf("variable\tvalue\tunits\n--------\t-----\t-----\n");
	printf("t(0)\t%13.3f\tGly\n"
	       "t(z)\t%13.3f\tGly\n"
	       "DLTT\t%13.3f\tGyr\n"
	       "DCMR\t%13.3f\tMpc\n"
	       "DCMR\t%13.3f\tGly\n"
	       "DA\t%13.3f\tMpc\n"
	       "DA\t%13.3f\tGly\n"
	       "scale\t%13.3f\tkpc/\"\n"
	       "V\t%13.3f\tGpc3\n"
	       "DL\t%13.3f\tMpc\n"
	       "DL\t%13.3f\tGly\n"
	       "mM\t%13.3f\t-\n", age_Gyr, zage_Gyr, DTT_Gyr, DCMR_Mpc,
	       DCMR_Gyr, DA_Mpc, DA_Gyr, kpc_DA, V_Gpc, DL_Mpc, DL_Gyr, mM);

	/*           // json output */
	/*           printf("{\n"); */
	/*           printf("\t\"t(0)\": %.3f,\n" */
	/*                  "\t\"t(z)\": %.3f,\n" */
	/*                  "\t\"DLTT\": %.3f,\n" */
	/*                  "\t\"DCMR\": %.3f,\n" */
	/*                  "\t\"DCMR\": %.3f,\n" */
	/*                  "\t\"DA\": %.3f,\n" */
	/*                  "\t\"DA\": %.3f,\n" */
	/*                  "\t\"scale\": %.3f,\n" */
	/*                  "\t\"V\": %.3f,\n" */
	/*                  "\t\"DL\": %.3f,\n" */
	/*                  "\t\"DL\": %.3f,\n" */
	/*                  "\t\"mM\": %.3f\n", age_Gyr, zage_Gyr, DTT_Gyr, DCMR_Mpc, DCMR_Gyr, */
	/*                  DA_Mpc, DA_Gyr, kpc_DA, V_Gpc, DL_Mpc, DL_Gyr, mM */
	/*                   ); */
	/*           printf("}\n"); */

	exit(0);
}

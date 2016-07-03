WIND_LIB = ../../sw/airborne/modules/meteo/lib_ukf_wind_estimator
MATH_LIB = ../../sw/airborne/math
STD_LIB = ../../sw/include

CFLAGS = $(shell pkg-config ivy-glib --cflags) -I$(WIND_LIB) -I$(MATH_LIB) -I$(STD_LIB)
main:
	gcc -Wall -fPIC $(CFLAGS) -o wind_estimator wind_estimator.c $(WIND_LIB)/Gen_UKF.c $(MATH_LIB)/pprz_algebra_float.c $(shell pkg-config ivy-glib --libs) -lm  -lpcre

clean:
	rm -rf wind_estimator


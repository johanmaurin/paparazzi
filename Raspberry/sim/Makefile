main:
	gcc -Wall -fPIC $(shell pkg-config ivy-glib --cflags) -o raspberry raspberry.c Gen_UKF.c $(shell pkg-config ivy-glib --libs) -lm  -lpcre

/* public domain
 *
 * The calculation are approximations. Ignored factors (among others, I guess):
 *
 *   1) the orbit around the sun is not a circle
 *   2) the altitude of your current position
 *   3) the diameter of the sun disc in the sky
 *   4) the light is refracted by by the atmosphere
 *
 *   https://en.wikipedia.org/wiki/Position_of_the_Sun#Calculations
 *   https://en.wikipedia.org/wiki/Sunrise_equation
 *   https://en.wikipedia.org/wiki/Equation_of_time
 */

#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include <stdio.h> // printf
#include <math.h> // fmod,acos,cos,asin
#include <time.h> // time, gmtime
#include <errno.h> // perror

/* superfluously accurate PI */
#define CIRCLE  (2*3.14159265358979323846)
#define CIRCLE_HALF (CIRCLE/2.0)
/* -23.44 degrees in radial angle */
#define MAX_SUN_DECLINATION (-.40903)
#define RAD_PER_DAY (CIRCLE/365.0)
#define RAD_PER_DEG (CIRCLE/360.0)
/* days that January the 1st is behind the longest night */
#define SOUTHERN_SOLSCTICES_OFFSET 10.0

enum day_state { NIGHT=0, DAWN, SUNRISE, DAY, SUNSET, DUSK };
const char * day_state_names [] = {
	[DAWN]="dawn",
	[SUNRISE]="sunrise",
	[DAY]="day",
	[SUNSET]="sunset",
	[DUSK]="dusk",
	[NIGHT]="night",
};

struct tm *
greenwich_mean_time(void)
{
	time_t now = time(NULL);
	struct tm *gmt = gmtime(&now);
	if (gmt == NULL) {
		perror("gmtime");
		exit(EXIT_FAILURE);
	}
	return gmt;
}

/* Earth sketched with the sun in the northern (up) summer position.
 *
 *                                day-night-plane
 *       a sunray           |
 *            .           _,|,_ /
 *              ` .    .`   |  /o.
 *   decline        ` /     | /ooo\
 *                   '  ` . |/ooøøø'
 *              -----|------/oooøøø|----- equator-plane
 *                   '     /|ooøøøø'
 *                    \.  /o|oøøøø/ .
 *                      '/oo|øø-'     `
 *                      /  `|`
 *                     /    |
 *                         axis
 *
 */
int
is_day(double latitude, double longitude)
{
	struct tm *gmt = greenwich_mean_time();
	/* 0 to 365 */
	double calendar_day = (double)gmt->tm_yday; 
	/* the fraction of the GMT day */
	double day_fraction = (gmt->tm_hour*60.+gmt->tm_min)/(24.*60.);
	/* essentially the angle of earth position in orbit around the sun (according to Wikipedia) */
	double elliptic_longitude = RAD_PER_DAY * (SOUTHERN_SOLSCTICES_OFFSET + calendar_day + day_fraction);
	/* angle of equatorial plane and the sun rays */
	double sun_decline = asin(sin(MAX_SUN_DECLINATION) * cos(elliptic_longitude));
	/* the "sunset" should start at most one hour and 15 minutes before
	 * actual sunset, if latidue is 90. That is obviously not correct, but
	 * it gives good enough estimates */
	double diffusion =  fmod(CIRCLE+(1.25/24.)*4.*(latitude-sun_decline), CIRCLE);
	/* angle of earth's rotation around it's axis */
	double day_angle = CIRCLE*day_fraction;
	/* angle that is missing until noon at your position */
	double noon_angle = CIRCLE_HALF+day_angle+longitude;
	/* angle between the disc slicing the earth into night and day
	 * and the latitude disc at midnight (where the angle would be 90
	 * deg, if decline was 0) */
	double season_angle = acos( -tan(latitude) * tan(sun_decline) );
	double sun_rise_angle = fmod(CIRCLE+noon_angle+season_angle, CIRCLE);
	double sun_set_angle = fmod(CIRCLE+noon_angle-season_angle, CIRCLE);

	if (sun_set_angle < diffusion) {
		return DUSK;
	}
	if (sun_set_angle > CIRCLE-diffusion) {
		return SUNSET;
	}
	if (sun_rise_angle < diffusion) {
		return SUNRISE;
	}
	if (sun_rise_angle > CIRCLE-diffusion) {
		return DAWN;
	}
	if (sun_rise_angle < sun_set_angle) {
		return DAY;
	}
	return NIGHT;
}

int
main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("usage: %s LATITUDE(degrees) LONGITUDE(degrees)\n", argv[0]);
	} else {
		double latitude = CIRCLE * atof(argv[1])/360.;
		double longitude = CIRCLE * fmod(atof(argv[2])/360. + 1., 1.);
		int day_state = is_day(latitude, longitude);

		puts(day_state_names[day_state]);

		if (day_state == SUNRISE || day_state == DAY || day_state == SUNSET) {
			return EXIT_SUCCESS;
		}
	}

	return EXIT_FAILURE;
}

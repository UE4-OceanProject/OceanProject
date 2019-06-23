package tomasalonso;

import java.util.Calendar;

/**
 * A very simple Sun/Moon calculator without using JPARSEC library.
 * @author T. Alonso Albi - OAN (Spain), email t.alonso@oan.es
 * @version May 25, 2017 (fixed nutation correction and moon age, better accuracy in Moon)
 */
public class SunMoonCalculator {
 
	/** Radians to degrees. */
	public static final double RAD_TO_DEG = 180.0 / Math.PI;
 
	/** Degrees to radians. */
	public static final double DEG_TO_RAD = 1.0 / RAD_TO_DEG;
 
	/** Radians to hours. */
	public static final double RAD_TO_HOUR = 180.0 / (15.0 * Math.PI);
 
	/** Radians to days. */
	public static final double RAD_TO_DAY = RAD_TO_HOUR / 24.0;
 
	/** Astronomical Unit in km. As defined by JPL. */
	public static final double AU = 149597870.691;
 
	/** Earth equatorial radius in km. IERS 2003 Conventions. */
	public static final double EARTH_RADIUS = 6378.1366;
 
	/** Two times Pi. */
	public static final double TWO_PI = 2.0 * Math.PI;
 
	/** The inverse of two times Pi. */
	public static final double TWO_PI_INVERSE = 1.0 / (2.0 * Math.PI);
 
	/** Four times Pi. */
	public static final double FOUR_PI = 4.0 * Math.PI;
 
	/** Pi divided by two. */
	public static final double PI_OVER_TWO = Math.PI / 2.0;
 
	/** Length of a sidereal day in days according to IERS Conventions. */
	public static final double SIDEREAL_DAY_LENGTH = 1.00273781191135448;
 
	/** Julian century conversion constant = 100 * days per year. */
	public static final double JULIAN_DAYS_PER_CENTURY = 36525.0;
 
	/** Seconds in one day. */
	public static final double SECONDS_PER_DAY = 86400;
 
	/** Our default epoch. <BR>
	 * The Julian Day which represents noon on 2000-01-01. */
	public static final double J2000 = 2451545.0;

	//XXX Added constant as is used at multiple places
	/** Lunar cycle length in days*/
	final public static double LUNAR_CYCLE_DAYS = 29.530588853;
 
	/**
	 * The set of twilights to calculate (types of rise/set events).
	 */
	public static enum TWILIGHT {
		/**
		 * Event ID for calculation of rising and setting times for astronomical
		 * twilight. In this case, the calculated time will be the time when the
		 * center of the object is at -18 degrees of geometrical elevation below the
		 * astronomical horizon. At this time astronomical observations are possible
		 * because the sky is dark enough.
		 */
		TWILIGHT_ASTRONOMICAL,
		/**
		 * Event ID for calculation of rising and setting times for nautical
		 * twilight. In this case, the calculated time will be the time when the
		 * center of the object is at -12 degrees of geometric elevation below the
		 * astronomical horizon.
		 */
		TWILIGHT_NAUTICAL,
		/**
		 * Event ID for calculation of rising and setting times for civil twilight.
		 * In this case, the calculated time will be the time when the center of the
		 * object is at -6 degrees of geometric elevation below the astronomical
		 * horizon.
		 */
		TWILIGHT_CIVIL,
		/**
		 * The standard value of 34' for the refraction at the local horizon.
		 */
		HORIZON_34arcmin
	};
 
 
	/** Input values. */
	private double jd_UT = 0, t = 0, obsLon = 0, obsLat = 0, TTminusUT = 0;
	private TWILIGHT twilight = TWILIGHT.HORIZON_34arcmin;
	private double slongitude = 0, sanomaly = 0;
 
	/** Values for azimuth, elevation, rise, set, and transit for the Sun. Angles in radians, rise ... as Julian days in UT. 
	 * Distance in AU. */
	public double sunAz, sunEl, sunRise, sunSet, sunTransit, sunTransitElev, sunDist;
 
	/** Values for azimuth, elevation, rise, set, and transit for the Moon. Angles in radians, rise ... as Julian days in UT. 
	 * Moon age is the number of days since last new Moon, in days, from 0 to 29.5. Distance in AU. */
	public double moonAz, moonEl, moonRise, moonSet, moonTransit, moonAge, moonTransitElev, moonDist;

	//XXX Added new fields for moon
	/** Moon illumination percentage (0-1), Position angle of axis, Bright Limb angle, Paralactic angle*/
	public double moonIll, moonP, moonBL, moonPar;
 
 
	/**
	 * Main constructor for Sun/Moon calculations. Time should be given in
	 * Universal Time (UT), observer angles in radians.
	 * @param year The year.
	 * @param month The month.
	 * @param day The day.
	 * @param h The hour.
	 * @param m Minute.
	 * @param s Second.
	 * @param obsLon Longitude for the observer.
	 * @param obsLat Latitude for the observer.
	 * @throws Exception If the date does not exists.
	 */
	public SunMoonCalculator(int year, int month, int day, int h, int m, int s,
			double obsLon, double obsLat) throws Exception {
		// The conversion formulas are from Meeus, chapter 7.
		boolean julian = false;
		if (year < 1582 || (year == 1582 && month <= 10) || (year == 1582 && month == 10 && day < 15)) julian = true;
		int D = day;
		int M = month;
		int Y = year;
		if (M < 3)
		{
			Y--;
			M += 12;
		}
		int A = Y / 100;
		int B = julian ? 0 : 2 - A + A / 4;
 
		double dayFraction = (h + (m + (s / 60.0)) / 60.0) / 24.0;
 
		double jd = dayFraction + (int) (365.25D * (Y + 4716)) + (int) (30.6001 * (M + 1)) + D + B - 1524.5;
 
		if (jd < 2299160.0 && jd >= 2299150.0)
		{
			throw new Exception("invalid julian day " + jd + ". This date does not exist.");
		}
 
		TTminusUT = 0;
		if (year > -600 && year < 2200) {
			double x = year + (month - 1 + day / 30.0) / 12.0;
			double x2 = x * x, x3 = x2 * x, x4 = x3 * x;
			if (year < 1600) {
				TTminusUT = 10535.328003326353 - 9.995238627481024 * x + 0.003067307630020489 * x2 - 7.76340698361363E-6 * x3 + 3.1331045394223196E-9 * x4 + 
						8.225530854405553E-12 * x2 * x3 - 7.486164715632051E-15 * x4 * x2 + 1.9362461549678834E-18 * x4 * x3 - 8.489224937827653E-23 * x4 * x4;
			} else {
				TTminusUT = -1027175.3477559977 + 2523.256625418965 * x - 1.885686849058459 * x2 + 5.869246227888417E-5 * x3 + 3.3379295816475025E-7 * x4 + 
						1.7758961671447929E-10 * x2 * x3 - 2.7889902806153024E-13 * x2 * x4 + 1.0224295822336825E-16 * x3 * x4 - 1.2528102370680435E-20 * x4 * x4;
			}
		}
		this.obsLon = obsLon;
		this.obsLat = obsLat;
		setUTDate(jd);
	}

	//XXX Added
	/**
	 * Alternate constructor for Sun/Moon calculations. Calendar should be given in
	 * Universal Time (UT), observer angles in radians.
	 * @param calendarSunMoon Calendar with date/time.
	 * @param obsLon Longitude for the observer.
	 * @param obsLat Latitude for the observer.
	 * @throws Exception If the date does not exists.
	 */
	public SunMoonCalculator(Calendar calendarSunMoon, double obsLon, double obsLat) throws Exception {
		this(calendarSunMoon.get(Calendar.YEAR), calendarSunMoon.get(Calendar.MONTH) + 1,
				calendarSunMoon.get(Calendar.DAY_OF_MONTH), calendarSunMoon.get(Calendar.HOUR_OF_DAY),
				calendarSunMoon.get(Calendar.MINUTE), calendarSunMoon.get(Calendar.SECOND),
				obsLon, obsLat);
	}

	//XXX Added - actually a modification of the method posted by you (Tomas)
	/**@return {optical librations (lp), lunar coordinates of the centre of the disk (bp),
	position angle of axis (p), bright limb angle (bl), paralactic angle (par)}*/
	private double[] getMoonDiskOrientationAngles(double lst, double sunRA, double sunDec,
			double moonLon, double moonLat, double moonRA, double moonDec) {

		// Moon's argument of latitude
		double F = Math.toRadians(93.2720993 + 483202.0175273 * t - 0.0034029 * t * t - t * t * t / 3526000.0 + t * t * t * t / 863310000.0);
		// Moon's inclination
		double I = Math.toRadians(1.54242);
		// Moon's mean ascending node longitude
		double omega = Math.toRadians(125.0445550 - 1934.1361849 * t + 0.0020762 * t * t + t * t * t / 467410.0 - t * t * t * t / 18999000.0);
		// Obliquity of ecliptic (approx, better formulae up)
		double eps = Math.toRadians(23.43929);
	 
		// Obtain optical librations lp and bp
		double W = moonLon - omega;
		double sinA = Math.sin(W) * Math.cos(moonLat) * Math.cos(I) - Math.sin(moonLat) * Math.sin(I);
		double cosA = Math.cos(W) * Math.cos(moonLat);
		double A = Math.atan2(sinA, cosA);
		double lp = normalizeRadians(A - F);
		double sinbp = - Math.sin(W) * Math.cos(moonLat) * Math.sin(I) - Math.sin(moonLat) * Math.cos(I);
		double bp = Math.asin(sinbp);
	 
		// Obtain position angle of axis p
		double x = Math.sin(I) * Math.sin(omega);
		double y = Math.sin(I) * Math.cos(omega) * Math.cos(eps) - Math.cos(I) * Math.sin(eps);
		double w = Math.atan2(x, y);
		double sinp = Math.sqrt(x*x + y*y) * Math.cos(moonRA - w) / Math.cos(bp);
		double p = Math.asin(sinp);
	 
		// Compute bright limb angle bl
		double bl = (Math.PI + Math.atan2(Math.cos(sunDec) * Math.sin(moonRA - sunRA),
				Math.cos(sunDec) * Math.sin(moonDec) * Math.cos(moonRA - sunRA)
				- Math.sin(sunDec) * Math.cos(moonDec)));
	 
		// Paralactic angle par
		y = Math.sin(lst - moonRA);
		x = Math.tan(obsLat) * Math.cos(moonDec) - Math.sin(moonDec) * Math.cos(lst - moonRA);
		double par = x != 0 ? Math.atan2(y, x) : (y / Math.abs(y)) * Math.PI / 2;

		return new double[] {lp, bp, p, bl, par};
	}

	//XXX Added - You may be able to make this more accurate
	/** Method to return the phase of the Moon as per Moon Age (days)*/
	public static String getMoonPhaseName(double lunarAge) {
		if (lunarAge >= 0 && lunarAge <= LUNAR_CYCLE_DAYS
				&& (lunarAge < 1 || lunarAge > LUNAR_CYCLE_DAYS - 1))
			return "New Moon";
		else if (lunarAge >= 1 && lunarAge < 6.4)
			return "Waxing Crescent";
		else if (lunarAge >= 6.4 && lunarAge < 8.4)
			return "First Quarter";
		else if (lunarAge >= 8.4 && lunarAge < 13.8)
			return "Waxing Gibbous";
		else if (lunarAge >= 13.8 && lunarAge < 15.8)
			return "Full Moon";
		else if (lunarAge >= 15.8 && lunarAge < 21.1)
			return "Waning Gibbous";
		else if (lunarAge >= 21.1 && lunarAge < 23.1)
			return "Last/Third Quarter";
		else if (lunarAge >= 23.1 && lunarAge <= LUNAR_CYCLE_DAYS - 1)
			return "Waning Crescent";
		else
			return "-";
	}
 
	/**
	 * Sets the rise/set times to return. Default is
	 * for the local horizon.
	 * @param t The Twilight.
	 */
	public void setTwilight(TWILIGHT t) {
		this.twilight = t;
	}
 
	private void setUTDate(double jd) {
		this.jd_UT = jd;
		this.t = (jd + TTminusUT / SECONDS_PER_DAY  - J2000) / JULIAN_DAYS_PER_CENTURY;
	}
 
	/** Calculates everything for the Sun and the Moon. */
	public void calcSunAndMoon() {
		double jd = this.jd_UT;
 
		// First the Sun
		double out[] = doCalc(getSun());
		sunAz = out[0];
		sunEl = out[1];
		sunRise = out[2];
		sunSet = out[3];
		sunTransit = out[4];
		sunTransitElev = out[5];
		sunDist = out[8];
		double sa = sanomaly, sl = slongitude;

		//XXX Added variables needed for Moon Disk calculation
		double sunRA = out[6], sunDec = out[7], lst = out[9];
 
		int niter = 3; // Number of iterations to get accurate rise/set/transit times
		sunRise = obtainAccurateRiseSetTransit(sunRise, 2, niter, true);
		sunSet = obtainAccurateRiseSetTransit(sunSet, 3, niter, true);
		sunTransit = obtainAccurateRiseSetTransit(sunTransit, 4, niter, true);
		if (sunTransit == -1) {
			sunTransitElev = 0;
		} else {
			// Update Sun's maximum elevation
			setUTDate(sunTransit);
			out = doCalc(getSun());
			sunTransitElev = out[5];
		}
 
		// Now Moon
		setUTDate(jd);
		sanomaly = sa;
		slongitude = sl;

		out = getMoon(); //XXX Changed from out = doCalc(getMoon());
		//XXX Added variables needed for Moon Disk calculation
		double moonLon = out[0], moonLat = out[1];
		out = doCalc(out);

		moonAz = out[0];
		moonEl = out[1];
		moonRise = out[2];
		moonSet = out[3];
		moonTransit = out[4];
		moonTransitElev = out[5];
		moonDist = out[8];

		//XXX Added setting of field and variables needed for Moon Disk calculation
		moonIll = (1 - Math.cos(moonAge / LUNAR_CYCLE_DAYS * 2 * Math.PI)) / 2;
		double moonRA = out[6], moonDec = out[7];

		double ma = moonAge;
 
		niter = 5; // Number of iterations to get accurate rise/set/transit times
		moonRise = obtainAccurateRiseSetTransit(moonRise, 2, niter, false);
		moonSet = obtainAccurateRiseSetTransit(moonSet, 3, niter, false);
		moonTransit = obtainAccurateRiseSetTransit(moonTransit, 4, niter, false);
		if (moonTransit == -1) {
			moonTransitElev = 0;
		} else {
			// Update Moon's maximum elevation
			setUTDate(moonTransit);
			getSun();
			out = doCalc(getMoon());
			moonTransitElev = out[5];
		}
		setUTDate(jd);
		sanomaly = sa;
		slongitude = sl;
		moonAge = ma;

		//XXX Added calculate moon disk values and set fields 
		out = getMoonDiskOrientationAngles(lst, sunRA, sunDec, Math.toRadians(moonLon),
				Math.toRadians(moonLat), moonRA, moonDec);
		moonP = out[2];
		moonBL = out[3];
		moonPar = out[4];
	}
 
	private double[] getSun() {
		// SUN PARAMETERS (Formulae from "Calendrical Calculations")
		double lon = (280.46645 + 36000.76983 * t + .0003032 * t * t);
		double anom = (357.5291 + 35999.0503 * t - .0001559 * t * t - 4.8E-07 * t * t * t);
		sanomaly = anom * DEG_TO_RAD;
		double c = (1.9146 - .004817 * t - .000014 * t * t) * Math.sin(sanomaly);
		c = c + (.019993 - .000101 * t) * Math.sin(2 * sanomaly);
		c = c + .00029 * Math.sin(3.0 * sanomaly); // Correction to the mean ecliptic longitude
 
		// Now, let calculate nutation and aberration
		double M1 = (124.90 - 1934.134 * t + 0.002063 * t * t) * DEG_TO_RAD;
		double M2 = (201.11 + 72001.5377 * t + 0.00057 * t * t) * DEG_TO_RAD;
		double d = -.00569 - .0047785 * Math.sin(M1) - .0003667 * Math.sin(M2);
 
		slongitude = lon + c + d; // apparent longitude (error<0.003 deg)
		double slatitude = 0; // Sun's ecliptic latitude is always negligible
		double ecc = .016708617 - 4.2037E-05 * t - 1.236E-07 * t * t; // Eccentricity
		double v = sanomaly + c * DEG_TO_RAD; // True anomaly
		double sdistance = 1.000001018 * (1.0 - ecc * ecc) / (1.0 + ecc * Math.cos(v)); // In UA
 
		return new double[] {slongitude, slatitude, sdistance, Math.atan(696000 / (AU * sdistance))};
	}
 
	private double[] getMoon() {
		// MOON PARAMETERS (Formulae from "Calendrical Calculations")
		double phase = normalizeRadians((297.8502042 + 445267.1115168 * t - 0.00163 * t * t + t * t * t / 538841 - t * t * t * t / 65194000) * DEG_TO_RAD);
 
		// Anomalistic phase
		double anomaly = (134.9634114 + 477198.8676313 * t + .008997 * t * t + t * t * t / 69699 - t * t * t * t / 14712000);
		anomaly = anomaly * DEG_TO_RAD;
 
		// Degrees from ascending node
		double node = (93.2720993 + 483202.0175273 * t - 0.0034029 * t * t - t * t * t / 3526000 + t * t * t * t / 863310000);
		node = node * DEG_TO_RAD;
 
		double E = 1.0 - (.002495 + 7.52E-06 * (t + 1.0)) * (t + 1.0);
 
		// Now longitude, with the three main correcting terms of evection,
		// variation, and equation of year, plus other terms (error<0.01 deg)
		// P. Duffet's MOON program taken as reference
		double l = (218.31664563 + 481267.8811958 * t - .00146639 * t * t + t * t * t / 540135.03 - t * t * t * t / 65193770.4);
		l += 6.28875 * Math.sin(anomaly) + 1.274018 * Math.sin(2 * phase - anomaly) + .658309 * Math.sin(2 * phase);
		l +=  0.213616 * Math.sin(2 * anomaly) - E * .185596 * Math.sin(sanomaly) - 0.114336 * Math.sin(2 * node);
		l += .058793 * Math.sin(2 * phase - 2 * anomaly) + .057212 * E * Math.sin(2 * phase - anomaly - sanomaly) + .05332 * Math.sin(2 * phase + anomaly);
		l += .045874 * E * Math.sin(2 * phase - sanomaly) + .041024 * E * Math.sin(anomaly - sanomaly) - .034718 * Math.sin(phase) - E * .030465 * Math.sin(sanomaly + anomaly);
		l += .015326 * Math.sin(2 * (phase - node)) - .012528 * Math.sin(2 * node + anomaly) - .01098 * Math.sin(2 * node - anomaly) + .010674 * Math.sin(4 * phase - anomaly);
		l += .010034 * Math.sin(3 * anomaly) + .008548 * Math.sin(4 * phase - 2 * anomaly);
		l += -E * .00791 * Math.sin(sanomaly - anomaly + 2 * phase) - E * .006783 * Math.sin(2 * phase + sanomaly) + .005162 * Math.sin(anomaly - phase) + E * .005 * Math.sin(sanomaly + phase);
		l += .003862 * Math.sin(4 * phase) + E * .004049 * Math.sin(anomaly - sanomaly + 2 * phase) + .003996 * Math.sin(2 * (anomaly + phase)) + .003665 * Math.sin(2 * phase - 3 * anomaly);
		l += E * 2.695E-3 * Math.sin(2 * anomaly - sanomaly) + 2.602E-3 * Math.sin(anomaly - 2*(node+phase));
		l += E * 2.396E-3 * Math.sin(2*(phase - anomaly) - sanomaly) - 2.349E-3 * Math.sin(anomaly+phase);
		l += E * E * 2.249E-3 * Math.sin(2*(phase-sanomaly)) - E * 2.125E-3 * Math.sin(2*anomaly+sanomaly);
		l += -E * E * 2.079E-3 * Math.sin(2*sanomaly) + E * E * 2.059E-3 * Math.sin(2*(phase-sanomaly)-anomaly);
		l += -1.773E-3 * Math.sin(anomaly+2*(phase-node)) - 1.595E-3 * Math.sin(2*(node+phase));
		l += E * 1.22E-3 * Math.sin(4*phase-sanomaly-anomaly) - 1.11E-3 * Math.sin(2*(anomaly+node));
		double longitude = l;
 
		// Let's add nutation here also
		double M1 = (124.90 - 1934.134 * t + 0.002063 * t * t) * DEG_TO_RAD;
		double M2 = (201.11 + 72001.5377 * t + 0.00057 * t * t) * DEG_TO_RAD;
		double d = - .0047785 * Math.sin(M1) - .0003667 * Math.sin(M2);
		longitude += d;
 
		// Get accurate Moon age
		double Psin = LUNAR_CYCLE_DAYS; //XXX Using constant
		moonAge = normalizeRadians((longitude - slongitude) * DEG_TO_RAD) * Psin / TWO_PI;
 
		// Now Moon parallax
		double parallax = .950724 + .051818 * Math.cos(anomaly) + .009531 * Math.cos(2 * phase - anomaly);
		parallax += .007843 * Math.cos(2 * phase) + .002824 * Math.cos(2 * anomaly);
		parallax += 0.000857 * Math.cos(2 * phase + anomaly) + E * .000533 * Math.cos(2 * phase - sanomaly);
		parallax += E * .000401 * Math.cos(2 * phase - anomaly - sanomaly) + E * .00032 * Math.cos(anomaly - sanomaly) - .000271 * Math.cos(phase);
		parallax += -E * .000264 * Math.cos(sanomaly + anomaly) - .000198 * Math.cos(2 * node - anomaly);
		parallax += 1.73E-4 * Math.cos(3 * anomaly) + 1.67E-4 * Math.cos(4*phase-anomaly);
 
		// So Moon distance in Earth radii is, more or less,
		double distance = 1.0 / Math.sin(parallax * DEG_TO_RAD);
 
		// Ecliptic latitude with nodal phase (error<0.01 deg)
		l = 5.128189 * Math.sin(node) + 0.280606 * Math.sin(node + anomaly) + 0.277693 * Math.sin(anomaly - node);
		l += .173238 * Math.sin(2 * phase - node) + .055413 * Math.sin(2 * phase + node - anomaly);
		l += .046272 * Math.sin(2 * phase - node - anomaly) + .032573 * Math.sin(2 * phase + node);
		l += .017198 * Math.sin(2 * anomaly + node) + .009267 * Math.sin(2 * phase + anomaly - node);
		l += .008823 * Math.sin(2 * anomaly - node) + E * .008247 * Math.sin(2 * phase - sanomaly - node) + .004323 * Math.sin(2 * (phase - anomaly) - node);
		l += .0042 * Math.sin(2 * phase + node + anomaly) + E * .003372 * Math.sin(node - sanomaly - 2 * phase);
		l += E * 2.472E-3 * Math.sin(2 * phase + node - sanomaly - anomaly);
		l += E * 2.222E-3 * Math.sin(2 * phase + node - sanomaly);
		l += E * 2.072E-3 * Math.sin(2 * phase - node - sanomaly - anomaly);
		double latitude = l;
 
		return new double[] {longitude, latitude, distance * EARTH_RADIUS / AU, Math.atan(1737.4 / (distance * EARTH_RADIUS))};
	}
 
	private double[] doCalc(double[] pos) {
		// Ecliptic to equatorial coordinates
		double t2 = this.t / 100.0;
		double tmp = t2 * (27.87 + t2 * (5.79 + t2 * 2.45));
		tmp = t2 * (-249.67 + t2 * (-39.05 + t2 * (7.12 + tmp)));
		tmp = t2 * (-1.55 + t2 * (1999.25 + t2 * (-51.38 + tmp)));
		tmp = (t2 * (-4680.93 + tmp)) / 3600.0;
		double angle = (23.4392911111111 + tmp) * DEG_TO_RAD; // obliquity
 
		// Add nutation in obliquity
		double M1 = (124.90 - 1934.134 * t + 0.002063 * t * t) * DEG_TO_RAD;
		double M2 = (201.11 + 72001.5377 * t + 0.00057 * t * t) * DEG_TO_RAD;
		double d = .002558 * Math.cos(M1) - .00015339 * Math.cos(M2);
		angle += d * DEG_TO_RAD;
 
		pos[0] *= DEG_TO_RAD;
		pos[1] *= DEG_TO_RAD;
		double cl = Math.cos(pos[1]);
		double x = pos[2] * Math.cos(pos[0]) * cl;
		double y = pos[2] * Math.sin(pos[0]) * cl;
		double z = pos[2] * Math.sin(pos[1]);
		tmp = y * Math.cos(angle) - z * Math.sin(angle);
		z = y * Math.sin(angle) + z * Math.cos(angle);
		y = tmp;
 
		// Obtain local apparent sidereal time
		double jd0 = Math.floor(jd_UT - 0.5) + 0.5;
		double T0 = (jd0 - J2000) / JULIAN_DAYS_PER_CENTURY;
		double secs = (jd_UT - jd0) * SECONDS_PER_DAY;
		double gmst = (((((-6.2e-6 * T0) + 9.3104e-2) * T0) + 8640184.812866) * T0) + 24110.54841;
		double msday = 1.0 + (((((-1.86e-5 * T0) + 0.186208) * T0) + 8640184.812866) / (SECONDS_PER_DAY * JULIAN_DAYS_PER_CENTURY));
		gmst = (gmst + msday * secs) * (15.0 / 3600.0) * DEG_TO_RAD;
		double lst = gmst + obsLon;
 
		// Obtain topocentric rectangular coordinates
		// Set radiusAU = 0 for geocentric calculations 
		// (rise/set/transit will have no sense in this case)
		double radiusAU = EARTH_RADIUS / AU;
		double correction[] = new double[] {
				radiusAU * Math.cos(obsLat) * Math.cos(lst),
				radiusAU * Math.cos(obsLat) * Math.sin(lst),
				radiusAU * Math.sin(obsLat)};
		double xtopo = x - correction[0];
		double ytopo = y - correction[1];
		double ztopo = z - correction[2];
 
		// Obtain topocentric equatorial coordinates
		double ra = 0.0;
		double dec = PI_OVER_TWO;
		if (ztopo < 0.0)
			dec = -dec;
		if (ytopo != 0.0 || xtopo != 0.0)
		{
			ra = Math.atan2(ytopo, xtopo);
			dec = Math.atan2(ztopo / Math.sqrt(xtopo * xtopo + ytopo * ytopo), 1.0);
		}
		double dist = Math.sqrt(xtopo * xtopo + ytopo * ytopo + ztopo * ztopo);
 
		// Hour angle
		double angh = lst - ra;
 
		// Obtain azimuth and geometric alt
		double sinlat = Math.sin(obsLat); 
		double coslat = Math.cos(obsLat); 
		double sindec = Math.sin(dec), cosdec = Math.cos(dec);
		double h = sinlat * sindec + coslat * cosdec * Math.cos(angh);
		double alt = Math.asin(h);
		double azy = Math.sin(angh);
		double azx = Math.cos(angh) * sinlat - sindec * coslat / cosdec;
		double azi = Math.PI + Math.atan2(azy, azx); // 0 = north
 
		// Get apparent elevation
		if (alt > -3 * DEG_TO_RAD) {
			double r = 0.016667 * DEG_TO_RAD * Math.abs(Math.tan(PI_OVER_TWO - (alt * RAD_TO_DEG +  7.31 / (alt * RAD_TO_DEG + 4.4)) * DEG_TO_RAD));
			double refr = r * ( 0.28 * 1010 / (10 + 273.0)); // Assuming pressure of 1010 mb and T = 10 C
			alt = Math.min(alt + refr, PI_OVER_TWO); // This is not accurate, but acceptable
		}
 
		switch (twilight) {
		case HORIZON_34arcmin:
			// Rise, set, transit times, taking into account Sun/Moon angular radius (pos[3]).
			// The 34' factor is the standard refraction at horizon.
			// Removing angular radius will do calculations for the center of the disk instead
			// of the upper limb.
			tmp = -(34.0 / 60.0) * DEG_TO_RAD - pos[3];
			break;
		case TWILIGHT_CIVIL:
			tmp = -6 * DEG_TO_RAD;
			break;
		case TWILIGHT_NAUTICAL:
			tmp = -12 * DEG_TO_RAD;
			break;
		case TWILIGHT_ASTRONOMICAL:
			tmp = -18 * DEG_TO_RAD;
			break;
		}
 
		// Compute cosine of hour angle
		tmp = (Math.sin(tmp) - Math.sin(obsLat) * Math.sin(dec)) / (Math.cos(obsLat) * Math.cos(dec));
		double celestialHoursToEarthTime = RAD_TO_DAY / SIDEREAL_DAY_LENGTH;
 
		// Make calculations for the meridian
		double transit_time1 = celestialHoursToEarthTime * normalizeRadians(ra - lst);
		double transit_time2 = celestialHoursToEarthTime * (normalizeRadians(ra - lst) - TWO_PI);
		double transit_alt = Math.asin(Math.sin(dec) * Math.sin(obsLat) + Math.cos(dec) * Math.cos(obsLat));
		if (transit_alt > -3 * DEG_TO_RAD) {
			double r = 0.016667 * DEG_TO_RAD * Math.abs(Math.tan(PI_OVER_TWO - (transit_alt * RAD_TO_DEG +  7.31 / (transit_alt * RAD_TO_DEG + 4.4)) * DEG_TO_RAD));
			double refr = r * ( 0.28 * 1010 / (10 + 273.0)); // Assuming pressure of 1010 mb and T = 10 C
			transit_alt = Math.min(transit_alt + refr, PI_OVER_TWO); // This is not accurate, but acceptable
		}
 
		// Obtain the current event in time
		double transit_time = transit_time1;
		double jdToday = Math.floor(jd_UT - 0.5) + 0.5;
		double transitToday2 = Math.floor(jd_UT + transit_time2 - 0.5) + 0.5;
		// Obtain the transit time. Preference should be given to the closest event
		// in time to the current calculation time
		if (jdToday == transitToday2 && Math.abs(transit_time2) < Math.abs(transit_time1)) transit_time = transit_time2;
		double transit = jd_UT + transit_time;
 
		// Make calculations for rise and set
		double rise = -1, set = -1;
		if (Math.abs(tmp) <= 1.0)
		{
			double ang_hor = Math.abs(Math.acos(tmp));
			double rise_time1 = celestialHoursToEarthTime * normalizeRadians(ra - ang_hor - lst);
			double set_time1 = celestialHoursToEarthTime * normalizeRadians(ra + ang_hor - lst);
			double rise_time2 = celestialHoursToEarthTime * (normalizeRadians(ra - ang_hor - lst) - TWO_PI);
			double set_time2 = celestialHoursToEarthTime * (normalizeRadians(ra + ang_hor - lst) - TWO_PI);
 
			// Obtain the current events in time. Preference should be given to the closest event
			// in time to the current calculation time (so that iteration in other method will converge)
			double rise_time = rise_time1;
			double riseToday2 = Math.floor(jd_UT + rise_time2 - 0.5) + 0.5;
			if (jdToday == riseToday2 && Math.abs(rise_time2) < Math.abs(rise_time1)) rise_time = rise_time2;
 
			double set_time = set_time1;
			double setToday2 = Math.floor(jd_UT + set_time2 - 0.5) + 0.5;
			if (jdToday == setToday2 && Math.abs(set_time2) < Math.abs(set_time1)) set_time = set_time2;
			rise = jd_UT + rise_time;
			set = jd_UT + set_time;
		}
 
		//XXX Added 'lst' to the returned array for furthur use to calculate Moon disk values
		return new double[] {azi, alt, rise, set, transit, transit_alt, ra, dec, dist, lst};
	}

	/**
	 * Transforms a Julian day (rise/set/transit fields) to a common date.
	 * @param jd The Julian day.
	 * @return A set of integers: year, month, day, hour, minute, second.
	 * @throws Exception If the input date does not exists.
	 */
	public static int[] getDate(double jd) throws Exception {
		if (jd < 2299160.0 && jd >= 2299150.0)
		{
			throw new Exception("invalid julian day " + jd + ". This date does not exist.");
		}
 
		// The conversion formulas are from Meeus,
		// Chapter 7.
		double Z = Math.floor(jd + 0.5);
		double F = jd + 0.5 - Z;
		double A = Z;
		if (Z >= 2299161D)
		{
			int a = (int) ((Z - 1867216.25) / 36524.25);
			A += 1 + a - a / 4;
		}
		double B = A + 1524;
		int C = (int) ((B - 122.1) / 365.25);
		int D = (int) (C * 365.25);
		int E = (int) ((B - D) / 30.6001);
 
		double exactDay = F + B - D - (int) (30.6001 * E);
		int day = (int) exactDay;
		int month = (E < 14) ? E - 1 : E - 13;
		int year = C - 4715;
		if (month > 2) year--;
		double h = ((exactDay - day) * SECONDS_PER_DAY) / 3600.0;
 
		int hour = (int) h;
		double m = (h - hour) * 60.0;
		int minute = (int) m;
		int second = (int) ((m - minute) * 60.0);
 
		return new int[] {year, month, day, hour, minute, second};
	}
	 
	/**
	 * Returns a date as a string.
	 * @param jd The Juliand day.
	 * @return The String.
	 * @throws Exception If the date does not exists.
	 */
	public static String getDateAsString(double jd) throws Exception {
		if (jd == -1) return "NO RISE/SET/TRANSIT FOR THIS OBSERVER/DATE";
 
		int date[] = SunMoonCalculator.getDate(jd);
		return date[0]+"/"+date[1]+"/"+date[2]+" "+date[3]+":"+date[4]+":"+date[5]+" UT";
	}

	/**
	 * Reduce an angle in radians to the range (0 - 2 Pi).
	 * 
	 * @param r Value in radians.
	 * @return The reduced radian value.
	 */
	public static double normalizeRadians(double r)
	{
		if (r < 0 && r >= -TWO_PI) return r + TWO_PI;
		if (r >= TWO_PI && r < FOUR_PI) return r - TWO_PI;
		if (r >= 0 && r < TWO_PI) return r;
 
		r -= TWO_PI * Math.floor(r * TWO_PI_INVERSE);
		if (r < 0.) r += TWO_PI;
 
		return r;
	}
 
	private double obtainAccurateRiseSetTransit(double riseSetJD, int index, int niter, boolean sun) {
		double step = -1;
		for (int i = 0; i< niter; i++) {
			if (riseSetJD == -1) return riseSetJD; // -1 means no rise/set from that location
			setUTDate(riseSetJD);
			double out[] = null;
			if (sun) {
				out = doCalc(getSun());
			} else {
				getSun();
				out = doCalc(getMoon());
			}
			step = Math.abs(riseSetJD - out[index]);
			riseSetJD = out[index];
		}
		if (step > 1.0 / SECONDS_PER_DAY) return -1; // did not converge => without rise/set/transit in this date
		return riseSetJD;
	}
	
	//XXX Removed: main method
}
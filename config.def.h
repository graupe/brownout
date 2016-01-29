
static float gammar(float percent) { return 0.01*(80 + percent*20); }
static float gammag(float percent) { return 0.01*(30 + percent*20 + 50*exp((percent-1)/4.0)); }
static float gammab(float percent) { return 0.01*( 0 + percent*20 + 80*exp( percent-1)); }

#ifndef SPRING_H
#define SPRING_H

struct Spring {
	int p0, p1; // connected particles
	float k;    // spring constant
	float r;    // rest length
};

#endif

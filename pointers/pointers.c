#include <stdio.h>

typedef struct pass_t {
	int onefish;
	int twofish;
	int redfish;
	int bluefish;
} pass_t;

void pass_by_value(pass_t p) {
	p.onefish = 5;
}

void pass_by_reference(pass_t *p) {
	p->onefish = 5;
}

int main(void) {
	pass_t pass_it;
	pass_it.onefish = 0;
	pass_it.twofish = 1;
	pass_it.redfish = 2;
	pass_it.bluefish = 3;

	pass_by_value(pass_it);
	printf("pass_it.onefish by value: %d\n", pass_it.onefish);

	pass_by_reference(&pass_it);
	printf("pass_it.onefish by reference: %d\n", pass_it.onefish);
}

#ifndef CLEFT_CLICK_H
#define CLEFT_CLICK_H

#include "quantum.h"

#define KEYMAP( \
		                k00, k01, k02, k03, k04, k05, k06,  k50, k51, k52, k53, k54, k55, k56, \
		                k10, k11, k12, k13, k14, k15,       k60, k61, k62, k63, k64, k65, k66, k67, \
		                k20, k21, k22, k23, k24, k25,       k70, k71, k72, k73, k74, k75, k76, \
		                k30, k31, k32, k33, k34, k35,       k80, k81, k82, k83, k84, k85, \
		                k40, k41, k42, k43, k44,            k90, k91, k92, k93, k94 \
		              ) \
{ \
  { k00, k01, k02, k03, k04, k05,   k06,     KC_NO, k50,   k51,   k52, k53, k54, k55, k56 }, \
  { k10, k11, k12, k13, k14, k15,   KC_NO,   k60,   k61,   k62,   k63, k64, k65, k66, k67 }, \
  { k20, k21, k22, k23, k24, k25,   KC_NO,   KC_NO, k70,   k71,   k72, k73, k74, k75, k76 }, \
  { k30, k31, k32, k33, k34, k35,   KC_NO,   KC_NO, KC_NO, k80,   k81, k82, k83, k84, k85 }, \
  { k40, k41, k42, k43, k44, KC_NO, KC_NO,   KC_NO, KC_NO, KC_NO, k90, k91, k92, k93, k94 }, \
}

/*#define KEYMAP( \
	k50, k51, k52, k53, k54, k55, k56, \
	k60, k61, k62, k63, k64, k65, k66, k67, \
	k70, k71, k72, k73, k74, k75, k76, \
	k80, k81, k82, k83, k84, k85, \
	k90, k91, k92, k93, k94 \
	) \
	{ \
  { KC_NO, k50,   k51,   k52, k53, k54, k55, k56 }, \
  { k60,   k61,   k62,   k63, k64, k65, k66, k67 }, \
  { KC_NO, k70,   k71,   k72, k73, k74, k75, k76 }, \
  { KC_NO, KC_NO, k80,   k81, k82, k83, k84, k85 }, \
  { KC_NO, KC_NO, KC_NO, k90, k91, k92, k93, k94 }, \
}*/

#endif

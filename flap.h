#pragma once
#include <SDL.h>
#include <iostream>
#include "renderstate.h"

typedef struct FlapAwareness {
	int flapInWorldX; /* where in the X range of flaps am I in the world? origin center being 0 */
	int flapInWorldY; /* where in the Y range of flaps am I in the world? origin middle being 0 */
	int myGridLeftX;  /* which is the absolute world grid x-coordinate of my first (left) column? */
	int myGridToopY;  /* which is the absolute worls grid y-coordinate of my first (top) row? */
	
	int numGridUnitsWide;
	int numGridUnitsHigh;
	int localDrawingOffsetX; /* where to start drawing on the flap, left */
	int localDrawingOffsetY; /* where to start drawing on the flap, top */
} FlapAwareness;

void PopulateFlap(int flapIndex, int flapInWorldX, int flapInWorldY);

FlapAwareness* _FigureOutAwarenessFor(int flapIndex, int flapInWorldX, int flapInWorldY);

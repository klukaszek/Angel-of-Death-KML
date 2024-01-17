#include "KMLHelpersA2.h"

//Helper functions for populating Points tab in Python GUI
char *getPointName(void *ppm);
void setPointName(void *ppm, char *str);
double getPointLong(void *ppm);
double getPointLat(void *ppm);
double getPointAlt(void *ppm);

//Helper functions for populating Paths tab in Python GUI
char *getPathName(void *ppm);
void setPathName(void *ppm, char *str);
double getPathLength(void *ppm);
bool checkPathLoop(void *ppm, double delta);

//Helper functions for populating Styles tab in Python GUI
char *getStyleColour(void *s);
void setStyleColour(void *s, char *str);
int getStyleWidth(void *s);
void setStyleWidth(void *s, int w);
int getStyleFill(void *s);
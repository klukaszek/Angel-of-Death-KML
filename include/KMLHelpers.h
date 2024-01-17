//Kyle Lukaszek
//ID: 1113798
//Course CIS 2750 F22
//Assignment 2

#include "KMLParser.h"
#include <ctype.h>

//Generates all structs from KML nodes and places them in their respective lists within the KML struct
void parseNodes(KML *kml, xmlDoc *doc, xmlNode *node);

//allocates all memory needed for an XMLNamespace struct and returns it.
XMLNamespace* createNamespace(char *prefix, char *value);

//Create KMLElement struct and set name string, and value string
KMLElement* createElement(char *name, char *value);

//Create 2d array of strings that contain the data for each set of coordinates
char** parseCoords(char *coords, int *count);

//Create Coordinate struct with a string of longitude, latitude, and (sometimes) altitude coordinates
Coordinate* createCoordinate(char *coords);

//Create Point struct with Coordinate struct and list for otherElements, then returns it
Point* createPoint(xmlDoc *doc, xmlNode *node);

//Create PointPlacemark struct with name (optional), line, leaves otherElement list empty so that createPlacemark() can attach it at the very end.
PointPlacemark* createPointPlacemark(char *name, Point *point);

//Create Line struct with 2 lists for coordinates and otherElements and returns it
Line* createLine(xmlDoc *doc, xmlNode *node);

//Create PathPlacemark struct with name (optional), line, leaves otherElement list empty so that createPlacemark() can attach it at the very end.
PathPlacemark* createPathPlacemark(char *name, Line *line);

//This function constructs a PointPlacemark struct or a PathPlacemark struct and returns is a void* data type. Once returned, the type is indicated by the passed "int *return_type".
//return_type value of 0 = NULL, return_type value of 1 = PointPlacemark, return_type value of 2 = PathPlacemark
void *createPlacemark(xmlDoc *doc, xmlNode *node, int *return_type);

//get string that represent id attribute for Style and StyleMap nodes
char *getIDStr(xmlNode *node);

//parses linestyle node data into Style struct
void parseLineStyle(xmlDoc *doc, xmlNode *node, Style *style);

Style* createStyle(xmlDoc *doc, xmlNode *node);

//Parses pair node data into stylemap struct
void parsePair(xmlDoc *doc, xmlNode *node, StyleMap *stylemap, int pair_num);

StyleMap* createStyleMap(xmlDoc *doc, xmlNode *node);

/* ******************************* Misc Helpers *************************** */

//Deletes a line struct and lists that exist within it
void deleteLine(void* data);
char* lineToString(void* data);
int compareLines(const void *first, const void *second);

//checks if given string is empty
int strIsEmpty(char *str);
//Kyle Lukaszek
//ID: 1113798
//Course CIS 2750 F22
//Assignment 2

#include "KMLHelpers.h"

// Validates xmlDoc* against .xsd file and returns validation status (0 = valid, >0 = invalid, -1 = error)
// based on the example on http://knol2share.blogspot.com/2009/05/validate-xml-against-xsd-in-c.html
int validateXmlDoc(xmlDoc *doc, const char *schemaFile);

//creates an xml tree from a kml struct
xmlDoc *KMLtoXmlDoc(const KML *kml);

//write namespaces to node so they can be written to file
bool writeNamespaces(const KML *doc, xmlNode *root_node);

//write pointPlacemarks to nodes so they can be written to file
bool writePointPlacemarks(const KML *doc, xmlNode *root_node);

//write pathPlacemarks to nodes so they can be written to file
bool writePathPlacemarks(const KML *doc, xmlNode *root_node);

//write styles to nodes so they can be written to file
bool writeStyles(const KML *doc, xmlNode *root_node);

//write stylemaps to nodes so they can be written to file
bool writeStyleMaps(const KML *doc, xmlNode *root_node);

//checks if a KML struct has a placemark with a stylemap url element
bool placemarkContainsStyleMap(const KML *doc);

//returns distance between two coordinates in metres
double haversine(double lon1, double lat1, double lon2, double lat2);

//empty function to pass initializeList() as a delete function so we do not free the contents of the list, and just free the list struct itself
void deleteDataDummy();
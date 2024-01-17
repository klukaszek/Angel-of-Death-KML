//Kyle Lukaszek
//ID: 1113798
//Course CIS 2750 F22
//Assignment 2

#include "KMLHelpersA2.h"

// Validates xmlDoc* against .xsd file and returns validation status (0 = valid, >0 = invalid, -1 = error)
// loosely based on the example on http://knol2share.blogspot.com/2009/05/validate-xml-against-xsd-in-c.html
// I added some things from libxml2's documentation since I was having problems getting this to work without any memory leaks
int validateXmlDoc(xmlDoc *doc, const char *schemaFile)
{
    if (doc == NULL) return -1;
    if (schemaFile == NULL) return -1;

    xmlDoc *schemaDoc = xmlReadFile(schemaFile, NULL, 0);
    // if schema file is not validated by libxml2, return null
    if (schemaDoc == NULL)
    {
        /*free the document */
        xmlFreeDoc(schemaDoc);

        /*
         *Free the global variables that may
         *have been allocated by the parser.
         */
        xmlCleanupParser();
        return -1;
    }

    xmlSchemaParserCtxtPtr parserCtxt = xmlSchemaNewDocParserCtxt(schemaDoc);
    if (parserCtxt == NULL)
    {
        /*free the document */
        xmlFreeDoc(schemaDoc);
        xmlSchemaFreeParserCtxt(parserCtxt);
        xmlSchemaCleanupTypes();
        /*
         *Free the global variables that may
         *have been allocated by the parser.
         */
        xmlCleanupParser();
        return -1;
    }

    xmlSchemaPtr schema = xmlSchemaParse(parserCtxt);
    if (schema == NULL)
    {
        /*free the document */
        xmlFreeDoc(schemaDoc);
        xmlSchemaFreeParserCtxt(parserCtxt);
        xmlSchemaFree(schema);
        xmlSchemaCleanupTypes();

        /*
         *Free the global variables that may
         *have been allocated by the parser.
         */
        xmlCleanupParser();
        return -1;
    }

    // make valid schema Ctxt pointer
    xmlSchemaValidCtxtPtr validCtxt = xmlSchemaNewValidCtxt(schema);
    if (validCtxt == NULL)
    {
        /*free the document */
        xmlFreeDoc(schemaDoc);
        xmlSchemaFreeParserCtxt(parserCtxt);
        xmlSchemaFree(schema);
        xmlSchemaFreeValidCtxt(validCtxt);
        xmlSchemaCleanupTypes();
        xmlCleanupParser();

        /*
         *Free the global variables that may
         *have been allocated by the parser.
         */
        xmlCleanupParser();
        return -1;
    }

    // Validate xml file against specific .xsd schemafile
    int valid = xmlSchemaValidateDoc(validCtxt, doc);

    //no memory leaks
    xmlFreeDoc(schemaDoc);
    xmlSchemaFreeParserCtxt(parserCtxt);
    xmlSchemaFree(schema);
    xmlSchemaFreeValidCtxt(validCtxt);
    xmlSchemaCleanupTypes();
    xmlCleanupParser();

    return valid;
}

//creates an xml tree from a kml struct
xmlDoc *KMLtoXmlDoc(const KML *doc)
{
    if(doc == NULL) return NULL;

    xmlDoc *new_doc = NULL;
    xmlNode *root_node = NULL;
    int success;

    /* 
     * Creates a new document, creates a node, and set it as a root node
     */
    new_doc = xmlNewDoc(BAD_CAST "1.0");
    //set ns to null since we will add namespaces after
    root_node = xmlNewNode(NULL, BAD_CAST "kml");   
    xmlDocSetRootElement(new_doc, root_node);

    xmlNode *node = root_node;

    //write namespaces to tree
    success = writeNamespaces(doc, node);
    if (success == false)
    {
        fprintf(stderr, "failed to write namespaces\n");
        xmlFreeDoc(new_doc);
        xmlCleanupParser();

        return NULL;
    }

    //these are used to check if we need a "Document" element to be the root
    int placemark_count = getLength(doc->pointPlacemarks) + getLength(doc->pathPlacemarks);
    int style_count = getLength(doc->styles);
    int stylemap_count = getLength(doc->styleMaps);
    
    //check if we need the "Document" element
    if(placemark_count == 1 && style_count == 0) node = root_node;
    else if (placemark_count > 1 || stylemap_count > 0) //maybe use helper function i wrote for this
    {
        xmlNode *doc_node = xmlNewNode(root_node->ns, BAD_CAST "Document");
        xmlAddChild(root_node, doc_node);
        //set root_node to doc_node so elements are written as children of "Document"
        node = doc_node;
    }

    //write stylemaps to tree
    success = writeStyleMaps(doc, node);
    if (success == false)
    {
        fprintf(stderr, "failed to write styles\n");
        xmlFreeDoc(new_doc);
        xmlCleanupParser();

        return NULL;
    }

    //write styles to tree
    success = writeStyles(doc, node);
    if (success == false)
    {
        fprintf(stderr, "failed to write styles\n");
        xmlFreeDoc(new_doc);
        xmlCleanupParser();

        return NULL;
    }

    //write point placemarks to tree
    success = writePointPlacemarks(doc, node);
    if (success == false)
    {
        fprintf(stderr, "failed to write Point Placemarks\n");
        xmlFreeDoc(new_doc);
        xmlCleanupParser();

        return NULL;
    }

    //write path placemarks to tree
    success = writePathPlacemarks(doc, node);
    if (success == false)
    {
        fprintf(stderr, "failed to write Path Placemarks\n");
        xmlFreeDoc(new_doc);
        xmlCleanupParser();

        return NULL;
    }

    return new_doc;
}

//write namespaces to specific node
bool writeNamespaces(const KML *doc, xmlNode *root_node)
{
    if (doc == NULL) return false;
    if (root_node == NULL) return false;
 
    ListIterator iter = createIterator(doc->namespaces);
    XMLNamespace *ns = nextElement(&iter);
    xmlNs *namespace;
    int c = 0;

    //iterate through XMLNamespace list and add namespaces to root node
    while(ns != NULL)
    {
        //make sure namespace is up to spec from KMLParser.h
        if(ns->value == NULL) return false;
        if(strcmp(ns->value, "") == 0) return false;

        //check if namespace has prefix
        if(ns->prefix != NULL)
        {
            namespace = xmlNewNs(root_node, BAD_CAST ns->value, BAD_CAST ns->prefix);
        }
        //else, just write href
        else
        {
            namespace = xmlNewNs(root_node, BAD_CAST ns->value, NULL);
        }

        //set namespace of root_node
        if(c == 0)
        {
            xmlSetNs(root_node, namespace);
            c = 1;
        }

        ns = nextElement(&iter);
    }

    return true;
}

//write pointPlacemarks to nodes so they can be written to file
bool writePointPlacemarks(const KML *doc, xmlNode *root_node)
{
    if (doc == NULL) return false;
    if (root_node == NULL) return false;
 
    ListIterator iter = createIterator(doc->pointPlacemarks);
    PointPlacemark *pp = nextElement(&iter);
    ListIterator elemIter;
    KMLElement *elem;
    char buff[256];

    //iterate through point placemark list
    while(pp != NULL)
    {
        //check if point placemark is up to spec
        if(pp->point == NULL) return false;
        if(pp->otherElements == NULL) return false;
        Point *point = pp->point;
        if (point->coordinate == NULL) return false;
        if (point->otherElements == NULL) return false;

        //create placemark parent node
        xmlNode *node = xmlNewChild(root_node, root_node->ns, BAD_CAST "Placemark", NULL);

        //add name node if placemark has a name, this includes empty string. if name is null, we don't need a name node
        if(pp->name != NULL && strcmp(pp->name, "") != 0)
        {
            xmlNewChild(node, node->ns, BAD_CAST "name", BAD_CAST pp->name);
        }

        //if there are other elements, get them and convert them to nodes
        if (getLength(pp->otherElements) > 0)
        {
            elemIter = createIterator(pp->otherElements);
            elem = nextElement(&elemIter);

            while (elem != NULL)
            {
                //make sure KMLElements are up to spec
                if(elem->name == NULL || elem->value == NULL) return false;
                if(elem->name[0] == '\0' || elem->value[0] == '\0') return false;
                xmlNewChild(node, node->ns, BAD_CAST elem->name, BAD_CAST elem->value);
                elem = nextElement(&elemIter);
            }
        }

        // get point node
        xmlNode *pointNode = xmlNewChild(node, node->ns, BAD_CAST "Point", NULL);

        // create other element nodes for point node
        if (getLength(point->otherElements) > 0)
        {
            elemIter = createIterator(point->otherElements);
            elem = nextElement(&elemIter);

            while (elem != NULL)
            {
                //make sure KMLElements are up to spec
                if(elem->name == NULL || elem->value == NULL) return false;
                if(elem->name[0] == '\0' || elem->value[0] == '\0') return false;
                xmlNewChild(pointNode, pointNode->ns, BAD_CAST elem->name, BAD_CAST elem->value);
                elem = nextElement(&elemIter);
            }
        }

        //this is for creating the coordinate node
        // check if we include altitude and then write coordinates to buff
        if (point->coordinate->altitude == DBL_MAX)
        {
            sprintf(buff, "%f,%f", point->coordinate->longitude, point->coordinate->latitude);
        }
        else
        {
            sprintf(buff, "%f,%f,%f", point->coordinate->longitude, point->coordinate->latitude, point->coordinate->altitude);
        }

        // create coordinate node
        xmlNewChild(pointNode, pointNode->ns, BAD_CAST "coordinates", BAD_CAST buff);

        //get next point placemark
        pp = nextElement(&iter);
    }

    return true;
}

//write pathPlacemarks to nodes so they can be written to file
bool writePathPlacemarks(const KML *doc, xmlNode *root_node)
{
    if (doc == NULL) return false;
    if (root_node == NULL) return false;
 
    ListIterator iter = createIterator(doc->pathPlacemarks);
    PathPlacemark *pp = nextElement(&iter);
    ListIterator elemIter;
    KMLElement *elem;
    char buff[256];

    //iterate through path placemark list
    while(pp != NULL)
    {
        //check if path placemark is up to spec from KMLParser.h
        if(pp->pathData == NULL) return false;
        Line *pathData = pp->pathData;
        if(pp->otherElements == NULL) return false;
        if(pathData->coordinates == NULL) return false;
        if(pathData->otherElements == NULL) return false;
        if(getLength(pathData->coordinates) < 2) return false;

        //create placemark parent node
        xmlNode *node = xmlNewChild(root_node, root_node->ns, BAD_CAST "Placemark", NULL);

        //add name node if placemark has a name, this includes empty string. if name is null, we don't need a name node
        if(pp->name != NULL && strcmp(pp->name, "") != 0)
        {
            xmlNewChild(node, node->ns, BAD_CAST "name", BAD_CAST pp->name);
        }

        //if there are other elements, get them and convert them to nodes
        if (getLength(pp->otherElements) > 0)
        {
            elemIter = createIterator(pp->otherElements);
            elem = nextElement(&elemIter);

            while (elem != NULL)
            {
                //make sure KMLElements are up to spec
                if(elem->name == NULL || elem->value == NULL) return false;
                if(elem->name[0] == '\0' || elem->value[0] == '\0') return false;
                xmlNewChild(node, node->ns, BAD_CAST elem->name, BAD_CAST elem->value);
                elem = nextElement(&elemIter);
            }
        }

        // create LineString node
        xmlNode *lineNode = xmlNewChild(node, node->ns, BAD_CAST "LineString", NULL);

        // create other element nodes for LineString node
        if (getLength(pathData->otherElements) > 0)
        {
            elemIter = createIterator(pathData->otherElements);
            elem = nextElement(&elemIter);

            // iterate through other elements and attach them as children nodes
            while (elem != NULL)
            {
                //make sure KMLElements are up to spec
                if(elem->name == NULL || elem->value == NULL) return false;
                if(elem->name[0] == '\0' || elem->value[0] == '\0') return false;
                xmlNewChild(lineNode, lineNode->ns, BAD_CAST elem->name, BAD_CAST elem->value);
                elem = nextElement(&elemIter);
            }
        }

        // create coordinate node
        ListIterator coordIter = createIterator(pathData->coordinates);
        Coordinate *coord = nextElement(&coordIter);
        // char * coordinate_string = malloc(sizeof(char) * 4096);
        int c = 0;
        // creates coordinate node
        xmlNode *coordNode = xmlNewNode(lineNode->ns, BAD_CAST "coordinates");

        // iterate through all coordinates and add them to coord node as child text
        while (coord != NULL)
        {
            // check if we include altitude and then write coordinates to buff
            if (coord->altitude == DBL_MAX)
            {
                if (c == 0)
                    sprintf(buff, "%f,%f", coord->longitude, coord->latitude);
                else
                    sprintf(buff, " %f,%f", coord->longitude, coord->latitude);
            }
            else
            {
                if (c == 0)
                    sprintf(buff, "%f,%f,%f", coord->longitude, coord->latitude, coord->altitude);
                else
                    sprintf(buff, " %f,%f,%f", coord->longitude, coord->latitude, coord->altitude);
            }

            // add text to coordNode
            xmlAddChild(coordNode, xmlNewText(BAD_CAST buff));

            coord = nextElement(&coordIter);
            c = 1;
        }

        // attach coordNode to lineNode as child
        xmlAddChild(lineNode, coordNode);

        // get next path placemark
        pp = nextElement(&iter);
    }

    return true;  
}

//write styles to nodes so they can be written to file
bool writeStyles(const KML *doc, xmlNode *root_node)
{
    if (doc == NULL) return false;
    if (root_node == NULL) return false;

    ListIterator iter = createIterator(doc->styles);
    Style *style = nextElement(&iter);
    char buff[256];

    //iterate through style list
    while(style != NULL)
    {
        //check if style is valid according to KMLParser.h specss
        if(style->id == NULL || style->colour == NULL) return false;
        if(strcmp(style->id, "") == 0 || strcmp(style->colour, "") == 0) return false;

        //add style node with id prop
        xmlNode *styleNode = xmlNewChild(root_node, root_node->ns, BAD_CAST "Style", NULL);
        xmlNewProp(styleNode, BAD_CAST "id", BAD_CAST style->id);

        //add linestyle node
        xmlNode *lineNode = xmlNewChild(styleNode, styleNode->ns, BAD_CAST "LineStyle", NULL);
        xmlNewChild(lineNode, lineNode->ns, BAD_CAST "color", BAD_CAST style->colour);

        //check if there is a width child node that has to be added
        if(style->width != -1)
        {
            sprintf(buff, "%d", style->width);
            xmlNewChild(lineNode, lineNode->ns, BAD_CAST "width", BAD_CAST buff);
        }

        //check if a polystyle node is needed, if so add it and the fill
        if(style->fill != -1)
        {
            xmlNode *polyNode = xmlNewChild(styleNode, styleNode->ns, BAD_CAST "PolyStyle", NULL);
            sprintf(buff, "%d", style->fill);
            xmlNewChild(polyNode, polyNode->ns, BAD_CAST "fill", BAD_CAST buff);
        }

        style = nextElement(&iter);
    }

    return true;
}

//write stylemaps to nodes so they can be written to file
bool writeStyleMaps(const KML *doc, xmlNode *root_node)
{
    if (doc == NULL) return false;
    if (root_node == NULL) return false;

    ListIterator iter = createIterator(doc->styleMaps);
    StyleMap *stylemap = nextElement(&iter);

    //iterate through stylemap list
    while(stylemap != NULL)
    {
        //make sure stylemap is up to spec
        if(stylemap->id == NULL) return false;
        if(strcmp(stylemap->id, "") == 0) return false;

        //create stylemap node and set id as prop
        xmlNode * styleMapNode = xmlNewChild(root_node, root_node->ns, BAD_CAST "StyleMap", NULL);
        xmlNewProp(styleMapNode, BAD_CAST "id", BAD_CAST stylemap->id);

        //create pair 1 node and set key1 and url1
        xmlNode *pair1 = xmlNewChild(styleMapNode, styleMapNode->ns, BAD_CAST "Pair", NULL);
        xmlNewChild(pair1, pair1->ns, BAD_CAST "key", BAD_CAST stylemap->key1);
        xmlNewChild(pair1, pair1->ns, BAD_CAST "styleUrl", BAD_CAST stylemap->url1);

        //create pair 2 node and set key2 and url2
        xmlNode *pair2 = xmlNewChild(styleMapNode, styleMapNode->ns, BAD_CAST "Pair", NULL);
        xmlNewChild(pair2, pair2->ns, BAD_CAST "key", BAD_CAST stylemap->key2);
        xmlNewChild(pair2, pair2->ns, BAD_CAST "styleUrl", BAD_CAST stylemap->url2);

        stylemap = nextElement(&iter);
    }

    return true;
}

//checks if a KML struct has a placemark with a stylemap url element
//this is called from writeKML which assumes doc has already been validated so we know that there are no illegal elements within the struct
bool placemarkContainsStyleMap(const KML *doc)
{
    if (doc == NULL) return false;

    ListIterator iter = createIterator(doc->pointPlacemarks);
    PointPlacemark *point = nextElement(&iter);

    while(point != NULL)
    {
        ListIterator elemIter = createIterator(point->otherElements);
        KMLElement *elem = nextElement(&elemIter);

        while(elem != NULL)
        {
            if(strcmp(elem->name, "styleUrl") == 0) return true;
            elem = nextElement(&iter);
        }
        
        point = nextElement(&iter);
    }

    return false;
}

//returns distance between two coordinates in metres
//referenced from link: https://www.movable-type.co.uk/scripts/latlong.html
double haversine(double lon1, double lat1, double lon2, double lat2)
{
    /*    
    Haversine formula:	
                a = sin²(Δφ/2) + cos φ1 ⋅ cos φ2 ⋅ sin²(Δλ/2)
                c = 2 ⋅ atan2( √a, √(1−a) )
     return ->  d = R ⋅ c
    */

    //earth's radius in metres
    const double R = 6371e3;

    //convert everything to radians
    double lat_rad1 = lat1 * (M_PI/180);
    double lat_rad2 = lat2 * (M_PI/180);
    double delta_lat_rad = (lat2-lat1) * (M_PI/180);
    double delta_lon_rad = (lon2-lon1) * (M_PI/180);
    
    //square of half chord length between points
    double a = (sin(delta_lat_rad/2) * sin(delta_lat_rad/2)) +
          cos(lat_rad1) * cos(lat_rad2) *
          (sin(delta_lon_rad/2) * sin(delta_lon_rad/2));
    
    //angular distance
    double c = 2 * atan2(sqrt(a), sqrt(1-a));

    //calculate path distance in metres
    double d = R * c;

    return d;
}

//empty function to pass initializeList() as a delete function so we do not free the contents of the list, and just free the list struct itself
void deleteDataDummy()
{
    return;
}
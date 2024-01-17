// Kyle Lukaszek
// ID: 1113798
// Course CIS 2750 F22
// Assignment 2

//includes KMLParser.h, KMLHelpers.h, as well as KMLHelpersA2.h
#include "KMLHelpersA3.h" 

/* ******************************* A1 functions *************************** */
/** Function to create an KML object based on the contents of an KML file.
 *@pre File name cannot be an empty string or NULL.
       File represented by this name must exist and must be readable.
 *@post KML has not been modified in any way
        Also, either:
        A valid KML struct has been created and its address was returned
        or
        An error occurred, and NULL was returned
 *@return the pinter to the new struct or NULL
 *@param fileName - a string containing the name of the KML file
**/
KML *createKML(const char *filename)
{

    if (filename == NULL)
        return NULL;

    //create xmlDoc data type to try and parse file.
    xmlDoc *doc = NULL;

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    // parse kml file and get DOM
    doc = xmlReadFile(filename, NULL, 0);
    if (doc == NULL)
    {
        //free doc on failure
        xmlFreeDoc(doc);

        //free any memory that has been allocated by xml parser
        xmlCleanupParser();

        //printf("error: could not parse file %s\n", filename);

        return(NULL);
    }

    // initialize kml struct
    KML *kml = malloc(sizeof(KML));

    // initialize all lists that are contained within a KML structure (see KMLParser.h)
    kml->namespaces = initializeList(&XMLNamespaceToString, &deleteXMLNamespace, &compareXMLNamespace);
    kml->pointPlacemarks = initializeList(&pointPlacemarkToString, &deletePointPlacemark, &comparePointPlacemarks);
    kml->pathPlacemarks = initializeList(&pathPlacemarkToString, &deletePathPlacemark, &comparePathPlacemarks);
    kml->styles = initializeList(&styleToString, &deleteStyle, &compareStyles);
    kml->styleMaps = initializeList(&styleMapToString, &deleteStyleMap, &compareStyleMaps);

    //get root_element from xmlDoc
    xmlNode *root_element = xmlDocGetRootElement(doc);

    //make sure the xmlDoc has a root xmlNode
    if(root_element == NULL)
    {
        //free doc on failure
        xmlFreeDoc(doc);

        //free any memory that has been allocated by xml parser
        xmlCleanupParser();
    }

    //Generates all structs and puts them in their respective lists
    parseNodes(kml, doc, root_element);

    //frees xmlDoc
    xmlFreeDoc(doc);

    /*
    *Free the global variables that may
    *have been allocated by the parser.
    */
    xmlCleanupParser();

    return kml;
}

/** Function to create a string representation of an KML object.
 *@pre KML object exists, is not NULL, and is valid
 *@post KML has not been modified in any way, and a string representing the KML contents has been created
 *@return a string contaning a humanly readable representation of an KML object
 *@param obj - a pointer to an KML struct
**/
char* KMLToString(const KML *doc)
{
    if(doc == NULL)
    {
        return NULL;
    }

    //to get strlen i just hovered my cursor over the string in VSCode
    int len = 99;
    char *str = (char *)malloc(sizeof(char) * (len+1));

    sprintf(str, "KML Struct:\n\tNamespaces: %d\n\tPoint Placemarks: %d\n\tPath Placemarks: %d\n\tStyles: %d\n\tStyle Maps: %d", getNumXMLNamespaces(doc), getNumPoints(doc), getNumPaths(doc), getNumStyles(doc), getNumStyleMaps(doc));

    return str;
}

/** Function to delete doc content and free all the memory.
 *@pre KML object exists, is not NULL, and has not been freed
 *@post KML object had been freed
 *@return none
 *@param obj - a pointer to an KML struct
**/
void deleteKML(KML* doc)
{
    if(doc == NULL)
    {
        return;
    }

    //free namespaces
    if(getLength(doc->namespaces) > 0) freeList(doc->namespaces);
    else free(doc->namespaces);

    //free point placemarks
    if(getLength(doc->pointPlacemarks) > 0) freeList(doc->pointPlacemarks);
    else free(doc->pointPlacemarks);

    //free path placemarks
    if(getLength(doc->pathPlacemarks) > 0) freeList(doc->pathPlacemarks);
    else free(doc->pathPlacemarks);

    //free styles
    if(getLength(doc->styles) > 0) freeList(doc->styles);
    else free(doc->styles);

    //free stylemaps
    if(getLength(doc->styleMaps) > 0) freeList(doc->styleMaps);
    else free(doc->styleMaps);

    free(doc);
}

//Total number of "point" Placemarks in the KML file - i.e. the total number of PointPlacemarks
int getNumPoints(const KML* doc)
{
    if(doc == NULL)
    {
        return 0;
    }

    int count = 0;
    ListIterator pointIter = createIterator(doc->pointPlacemarks);
    void *elem;

    elem = nextElement(&pointIter);
    while(elem != NULL)
    {
        count++;
        elem = nextElement(&pointIter);
    }

    return count;
}

//Total number of "path" Placemarks in the KML file - i.e. the total number of PathPlacemarks
int getNumPaths(const KML* doc)
{
    if(doc == NULL)
    {
        return 0;
    }

    int count = 0;
    ListIterator pathIter = createIterator(doc->pathPlacemarks);
    void *elem;

    elem = nextElement(&pathIter);
    while(elem != NULL)
    {
        count++;
        elem = nextElement(&pathIter);
    }

    return count;
}

//Total number of KMLElements in the document DO THIS LATER NOT COMPLETE YET
int getNumKMLElements(const KML* doc)
{
    if(doc == NULL)
    {
        return 0;
    }

    int count = 0;
    
    //iterate through path placemarks
    ListIterator pathIter = createIterator(doc->pathPlacemarks);
    PathPlacemark *path;
    path = (PathPlacemark *)nextElement(&pathIter);
    while(path != NULL)
    {
        ListIterator elemIter = createIterator(path->otherElements);

        //Get number of KMLElements in pointplacemark struct
        KMLElement *elem = (KMLElement *)nextElement(&elemIter);
        while(elem != NULL)
        {
            count++;
            elem = (KMLElement *)nextElement(&elemIter);
        }

        //Get number of KMLElements in line struct of path placemark
        Line *line = path->pathData;
        elemIter = createIterator(line->otherElements);
        elem = (KMLElement *)nextElement(&elemIter);
        while(elem != NULL)
        {
            count++;
            elem = (KMLElement *)nextElement(&elemIter);
        }

        path = (PathPlacemark *)nextElement(&pathIter);
    }

    //iterate through pointplacemarks
    ListIterator pointIter = createIterator(doc->pointPlacemarks);
    PointPlacemark *pointmark;
    pointmark = (PointPlacemark *)nextElement(&pointIter);
    while(pointmark != NULL)
    {
        ListIterator elemIter = createIterator(pointmark->otherElements);

        //Get number of KMLElements in pointplacemark struct
        KMLElement *elem = (KMLElement *)nextElement(&elemIter);
        while(elem != NULL)
        {
            count++;
            elem = (KMLElement *)nextElement(&elemIter);
        }

        //Get number of KMLElements in point struct of pointplacemark
        Point *p = pointmark->point;
        elemIter = createIterator(p->otherElements);
        elem = (KMLElement *)nextElement(&elemIter);
        while(elem != NULL)
        {
            count++;
            elem = (KMLElement *)nextElement(&elemIter);
        }

        pointmark = (PointPlacemark *)nextElement(&pointIter);
    }
 
    return count;
}

//Total number of XMLNamespaces in the document
int getNumXMLNamespaces(const KML* doc)
{
    if(doc == NULL)
    {
        return 0;
    }

    int count = 0;
    ListIterator nsIter = createIterator(doc->namespaces);
    void *elem;

    elem = nextElement(&nsIter);
    while(elem != NULL)
    {
        count++;
        elem = nextElement(&nsIter);
    }

    return count;
}

//Total number of Styles in the document
int getNumStyles(const KML* doc)
{
    if(doc == NULL)
    {
        return 0;
    }

    int count = 0;
    ListIterator styleIter = createIterator(doc->styles);
    void *elem;

    elem = nextElement(&styleIter);
    while(elem != NULL)
    {
        count++;
        elem = nextElement(&styleIter);
    }

    return count;
}

//Total number of StyleMaps in the document
int getNumStyleMaps(const KML* doc)
{
    if(doc == NULL)
    {
        return 0;
    }

    int count = 0;
    ListIterator smIter = createIterator(doc->styleMaps);
    void *elem;

    elem = nextElement(&smIter);
    while(elem != NULL)
    {
        count++;
        elem = nextElement(&smIter);
    }

    return count;
}

// Function that returns a PointPlacemark with the given name.  If more than one exists, return the first one.  
// Return NULL if the PointPlacemark does not exist
PointPlacemark* getPointPlacemark(const KML* doc, char* name)
{
    if(doc == NULL) return NULL;
    if(name == NULL) return NULL;

    PointPlacemark *mark = NULL;
    ListIterator iter = createIterator(doc->pointPlacemarks);

    mark = (PointPlacemark *)nextElement(&iter);

    //iterate through pointplacemark list
    while(mark != NULL)
    {
        //return first pointplacemark with matching name
        if(mark->name != NULL)
        {
            if(strcmp(mark->name, name) == 0) return mark;
        }

        mark = (PointPlacemark *)nextElement(&iter);
    }

    return mark;
}

// Function that returns a PathPlacemark with the given name.  If more than one exists, return the first one. 
// Return NULL if the PathPlacemark does not exist 
PathPlacemark* getPathPlacemark(const KML* doc, char* name)
{
    if(doc == NULL) return NULL;
    if(name == NULL) return NULL;

    PathPlacemark *mark = NULL;
    ListIterator iter = createIterator(doc->pathPlacemarks);

    mark = (PathPlacemark *)nextElement(&iter);

    //iterate through pathplacemark list
    while(mark != NULL)
    {
        if(mark->name != NULL)
        {
            //return first pathplacemark with matching name
            if(strcmp(mark->name, name) == 0) return mark;
        }

        mark = (PathPlacemark *)nextElement(&iter);
    }

    return mark;
}

/** Function that returns a StyelMap associated with the provided PathPlacemark.  If more than one exists, return the first one.  
 *@pre 
    - KML object exists, is not NULL, and has not been freed
    - ppm exists, is not NULL, and has not been freed
 *@post 
    - KML object has not been modified in any way
    - ppm has not been modified in any way
 *@return returns a struct representing the StyleMap from the PathPlacemark.  
    - a pointer to a matching struct if the element was found
    - NULL otherwise
 *@param 
    - doc - a pointer to an KML struct
    - ppm - the PathPlacemark whose StyleMap we want to get
*/
StyleMap* getMapFromPath(const KML *doc, const PathPlacemark *ppm)
{
    if(doc == NULL) return NULL;
    if(ppm == NULL) return NULL;

    KMLElement *elem = NULL;
    ListIterator iter = createIterator(ppm->otherElements);

    char *id = NULL;

    int found = false;

    elem = (KMLElement *)nextElement(&iter);

    //iterate through other elements to find the style url
    while(elem != NULL && found == false)
    {
        //find id of styleUrl KMLElement
        if(elem->name != NULL)
        {
            if(strcmp(elem->name, "styleUrl") == 0)
            {
                if(elem->value != NULL)
                {
                    id = elem->value;
                    //shift the id pointer to remove the leading "#" from the PathPlacemark styleUrl
                    id++;
                    found = true;
                }
            }
        }

        //iterate only if id has not been found
        if(found != true) elem = (KMLElement *)nextElement(&iter);
    }

    //if no styleUrl is found, return NULL
    if(id == NULL) return NULL;

    StyleMap *map = NULL;
    iter = createIterator(doc->styleMaps);  
    map = (StyleMap *)nextElement(&iter);

    //iterate through stylemap list other
    while(map != NULL)
    {
        
        //if the stylemap id matches, return stylemap
        if(map->id != NULL)
        {
            if(strcmp(map->id, id) == 0) return map;
        }

        map = (StyleMap *)nextElement(&iter);
    }

    return map;
}

/** Function that returns a Style at the specified index in the specified StyleMap.
 *@pre 
    - KML object exists, is not NULL, and has not been freed
    - map exists, is not NULL, and has not been freed
    - index is a non-negative number
 *@post 
    - KML object has not been modified in any way
    - map has not been modified in any way
 *@return returns a struct representing the Style from the StyleMap.  
    - a pointer to a matching struct if the element was found
    - NULL otherwise
 *@param 
    - doc - a pointer to an KML struct
    - map - the StyleMap referencing the style we want
    - index - the index of the style we want, since the StyleMap may have more than on Style 
    
*/
Style* getStyleFromMap(const KML *doc, const StyleMap *map, int index)
{
    if(doc == NULL) return NULL;
    if(map == NULL) return NULL;
    if(index < 0) return NULL;

    char* id = NULL;

    //stylemaps only have 2 urls in our library
    //these should be 0 and 1 but the test harness uses 1 and 2.
    if(map->url1 == NULL) return NULL;
    if(map->url2 == NULL) return NULL;
    if(index == 0) id = map->url1;
    if(index == 1) id = map->url2;

    if(id == NULL) return NULL;

    //increment id pointer to remove leading "#" from StyleMap->url1/2
    id++;

    ListIterator iter = createIterator(doc->styles);
    Style *style = (Style *)nextElement(&iter);

    //iterate through style list
    while(style != NULL)
    {
        //return first style with matching id
        if(style->id != NULL)
        {
            if(strcmp(style->id, id) == 0) return style;
        }

        style = (Style *)nextElement(&iter);
    }

    //if we get to this point style will already be NULL so we are essentially returning NULL
    return style;
}

/* ******************************* A2 functions *************************** */
/** Function to create a KML struct based on the contents of an KML file.
 * This function must validate the XML tree generated by libxml against a KML schema file
 * before attempting to traverse the tree and create a KML struct
 *@pre File name cannot be an empty string or NULL.
       File represented by this name must exist and must be readable.
       Schema file name is not NULL/empty, and represents a valid schema file
 *@post Either:
        A valid KML struct has been created and its address was returned
		or 
		An error occurred, or KML file was invalid, and NULL was returned
 *@return the pinter to the new struct or NULL
 *@param fileName - a string containing the name of the KML file
**/
KML* createValidKML(const char *fileName, const char* schemaFile)
{

    if (fileName == NULL) return NULL;
    if (schemaFile == NULL) return NULL;
    if (fileName[0] == '\0') return NULL;
    if (schemaFile[0] == '\0') return NULL;

    KML *kml;

    xmlDoc *doc = NULL;

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    //read file from filename
    doc = xmlReadFile(fileName, NULL, 0);

    //if document cannot be read, return NULL
    if (doc == NULL)
    {
        /*free the document */
        xmlFreeDoc(doc);

        /*
         *Free the global variables that may
         *have been allocated by the parser.
         */
        xmlCleanupParser();
        return (NULL);
    }

    //check if xmlDoc * can be validated against schemaFile
    //this function is seems to be very memory intensive, but so was the example from the slides
    int valid = validateXmlDoc(doc, schemaFile);

    //if doc is valid
    if(valid == 0)
    {
        printf("Validated %s with %s\n", fileName, schemaFile);
        kml = malloc(sizeof(KML));
    }
    //if doc is invalid
    else if(valid > 0)
    {
        printf("%s with %s cannot be validated.\n", fileName, schemaFile);
        /*free the document */
        xmlFreeDoc(doc);

        /*
        *Free the global variables that may
        *have been allocated by the parser.
        */
        
        xmlDictCleanup();
        xmlCleanupParser();
        return NULL;
    }
    //else, some kind of API error occured
    else
    {

        printf("xmlSchemaValidateDoc returned '%d' which means an internal error or API error occured...\n", valid);
        /*free the document */
        xmlFreeDoc(doc);

        /*
         *Free the global variables that may
         *have been allocated by the parser.
         */

        //deals with a weird issue where xmlFreeDoc generates a mutex for an xmlDict which does not get freed so this needs to be called
        xmlDictCleanup();
        xmlCleanupParser();
        return NULL;
    }

    // initialize all lists that are contained within a KML structure (see KMLParser.h)
    kml->namespaces = initializeList(&XMLNamespaceToString, &deleteXMLNamespace, &compareXMLNamespace);
    kml->pointPlacemarks = initializeList(&pointPlacemarkToString, &deletePointPlacemark, &comparePointPlacemarks);
    kml->pathPlacemarks = initializeList(&pathPlacemarkToString, &deletePathPlacemark, &comparePathPlacemarks);
    kml->styles = initializeList(&styleToString, &deleteStyle, &compareStyles);
    kml->styleMaps = initializeList(&styleMapToString, &deleteStyleMap, &compareStyleMaps);

    //get root_element from xmlDoc
    xmlNode *root_element = xmlDocGetRootElement(doc);

    //make sure the xmlDoc has a root xmlNode
    if(root_element == NULL)
    {
        //free doc on failure
        xmlFreeDoc(doc);
        xmlDictCleanup();

        //free any memory that has been allocated by xml parser
        xmlCleanupParser();
    }

    //Generates all structs and puts them in their respective lists
    parseNodes(kml, doc, root_element);

    //frees xmlDoc
    xmlFreeDoc(doc);
    xmlDictCleanup();

    /*
    *Free the global variables that may
    *have been allocated by the parser.
    */
    xmlCleanupParser();

    return kml;
}

/** Function to validating an existing a KML struct against a KML schema file
 *@pre 
    KML struct exists and is not NULL
    schema file name is not NULL/empty, and represents a valid schema file
 *@post KML struct has not been modified in any way
 *@return the boolean and indicating whether the KML struct is valid
 *@param obj - a pointer to a KML struct
 *@param obj - the name iof a schema file
 **/
bool validateKML(const KML *doc, const char* schemaFile)
{
    if(doc == NULL) return false;
    if(schemaFile == NULL) return false;
    if(schemaFile[0] == '\0') return false;

    ListIterator iter;

    //validate all namespaces
    if(doc->namespaces == NULL) return false;
    iter = createIterator(doc->namespaces);
    XMLNamespace *ns = nextElement(&iter);
    while(ns != NULL)
    {
        //make sure namespace->value is up to spec
        if(ns->value == NULL) return false;
        if(strcmp(ns->value, "") == 0) return false;
        ns = nextElement(&iter);
    }

    //validate all stylemaps
    if(doc->styleMaps == NULL) return false;
    iter = createIterator(doc->styleMaps);
    StyleMap *stylemap = nextElement(&iter);
    while(stylemap != NULL)
    {
        //make sure stylemap is up to spec
        if(stylemap->id == NULL ) return false;
        if(strcmp(stylemap->id, "") == 0) return false;

        stylemap = nextElement(&iter);
    }

    //validate all styles
    if(doc->styles == NULL) return false;
    iter = createIterator(doc->styles);
    Style *style = nextElement(&iter);
    while(style != NULL)
    {
        //check if style is valid according to KMLParser.h specs
        if(style->id == NULL || style->colour == NULL) return false;
        if(strcmp(style->id, "") == 0 || strcmp(style->colour, "") == 0) return false;
        style = nextElement(&iter);
    }
    
    //validate all point placemarks
    if(doc->pointPlacemarks == NULL) return false;
    iter = createIterator(doc->pointPlacemarks);
    PointPlacemark *pointPlacemark = nextElement(&iter);
    while(pointPlacemark != NULL)
    {
        //check if point placemark is up to spec
        if(pointPlacemark->point == NULL) return false;
        if(pointPlacemark->otherElements == NULL) return false;

        //check if point struct is up to spec
        Point *point = pointPlacemark->point;
        if (point->coordinate == NULL) return false;
        if (point->otherElements == NULL) return false;

        //validate KML elements in pathPlacemark->otherElements list
        ListIterator elemIter = createIterator(pointPlacemark->otherElements);
        KMLElement *elem = nextElement(&elemIter);
        while(elem != NULL)
        {
            if(elem->name == NULL || elem->value == NULL) return false;
            if(elem->name[0] == '\0' || elem->value[0] == '\0') return false;
            elem = nextElement(&elemIter);
        }

        //validate KML elements in point->otherElements list
        elemIter = createIterator(point->otherElements);
        elem = nextElement(&elemIter);
        while(elem != NULL)
        {
            if(elem->name == NULL || elem->value == NULL) return false;
            if(elem->name[0] == '\0' || elem->value[0] == '\0') return false;
            elem = nextElement(&elemIter);
        }

        pointPlacemark = nextElement(&iter);
    }

    //validate all path placemarks
    if(doc->pathPlacemarks == NULL) return false;
    iter = createIterator(doc->pathPlacemarks);
    PathPlacemark *pathPlacemark = nextElement(&iter);
    while(pathPlacemark != NULL)
    {
        //check if path placemark is up to spec from KMLParser.h
        if(pathPlacemark->pathData == NULL) return false;
        if(pathPlacemark->otherElements == NULL) return false;
        
        //check if line struct is up to spec
        Line *pathData = pathPlacemark->pathData;
        if(pathData->coordinates == NULL) return false;
        if(pathData->otherElements == NULL) return false;
        if(getLength(pathData->coordinates) < 2) return false;

        //validate KML elements in pathPlacemark->otherElements list
        ListIterator elemIter = createIterator(pathPlacemark->otherElements);
        KMLElement *elem = nextElement(&elemIter);
        while(elem != NULL)
        {
            if(elem->name == NULL || elem->value == NULL) return false;
            if(elem->name[0] == '\0' || elem->value[0] == '\0') return false;
            elem = nextElement(&elemIter);
        }

        //validate KML elements in pathData->otherElements list
        elemIter = createIterator(pathData->otherElements);
        elem = nextElement(&elemIter);
        while(elem != NULL)
        {
            if(elem->name == NULL || elem->value == NULL) return false;
            if(elem->name[0] == '\0' || elem->value[0] == '\0') return false;
            elem = nextElement(&elemIter);
        }

        pathPlacemark = nextElement(&iter);
    }

    //once we know that the contents of the KML struct are valid, we turn the struct into an xmlDoc and validate the tree against the schemaFile
    xmlDoc *temp = KMLtoXmlDoc(doc);
    if (temp == NULL) return false;
    int valid = validateXmlDoc(temp, schemaFile);

    // if doc is not valid
    if (valid != 0)
    {
        xmlFreeDoc(temp);
        /*
         *Free the global variables that may
         *have been allocated by the parser.
         */

        xmlDictCleanup();
        xmlCleanupParser();
        return false;
    }

    xmlFreeDoc(temp);

    return true;
}

/** Function to writing a KML struct into a file in KML format.
 *@pre
    KML object exists, is valid, and and is not NULL.
    fileName is not NULL, has the correct extension
 *@post 
    - KML has not been modified in any way
    - file name has not been modified in any way
    - a file representing the KML contents in KML format has been created
 *@return a boolean value indicating success or failure of the write
 *@param
    - doc - a pointer to a KML struct
 	- fileName - the name of the output file
 **/
bool writeKML(const KML* doc, const char* fileName)
{
    if (doc == NULL) return false;
    if (fileName == NULL) return false;
    if (fileName[0] == '\0') return false;

    xmlDoc *new_doc = KMLtoXmlDoc(doc);

    if(new_doc == NULL) return false;

    /* 
     * Dumping document to stdio or file
     */
    xmlSaveFormatFileEnc(fileName, new_doc, "UTF-8", 1);

    /*free the document */
    xmlFreeDoc(new_doc);

    //free any variables that may have been allocated by encoding handlers
    xmlCleanupCharEncodingHandlers();

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();

    /*
     * this is to debug memory for regression tests
     */
    return true;
}

/** Function that returns the length of the path in a PathPlacemark 
 *@pre PathPlacemark object exists, is not null, and has not been freed
 *@post PathPlacemark object had been modified in any way
 *@return length of the path in meters
 *@param ppm - a pointer to a PathPlacemark struct
**/
double getPathLen(const PathPlacemark *ppm)
{
    //make sure pathmark is valid, pathData is valid, and there are 2 or more points
    if(ppm == NULL) return 0;
    if(ppm->pathData == NULL) return 0;
    if(ppm->pathData->coordinates == NULL) return 0;
    if(getLength(ppm->pathData->coordinates) < 2) return 0;

    double len = 0;
    Line *pathData = ppm->pathData;

    ListIterator iter = createIterator(pathData->coordinates);
    Coordinate *coord = nextElement(&iter);
    int c = 0;
    double lat1, lat2, lon1, lon2;

    while(coord != NULL)
    {
        //on first iteration set lon1/lat1
        if(c == 0)
        {
            lon1 = coord->longitude;
            lat1 = coord->latitude;
            c=1;
        }
        //for every other iteration we get lon2/lat2, calculate distance with haversine formula, and set lon1/lat1 to lon2/lat2
        else
        {
            lon2 = coord->longitude;
            lat2 = coord->latitude;

            //add distance between points to len
            len += haversine(lon1, lat1, lon2, lat2);

            lon1 = lon2;
            lat1 = lat2;
        }

        //get next coordinate
        coord = nextElement(&iter);
    }

    return len;
}

/** Function that checks if the current path is a loop
 *@pre PathPlacemark object exists, is not null
 *@post PathPlacemark object exists, is not null, has not been modified
 *@return true if the path is a loop, false otherwise
 *@param ppm - a pointer to a PathPlacemark struct
 *@param delta - the tolerance used for comparing distances between start and end points
**/
bool isLoopPath(const PathPlacemark* ppm, double delta)
{
    if(ppm == NULL) return false;
    if(ppm->pathData == NULL) return false;
    if(ppm->pathData->coordinates == NULL) return false;
    if(getLength(ppm->pathData->coordinates) < 4) return false; //have at least 4 coordinates in total
    if(delta < 0) return false; //delta cannot be negative

    Line *path = ppm->pathData;

    Coordinate *first_coord, *tmp_coord, *last_coord;
    
    ListIterator iter = createIterator(path->coordinates);
    first_coord = nextElement(&iter); //get first coordinate
    tmp_coord = nextElement(&iter); //get second coordinate
    
    //gets last coordinate from coordinate list
    while(tmp_coord != NULL)
    {
        last_coord = tmp_coord;
        tmp_coord = nextElement(&iter);
    }

    //get distance between first and last point
    double dist = haversine(first_coord->longitude, first_coord->latitude, last_coord->longitude, last_coord->latitude);

    bool is_loop = (dist < delta);

    return is_loop; 
}

/** Function that returns the the list of paths with the specified length, using the provided tolerance 
 * to compare path lengths
 *@pre KML struct exists, is not null
 *@post KML struct exists, is not null, has not been modified
 *@return the list of PathPlacemark with the specified length
 *@param doc - a pointer to a KML struct
 *@param len - search track length
 *@param delta - the tolerance used for comparing track lengths
**/
List* getPathsWithLength(const KML *doc, double len, double delta)
{
    //make sure function parameters are up to spec
    if (doc == NULL) return NULL;
    if (len < 0) return NULL;
    if (delta < 0) return NULL;

    //make sure doc has path placemarks
    if (doc->pathPlacemarks == NULL) return NULL;
    if (getLength(doc->pathPlacemarks) == 0) return NULL;

    //initialize an empty list
    List *results = initializeList(&pathPlacemarkToString, &deleteDataDummy, &comparePathPlacemarks);

    //create iterator of path placemarks and get first element from list
    ListIterator iter = createIterator(doc->pathPlacemarks);
    PathPlacemark *ppm = nextElement(&iter);

    //set max/min length based on len +/- delta
    double max_len = len + delta;
    double min_len = len - delta;

    //iterate through path placemarks
    while (ppm != NULL)
    {
        //get current path total distance
        double dist = getPathLen(ppm);
        
        //make sure getPathLen() does not return 0 since that means an error occured
        if(dist > 0)
        {
            //if the distance between two lengths is within the tolerance, they are considered equal and the path placemark is added to the results list
            if(dist >= min_len && dist <= max_len) insertBack(results, ppm);
        }
        
        //get next path placemark
        ppm = nextElement(&iter);
    }

    return results;
}

/* ******************************* List helper functions  - MUST be implemented *************************** */

//Free KMLElement struct and strings that exist within it.
void deleteKMLElement( void* data)
{
    if(data == NULL)
    {
        return;
    }

    KMLElement *elem = data;

    if(elem->name != NULL) free(elem->name);
    if(elem->value != NULL) free(elem->value);
    free(elem);
}

//Return string of KMLElement struct contents
char* KMLElementToString( void* data)
{
    if(data == NULL)
    {
        return NULL;
    }

    KMLElement *elem = data;
    char *str;

    //malloc string with size of name + value + 3 extra characters for ": " and the escape character
    str = malloc((strlen(elem->name) + strlen(elem->value) + 3) * sizeof(char));

    strcpy(str, elem->name);
    strcat(str, ": ");
    strcat(str, elem->value);
    
    return str;
}

//compare contents of KMLElements and return if all contents are identical
int compareKMLElements(const void *first, const void *second)
{
    if(first == NULL || second == NULL)
    {
        return 0;
    }

    const KMLElement *first_elem = first;
    const KMLElement *second_elem = second;

    //it should be impossible for an element to contain an empty or null string so we return the result of the strcmp
    return (strcmp(first_elem->name, second_elem->name) == 0 && strcmp(first_elem->value, second_elem->value) == 0);
}

//Free up namespace struct and strings that exist within it
void deleteXMLNamespace(void* data)
{
    if(data == NULL)
    {
        return;
    }

    XMLNamespace *ns = data;

    //check in case prefix is not allocated, don't have to worry since it should NEVER be NULL
    if(ns->prefix != NULL) free(ns->prefix);
    free(ns->value);
    free(ns);
}

//Return string of XMLNamespace struct contents
char* XMLNamespaceToString( void* data)
{
    if(data == NULL)
    {
        return NULL;
    }

    XMLNamespace *namespace = data;
    char *str;
    char tmp[6] = "(NULL)";

    //malloc string with size of prefix + value + 3 extra characters for ": " and the escape character
    if(namespace->prefix != NULL)
    {
        str = malloc((strlen(namespace->prefix) + strlen(namespace->value) + 3) * sizeof(char));
        strcpy(str, namespace->prefix);
    }
    else    //malloc string with size of tmp + value + 3 extra characters for ": " and the escape character
    {
        str = malloc((strlen(tmp) + strlen(namespace->value) + 3) * sizeof(char));
        strcpy(str, tmp);
    }

    strcat(str, ": ");
    strcat(str, namespace->value);
    
    return str;
}

//compare contents of XMLNamespace struct and return if all contents are identical
int compareXMLNamespace(const void *first, const void *second)
{
    if(first == NULL || second == NULL)
    {
        return 0;
    }

    const XMLNamespace *first_ns = first;
    const XMLNamespace *second_ns = second;

    //reusable logic (was used for testing)
    bool prefix_null = (first_ns->prefix == NULL && second_ns->prefix == NULL);
    bool prefix_not_null = (first_ns->prefix != NULL && second_ns->prefix != NULL);

    if(prefix_not_null) //if both prefixes are not null, return strcmp of namespace prefixes and strcmp of namespace values
    {
        bool prefixes = (strcmp(first_ns->prefix, second_ns->prefix) == 0);
        bool values = (strcmp(first_ns->value, second_ns->value) == 0);

        return (prefixes && values);
    }
    else if(prefix_null) //if both prefixes are null, return strcmp of namespace values
    {
        bool values = (strcmp(first_ns->value, second_ns->value) == 0);
        return (values);
    }

    //if prefixes do not match as NULL or with strings, return false (0).
    return false;
}

//Free up style struct
void deleteStyle( void* data)
{
    if(data == NULL)
    {
        return;
    }

    Style *style = data;

    if(style->id != NULL) free(style->id);
    if(style->colour != NULL) free(style->colour);
    free(style);
}

char* styleToString( void* data)
{
    if(data == NULL)
    {
        return NULL;
    }

    char *str;
    Style *style = data;
    int len = 0;

    if(style->id != NULL) len += strlen(style->id);
    if(style->colour != NULL) len += strlen(style->colour);

    str = malloc(sizeof(char) * (len + 46));
    sprintf(str, "Style id: %s\n\tcolour: %s\n\twidth: %d\n\tfill: %d", style->id, style->colour, style->width, style->fill);

    return str;
}

int compareStyles(const void *first, const void *second)
{
    if(first == NULL || second == NULL) return 0;

    const Style *s1 = first;
    const Style *s2 = second;

    int id_match = false;
    int color_match = false;
    int width_match = false;
    int fill_match = false;

    //compare ids
    if(s1->id != NULL && s2->id != NULL)
    {
        if(strcmp(s1->id, s2->id) == 0) id_match = true;
    }

    //compare colors
    if(s1->colour != NULL && s2->colour != NULL)
    {
        if(strcmp(s1->colour, s2->colour) == 0) color_match = true;
    }

    //compare width
    if(s1->width == s2->width) width_match = true;

    //compare fill
    if(s1->fill == s2->fill) fill_match = true;

    return (id_match && color_match && width_match && fill_match);
}

//Free up stylemap struct
void deleteStyleMap( void* data)
{
    if(data == NULL)
    {
        return;
    }

    StyleMap *map = data;

    if(map->id != NULL) free(map->id);
    if(map->key1 != NULL) free(map->key1);
    if(map->url1 != NULL) free(map->url1);
    if(map->key2 != NULL) free(map->key2);
    if(map->url2 != NULL) free(map->url2);
    free(map);
}

char* styleMapToString( void* data)
{
    if(data == NULL)
    {
        return NULL;
    }

    //strlen of all strings in stylemap struct
    int len = 0;
    StyleMap *map = data;
    char *str;

    if(map->id != NULL) len += strlen(map->id);
    if(map->key1 != NULL) len += strlen(map->key1);
    if(map->url1 != NULL) len += strlen(map->url1);
    if(map->key2 != NULL) len += strlen(map->key2);
    if(map->url2 != NULL) len += strlen(map->url2);

    //56 = length of string below according to vscode
    str = malloc(sizeof(char) * (len + 56));

    sprintf(str, "StyleMap id: %s\n\tkey1: %s\n\turl1: %s\n\tkey2: %s\n\turl2: %s", map->id, map->key1, map->url1, map->key2, map->url2);

    return str;
}

int compareStyleMaps(const void *first, const void *second)
{
    if(first == NULL || second == NULL) return 0;

    const StyleMap *s1 = first;
    const StyleMap *s2 = second;

    int id_match = false;
    int key1_match = false;
    int url1_match = false;
    int key2_match = false;
    int url2_match = false;

    //check ids
    if(s1->id != NULL && s2->id != NULL)
    {
        if(strcmp(s1->id, s2->id) == 0) id_match = true;
    }

    //check key1 in both structs
    if(s1->key1 != NULL && s2->key1 != NULL)
    {
        if(strcmp(s1->key1, s2->key1) == 0) key1_match = true;
    }

    //check url1 in both structs
    if(s1->url1 != NULL && s2->url1 != NULL)
    {
        if(strcmp(s1->url1, s2->url1) == 0) url1_match = true;
    }

    //check key2 in both structs
    if(s1->key2 != NULL && s2->key2 != NULL)
    {
        if(strcmp(s1->key2, s2->key2) == 0) key2_match = true;
    }

    //check url2 in both structs
    if(s1->url2 != NULL && s2->url2 != NULL)
    {
        if(strcmp(s1->url2, s2->url2) == 0) url2_match = true;
    }

    return (id_match && key1_match && url1_match && key2_match && url2_match);
}

//Free up coordinate struct
void deleteCoordinate(void* data)
{
    if(data == NULL)
    {
        return;
    }

    Coordinate *coord = data;

    free(coord);
}   

char* coordinateToString( void* data)
{
    if(data == NULL)
    {
        return NULL;
    }

    char *str;
    Coordinate *c = data;

    //a length of 65 seems to result in 0 errors from valgrind, 62 causes errors so i added 3 to account for negative symbol as well
    str = malloc(sizeof(char) * 65);

    if(c->altitude != DBL_MAX)
    {
        sprintf(str, "\tCoordinate:\n\t\t\tlong: %lf\n\t\t\tlat: %lf\n\t\t\talt: %lf", c->longitude, c->latitude, c->altitude);
    } 
    else
    {
        sprintf(str, "\tCoordinate:\n\t\t\tlong: %lf\n\t\t\tlat: %lf\n\t\t\talt: MAX", c->longitude, c->latitude);
    }
    
    return str;
}

int compareCoordinates(const void *first, const void *second)
{
    if(first == NULL || second == NULL) return 0;

    const Coordinate *c1 = first;
    const Coordinate *c2 = second;

    int long_match = false;
    int lat_match = false;
    int alt_match = false;

    if(c1->longitude == c2->longitude) long_match = true;
    if(c1->latitude == c2->latitude) lat_match = true;
    if(c1->altitude == c2->altitude) alt_match = true;

    return (long_match && lat_match && alt_match);
}

//Free up point struct
void deletePoint(void* data)
{
    if(data == NULL)
    {
        return;
    }

    Point *point = data;
    
    if(point->coordinate != NULL) deleteCoordinate(point->coordinate);
    if(point->otherElements != NULL) freeList(point->otherElements);
    free(point);
}

char* pointToString(void* data)
{
    if(data == NULL) return NULL;

    Point *point = data;
    char *str;
    char *coord_str = coordinateToString(point->coordinate);

    //count number of other elements attached to point struct
    int count = 0;
    ListIterator nsIter = createIterator(point->otherElements);
    void *elem;

    elem = nextElement(&nsIter);
    while(elem != NULL)
    {
        count++;
        elem = nextElement(&nsIter);
    }

    //malloc string with length of coordinate string and the string below (25 chars according to vscode)
    str = malloc(sizeof(char) * (strlen(coord_str) + 32));

    sprintf(str, "Point:\n\t%s\n\t\tOther Elements: %d", coord_str, count);

    free(coord_str);

    return str;
}

int comparePoints(const void *first, const void *second)
{
    if(first == NULL || second == NULL) return 0;

    const Point *p1 = first;
    const Point *p2 = second;

    int coord_match = false;
    int list_match = true;

    //check if coords match
    if(compareCoordinates(p1->coordinate, p2->coordinate)) coord_match = true;

    //iterate through both lists of elements
    ListIterator iter1 = createIterator(p1->otherElements);
    ListIterator iter2 = createIterator(p2->otherElements);

    KMLElement *elem1 = nextElement(&iter1);
    KMLElement *elem2 = nextElement(&iter2);

    //compare list elements
    while(elem1 != NULL && elem2 != NULL && list_match != false)
    {
        //if elem1 and elem2 do not match set list_match to false and break loop
        if(!compareKMLElements(elem1, elem2)) list_match = false;
        
        elem1 = nextElement(&iter1);
        elem2 = nextElement(&iter2);
    } 

    return (coord_match && list_match);
}

//Free up pointplacemark struct
void deletePointPlacemark(void* data)
{
    if(data == NULL)
    {
        return;
    }

    PointPlacemark *mark = data;

    if(mark->name != NULL) free(mark->name);
    if(mark->point != NULL) deletePoint(mark->point);
    if(mark->otherElements != NULL) freeList(mark->otherElements);
    free(mark);
}

char* pointPlacemarkToString(void* data)
{
    if(data == NULL)
    {
        return NULL;
    }

    char *str;
    PointPlacemark *mark = data;

    // count number of other elements attached to pointplacemark struct
    int count = 0;

    if(getLength(mark->otherElements) > 0)
    {
        ListIterator nsIter = createIterator(mark->otherElements);
        void *elem;

        elem = nextElement(&nsIter);
        while (elem != NULL)
        {
            count++;
            elem = nextElement(&nsIter);
        }
    }
    char *point_str = pointToString(mark->point);

    str = malloc(sizeof(char) * (strlen(point_str) + 55));

    sprintf(str, "Point Placemark: %s\n\t%s\n\tOther Elements: %d", mark->name, point_str, count);

    free(point_str);

    return str;
}

int comparePointPlacemarks(const void *first, const void *second)
{
    if(first == NULL || second == NULL) return 0;

    const PointPlacemark *p1 = first;
    const PointPlacemark *p2 = second;

    int name_match = false;
    int point_match = false;
    int list_match = true;

    //compare names
    if(p1->name != NULL && p2->name != NULL)
    {
        if(strcmp(p1->name, p2->name) == 0) name_match = true;
    }

    //compare points
    if(comparePoints(p1->point, p2->point)) point_match = true;

    //used to avoid iterating through list if anything has already been found to be not matching
    if(name_match == false || point_match == false) return false;

    // iterate through both lists of elements
    ListIterator iter1 = createIterator(p1->otherElements);
    ListIterator iter2 = createIterator(p2->otherElements);

    KMLElement *elem1 = nextElement(&iter1);
    KMLElement *elem2 = nextElement(&iter2);

    // compare list elements
    while (elem1 != NULL && elem2 != NULL && list_match != false)
    {
        // if elem1 and elem2 do not match set list_match to false and break loop
        if (!compareKMLElements(elem1, elem2)) list_match = false;

        elem1 = nextElement(&iter1);
        elem2 = nextElement(&iter2);
    }

    return (name_match && point_match && list_match);
}

//Free up pathplacemark struct
void deletePathPlacemark(void* data)
{
    if(data == NULL)
    {
        return;
    }

    PathPlacemark *mark = data;

    if(mark->name != NULL) free(mark->name);
    if(mark->pathData != NULL) deleteLine(mark->pathData);
    if(mark->otherElements != NULL) freeList(mark->otherElements);
    free(mark);
}

char* pathPlacemarkToString(void* data)
{
    if(data == NULL) return NULL;

    char *str;
    PathPlacemark *mark = data;

    int count = 0;

    if(getLength(mark->otherElements) > 0)
    {
        ListIterator nsIter = createIterator(mark->otherElements);
        void *elem;

        elem = nextElement(&nsIter);
        while (elem != NULL)
        {
            count++;
            elem = nextElement(&nsIter);
        }
    }

    char *line_str = lineToString(mark->pathData); 
    str = malloc(sizeof(char) * strlen(line_str) + 64);

    sprintf(str, "Path Placemark: %s\n\t%s\n\tOther Elements: %d", mark->name, line_str, count);

    free(line_str);

    return str;
}

int comparePathPlacemarks(const void *first, const void *second)
{
    if(first == NULL || second == NULL) return 0;

    const PathPlacemark *p1 = first;
    const PathPlacemark *p2 = second;

    int name_match = false;
    int line_match = false;
    int list_match = true;

    //compare names
    if(p1->name != NULL && p2->name != NULL)
    {
        if(strcmp(p1->name, p2->name) == 0) name_match = true;
    }

    //avoid comparing lines since its wasted time
    if(name_match == false) return false;

    //compare lines
    if(compareLines(p1->pathData, p2->pathData)) line_match = true;

    //avoid comparing list elements
    if(line_match == false) return false;


    // iterate through both lists of elements
    ListIterator iter1 = createIterator(p1->otherElements);
    ListIterator iter2 = createIterator(p2->otherElements);

    KMLElement *elem1 = nextElement(&iter1);
    KMLElement *elem2 = nextElement(&iter2);

    //compare list elements
    while (elem1 != NULL && elem2 != NULL && list_match != false)
    {
        // if elem1 and elem2 do not match set list_match to false and break loop
        if (!compareKMLElements(elem1, elem2)) list_match = false;

        elem1 = nextElement(&iter1);
        elem2 = nextElement(&iter2);
    }

    return (name_match && line_match && list_match);
}
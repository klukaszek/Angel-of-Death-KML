//Kyle Lukaszek
//ID: 1113798
//Course CIS 2750 F22
//Assignment 1

#include "KMLHelpers.h"

//Generates all structs from KML nodes and places them in their respective lists within the KML struct
void parseNodes(KML *kml, xmlDoc *doc, xmlNode *node)
{
     if(kml == NULL) return;


    xmlNode *cur_node = node;
    int is_root = 0;

    while(cur_node != NULL)
    {
        // deal with XMLNamespace
        if (cur_node->type == XML_ELEMENT_NODE && is_root == 0)
        {
            // Iterate through all namespaces in root element
            while (cur_node->ns != NULL)
            {
                // create XMLNamespace struct here
                XMLNamespace *namespace = createNamespace((char *)cur_node->ns->prefix, (char *)cur_node->ns->href);

                // If struct is not null then push namespace to KML namespace list.
                if (namespace != NULL)
                {
                    insertBack(kml->namespaces, namespace);
                }

                // Get next namespace from root element
                cur_node->ns = cur_node->ns->next;
            }

            cur_node = cur_node->xmlChildrenNode;
            is_root++;
        }

        //If there is a document node, get the first child so you can traverse the tree
        if ((!xmlStrcmp(cur_node->name, (const xmlChar *)"Document")))
        {
            cur_node = cur_node->children;
        }

        // convert placemark nodes and its children to respective structs
        else if ((!xmlStrcmp(cur_node->name, (const xmlChar *)"Placemark")))
        {
            
            int return_type = 0;
            int *ptr = &return_type;
            void *placemark = createPlacemark(doc, cur_node, ptr);

            //Check what kind of placemark was returned by createPlacemark()
            if(placemark != NULL)
            {
                //Insert PointPlacemark into list in kml struct
                if(return_type == 1)
                {
                    insertBack(kml->pointPlacemarks, (PointPlacemark *)placemark);
                }
                //Insert PathPlacemark into list in kml struct
                else if(return_type == 2)
                {
                    insertBack(kml->pathPlacemarks, (PathPlacemark *)placemark);
                }
            }
        }

        // convert StyleMap node and its children to respective structs
        else if ((!xmlStrcmp(cur_node->name, (const xmlChar *)"StyleMap")))
        {
            StyleMap *stylemap = createStyleMap(doc, cur_node);
            if(stylemap != NULL)
            {
                insertBack(kml->styleMaps, stylemap);
            }
        }

        // convert style node and its children to respective structs
        else if ((!xmlStrcmp(cur_node->name, (const xmlChar *)"Style")))
        {
            Style *style = createStyle(doc, cur_node);
            if(style != NULL)
            {
                insertBack(kml->styles, style);
            }
        }

        //iterate to next node
        if (cur_node != NULL) cur_node = cur_node->next;
    }
}

//allocates all memory needed for an XMLNamespace struct and returns it.
XMLNamespace* createNamespace(char *prefix, char *value)
{
    //check if value is null or is empty
    if(value == NULL || value[0] == '\0') return NULL;

    XMLNamespace *namespace = malloc(sizeof(XMLNamespace));

    //set prefix if it is not null
    if(prefix != NULL)
    {
        namespace->prefix = malloc(sizeof(char) * (strlen(prefix) + 1));
        strcpy(namespace->prefix, prefix);
    }
    else
    {
        namespace->prefix = NULL;
    }
    
    //set value since it cannot be null
    namespace->value = malloc(sizeof(char) * (strlen(value) + 1));
    strcpy(namespace->value, value);

    return namespace;
}

//Create KMLElement struct and set name string, and value string
KMLElement* createElement(char *name, char *value)
{
    //check if name or value are NULL
    if(name == NULL || value == NULL)
    {
        return NULL;
    }
    else if(name[0] == '\0' || value[0] == '\0') //check if name or value are empty
    {
        return NULL;
    }

    //allocate memory for element struct
    KMLElement *elem = malloc(sizeof(KMLElement));

    //allocate memory for strings within element struct
    elem->name = malloc((strlen(name) + 1) * sizeof(char));
    elem->value = malloc((strlen(value) + 1) * sizeof(char));
    
    //copy strings of name and value to struct
    strcpy(elem->name, name);
    strcpy(elem->value, value);

    return elem;
}

//Create Coordinate struct with a string of longitude, latitude, and (sometimes) altitude coordinates
Coordinate* createCoordinate(char *coords)
{

    if(coords == NULL) return NULL;

    int count = 0;
    double value = 0;

    Coordinate *coordinate = malloc(sizeof(Coordinate));
    coordinate->altitude = DBL_MAX;

    char *token = strtok(coords, ",");
    

    while (token != NULL)
    {
        sscanf(token, "%lf", &value);

        switch(count)
        {
            case 0:
                coordinate->longitude = value;
                break;
            case 1:
                coordinate->latitude = value;
                break;
            case 2:
                coordinate->altitude = value;
                break;
        }

        count++;
        token = strtok(NULL, ",");
    }

    return coordinate;
}

//Create 2d array of strings that contain the data for each set of coordinates
char** parseCoords(char *coords, int *count)
{           

    //check if node is NULL
    if(coords == NULL || coords[0] == '\0')
    {
        return NULL;
    }

    char *token;

    /*-------------------------- count sets of coords -------------------*/
    char *copy = malloc(sizeof(char) * (strlen(coords) + 1));
    strcpy(copy, coords);
    copy[strlen(copy)-1] = '\0';

    token = strtok(copy, " \t");
    while(token != NULL)
    {
        //if string is not empty, increment array length
        if(!strIsEmpty(token)) *count += 1;
        
        token = strtok(NULL, " ");
    }

    free(copy);
    /*-------------------------------------------------------------------*/

    /*------- this is where you assemble a 2D string array of the coordinates------*/
    char **coord_arr = (char **) calloc(*count, sizeof(char *));
    token = strtok(coords, " \t");
    int i = 0;
    while(token != NULL)
    {
        if(!strIsEmpty(token))
        {
            coord_arr[i] = (char *) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(coord_arr[i], token);
            i++;
        }

        token = strtok(NULL, " ");
    }
    /*-------------------------------------------------------------------*/

    return coord_arr;
}

//Create Point struct with Coordinate struct and list for otherElements, then returns it
Point* createPoint(xmlDoc *doc, xmlNode *node)
{
    if(doc == NULL) return NULL;
    if(node == NULL) return NULL;

    xmlChar *key;
    int count = 0;
    int *ptr = &count;

    Point *point = malloc(sizeof(Point));
    point->coordinate = NULL;
    point->otherElements = initializeList(&KMLElementToString, &deleteKMLElement, &compareKMLElements);

    //Go to first child of point node
    node = node->children;

    //iterate through the children of the point node
    while(node != NULL)
    {
        //get content of node
        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);

        //Add Coordinate struct to Point struct
        if ((!xmlStrcmp(node->name, (const xmlChar *)"coordinates")))
        {
            char ** coords = parseCoords((char *) key, ptr);
            
            //iterate through array of coordinates, should only have 1 string since this is a point
            if(coords != NULL)
            {
                //this should only ever go up to i=1 since this is a point and not a line
                for(int i = 0; i < count; i++)
                {
                    if(coords[i] != NULL)
                    {
                        point->coordinate = createCoordinate(coords[i]);
                        free(coords[i]);
                    }
                }
                free(coords);
            }
        }

        //Add KMLElements to list to add to final placemark struct as long as node is not titled "text"
        else if(xmlStrcmp(node->name, (const xmlChar *)"text"))
        {
            KMLElement *elem = createElement((char *)node->name, (char *)key);
            if(elem != NULL)
            {
                insertBack(point->otherElements, elem);
            }
        }

        //free content of node
        xmlFree(key);

        //go to next node
        node = node->next;
    }

    //Check coordinate requirements and list requirements for Point struct
    if(point->coordinate == NULL || point->otherElements == NULL)
    {
        deletePoint(point);
        return NULL;
    }

    return point;
}

//Create PointPlacemark struct with name (optional), line, leaves otherElement list empty so that createPlacemark() can attach it at the very end.
PointPlacemark* createPointPlacemark(char *name, Point *point)
{   
    if(point == NULL)
    {
        return NULL;
    }

    PointPlacemark *pointmark = malloc(sizeof(PointPlacemark));
    pointmark->name = NULL;

    if (name != NULL)
    {
        pointmark->name = malloc(sizeof(char) * (strlen(name) + 1));
        strcpy(pointmark->name, name);
        //name is freed in function that calls this
    }

    pointmark->point = point;
    pointmark->otherElements = NULL;

    return pointmark;
}

//Create Line struct with 2 lists for coordinates and otherElements and returns it
Line* createLine(xmlDoc *doc, xmlNode *node)
{  
    if(doc == NULL) return NULL;
    if(node == NULL) return NULL;

    xmlChar *key;
    int count = 0;
    int *ptr = &count;

    Line *line = malloc(sizeof(Line));
    line->coordinates = initializeList(&coordinateToString, &deleteCoordinate, &compareCoordinates);
    line->otherElements = initializeList(&KMLElementToString, &deleteKMLElement, &compareKMLElements);

    //Go to first child of line node
    node = node->children;

    //iterate through children of line node
    while(node != NULL)
    {
        //get content of node
        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);

        //Add Coordinate struct to Point struct
        if ((!xmlStrcmp(node->name, (const xmlChar *)"coordinates")))
        {
            char ** coords = parseCoords((char *) key, ptr);
            
            //iterate through array of coordinates, should be greater than 2
            if(coords != NULL)
            {
                //follow Line struct coordinate requirements (must have minimum length of 2)
                if(count >= 2)
                {
                    // this should only ever go up to i=1 since this is a point and not a line
                    for (int i = 0; i < count; i++)
                    {
                        if (coords[i] != NULL)
                        {
                            insertBack(line->coordinates, createCoordinate(coords[i]));
                            free(coords[i]);
                        }
                    }
                }
                free(coords);
            }
        }

        //Add KMLElements to list to add to final placemark struct as long as node is not titled "text"
        else if(xmlStrcmp(node->name, (const xmlChar *)"text"))
        {
            KMLElement *elem = createElement((char *)node->name, (char *)key);
            if(elem != NULL)
            {
                insertBack(line->otherElements, elem);
            }
        }

        //free content of node
        xmlFree(key);

        //go to next node
        node = node->next;
    }

    //Check coordinate requirements and list requirements for Point struct
    if(line->coordinates == NULL || getLength(line->coordinates) < 2 || line->otherElements == NULL)
    {
        deleteLine(line);
        return NULL;
    }

    return line;
}

//Create PathPlacemark struct with name (optional), line, leaves otherElement list empty so that createPlacemark() can attach it at the very end.
PathPlacemark* createPathPlacemark(char *name, Line *line)
{
    if(line == NULL)
    {
        return NULL;
    }

    PathPlacemark *pathmark = malloc(sizeof(PathPlacemark));
    pathmark->name = NULL;

    if (name != NULL)
    {
        pathmark->name = malloc(sizeof(char) * (strlen(name) + 1));
        strcpy(pathmark->name, name);
        //name is freed in function that calls this
    }

    pathmark->pathData = line;
    pathmark->otherElements = NULL;

    return pathmark;
}

//This function constructs a PointPlacemark struct or a PathPlacemark struct and returns is a void* data type. Once returned, the type is indicated by the passed "int *return_type".
void *createPlacemark(xmlDoc *doc, xmlNode *node, int *return_type)
{   
    if(doc == NULL) return NULL;
    if(node == NULL) return NULL;

    char *name = NULL;
    xmlChar *key;
    PointPlacemark *pointmark = NULL;
    PathPlacemark *pathmark = NULL;
    List *otherElements = initializeList(&KMLElementToString, &deleteKMLElement, &compareKMLElements);

    //go to 1st child of placemark node
    node = node->children;

    //iterate through children of placemark node
    while(node != NULL)
    {   
        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);

        //Get name for final placemark struct
        if ((!xmlStrcmp(node->name, (const xmlChar *)"name")))
        {
            if (xmlStrlen(key) < 256)
            {
                name = malloc(sizeof(char) * (strlen((char *)key) + 1));
                strcpy(name, (char *)key);
            }
        }

        //Get Point struct for final placemark struct
        else if ((!xmlStrcmp(node->name, (const xmlChar *)"Point")))
        {
            pointmark = createPointPlacemark(name, createPoint(doc, node));
        }

        //Get Line struct for final placemark struct
        else if((!xmlStrcmp(node->name, (const xmlChar *)"LineString")))
        {
            pathmark = createPathPlacemark(name, createLine(doc, node));
        }
        
        //Add KMLElements to list to add to final placemark struct as long as node is not titled "text"
        else if(xmlStrcmp(node->name, (const xmlChar *)"text"))
        {
            KMLElement *elem = createElement((char *)node->name, (char *)key);
            if(elem != NULL)
            {
                insertBack(otherElements, elem);
            }
        }

        xmlFree(key);
        node = node->next;
    }

    //free name 
    if(name != NULL)
    {
        free(name);
    }

    //Return PointPlacemark
    if(pointmark != NULL)
    {
        pointmark->otherElements = otherElements;
        
        //set return type value to that of PointPlacemark
        *return_type = 1;
        return pointmark;
    }

    //Return PathPlacemark
    if(pathmark != NULL)
    {
        pathmark->otherElements = otherElements;
        
        //set return type value to that of PathPlacemark
        *return_type = 2;
        return pathmark;
    }

    return NULL;
}

//get string that represent id attribute for Style and StyleMap nodes
char *getIDStr(xmlNode *node)
{
    if(node == NULL) return NULL;
    
    char *id = NULL;
    xmlAttr *attr = node->properties;
    int found_id = false;

    //this is to check for the id attribute of the style node
    while(attr != NULL && found_id == false)
    {
        xmlNode *attr_node = attr->children;
        char *name = (char *)(attr_node->name);
        char *cont = (char *)(attr_node->content);
        
        //if either of these are null, something is wrong with the xml attribute and we return NULL in the else branch
        if (name != NULL && cont != NULL)
        {
            //check for id attribute in node (id is called text for some reason)
            if(strcmp(name, "text") == 0)
            {
                id = malloc(sizeof(char) * (strlen(cont) + 1));
                strcpy(id, cont);
                found_id = true;
            }
        }

        attr = attr->next;
    }

    //if it could not get an id from xmlAttr id is still NULL so we don't have to check
    return id;
}

//parses linestyle node data into Style struct
void parseLineStyle(xmlDoc *doc, xmlNode *node, Style *style)
{
    if(doc == NULL || node == NULL || style == NULL) return;

    xmlChar *key;

    node = node->children;

    // Look for linestyle color and width
    while (node != NULL)
    {

        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);

        // Get style->colour
        if ((!xmlStrcmp(node->name, (const xmlChar *)"color")))
        {
            // make sure color is 8 hex digits in length, otherwise just ignore it since it is incomplete
            if (xmlStrlen(key) == 8)
            {
                style->colour = malloc(sizeof(char) * ((xmlStrlen(key) + 1)));
                strcpy(style->colour, (char *)key);
            }
        }

        // Get style->width for linestyle
        if ((!xmlStrcmp(node->name, (const xmlChar *)"width")))
        {
            // max int is 2147483647 which is 10 digits
            if (xmlStrlen(key) <= 10 && xmlStrlen(key) > 0)
            {
                sscanf((char *)key, "%d", &style->width);
            }
        }

        xmlFree(key);

        // get next linestyle child node
        node = node->next;
    }
}

//parses polystyle node data into Style struct
void parsePolyStyle(xmlDoc *doc, xmlNode *node, Style *style)
{
    if(doc == NULL || node == NULL || style == NULL) return;

    xmlChar *key;

    node = node->children;
    // Look for polystyle fill and width
    while (node != NULL)
    {
        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);

        // Get style->fill for polystyle
        if ((!xmlStrcmp(node->name, (const xmlChar *)"fill")))
        {
            // max int is 2147483647 which is 10 digits
            if (xmlStrlen(key) <= 10 && xmlStrlen(key) > 0)
            {
                sscanf((char *)key, "%d", &style->fill);
            }
        }

        xmlFree(key);

        // get next polystyle child node
        node = node->next;
    }
}

//Create style struct from style node in KML file
Style* createStyle(xmlDoc *doc, xmlNode *node)
{
    if(doc == NULL || node == NULL) return NULL;

    //initialize style struct
    Style *style = malloc(sizeof(Style));
    style->id = getIDStr(node);
    style->colour = NULL;
    //default value of linestyle width
    style->width = -1;
    //default value of polystyle width
    style->fill = -1;

    //make sure the style has an id, else free style and return NULL
    if(style->id != NULL)
    {
        // Get 1st child of Style node
        node = node->children;

        // iterate through children node to find linestyle or polystyle
        while (node != NULL)
        {
            // Get LineStyle node, and iterate through all of its child nodes in parseLineStyle
            if ((!xmlStrcmp(node->name, (const xmlChar *)"LineStyle")))
            {
                parseLineStyle(doc, node, style);
            }

            // Get PolyStyle node, and iterate through all of its child nodes in parsePolyStyle
            if ((!xmlStrcmp(node->name, (const xmlChar *)"PolyStyle")))
            {
                parsePolyStyle(doc, node, style);
            }

            // iterate to next node
            node = node->next;
        }
    }
    else
    {
        free(style);
        return NULL;
    }

    //check if color is not NULL 
    //we don't have to check strlen since we check that the color is 8 chars in length before allocating memory and copying color
    if(style->colour == NULL)
    {
        style->colour = malloc(sizeof(char) * 9);
        strcpy(style->colour, "ffffffff");
    }

    return style;
}

//Parses pair node data into stylemap struct
void parsePair(xmlDoc *doc, xmlNode *node, StyleMap *stylemap, int pair_num)
{
    if(doc == NULL || node == NULL || stylemap == NULL) return;

    xmlChar *key;

    node = node->children;

    // Look for pair key and styleURL
    while (node != NULL)
    {
        key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);

        // Get stylemap->key1/2
        if ((!xmlStrcmp(node->name, (const xmlChar *)"key")))
        {
            if (xmlStrlen(key) < 256)
            {
                //set key1
                if(pair_num == 1)
                {
                    stylemap->key1 = malloc(sizeof(char) * (xmlStrlen(key) + 1));
                    strcpy(stylemap->key1, (char *)key);
                }
                //set key2
                else if(pair_num == 2)
                {
                    stylemap->key2 = malloc(sizeof(char) * (xmlStrlen(key) + 1));
                    strcpy(stylemap->key2, (char *)key);
                }
            }
        }

        // Get stylemap->url1/2
        if ((!xmlStrcmp(node->name, (const xmlChar *)"styleUrl")))
        {
            if (xmlStrlen(key) < 256)
            {
                //set url1
                if(pair_num == 1)
                {
                    stylemap->url1 = malloc(sizeof(char) * (xmlStrlen(key) + 1));
                    strcpy(stylemap->url1, (char *)key);
                }
                //set url2
                else if(pair_num == 2)
                {
                    stylemap->url2 = malloc(sizeof(char) * (xmlStrlen(key) + 1));
                    strcpy(stylemap->url2, (char *)key);
                }
            }
        }

        xmlFree(key);
        node = node->next;
    }
}

//Create stylemap struct from stylemap node in KML file
StyleMap* createStyleMap(xmlDoc *doc, xmlNode *node)
{
    if(doc == NULL || node == NULL) return NULL;

    int pair_num = 1;
    StyleMap *stylemap = malloc(sizeof(StyleMap));
    stylemap->id = getIDStr(node);
    stylemap->key1 = NULL;
    stylemap->url1 = NULL;
    stylemap->key2 = NULL;
    stylemap->url2 = NULL;

    //make sure stylemap id is not NULL
    if(stylemap->id != NULL)
    {
        //Get 1st child of StyleMap node
        node = node->children;

        while(node != NULL)
        {
            if ((!xmlStrcmp(node->name, (const xmlChar *)"Pair")))
            {
                //parse pair node into stylemap struct
                parsePair(doc, node, stylemap, pair_num);
                pair_num++;
            }

            node = node->next;
        }
    }
    else
    {
        free(stylemap);
        return NULL;
    }

    return stylemap;
}


/* ******************************* Misc Helpers *************************** */
//Deletes a line struct and lists that exist within it
void deleteLine(void* data)
{
    if(data == NULL) return;

    Line *line = data;

    if(line->coordinates != NULL) freeList(line->coordinates);
    if(line->otherElements != NULL) freeList(line->otherElements);
    free(line);
}

char* lineToString(void* data)
{
    if(data == NULL) return NULL;

    char *str;
    Line *line = data;
    int coord_count = 0;
    int elem_count = 0;

    ListIterator coordIter = createIterator(line->coordinates);
    Coordinate *coord = nextElement(&coordIter);
    while(coord != NULL)
    {
        coord_count++;
        coord = nextElement(&coordIter);
    }

    ListIterator iter = createIterator(line->otherElements);
    KMLElement *elem = nextElement(&iter);
    while(elem != NULL)
    {
        elem_count++;
        elem = nextElement(&iter);
    }

    str = malloc(sizeof(char) * 47);

    sprintf(str, "Path Data:\n\t\tNum Coords: %d\n\t\tOther Elements: %d", coord_count, elem_count);

    return(str);
}

int compareLines(const void *first, const void *second)
{
    if(first == NULL || second == NULL) return false;

    const Line *l1 = first;
    const Line *l2 = second;

    int coord_match = true;
    int list_match = true;

    ListIterator coords1 = createIterator(l1->coordinates);
    ListIterator coords2 = createIterator(l2->coordinates);

    Coordinate *coord1 = nextElement(&coords1);
    Coordinate *coord2 = nextElement(&coords2);

    //compare coordinate list elements
    while (coord1 != NULL && coord2 != NULL && coord_match != false)
    {
        // if coord1 and coord2 do not match set coord_match to false and break loop
        if(!compareCoordinates(coord1, coord2)) coord_match = false;

        coord1 = nextElement(&coords1);
        coord2 = nextElement(&coords2);
    }

    if(coord_match == false) return false;

    // iterate through both lists of elements
    ListIterator iter1 = createIterator(l1->otherElements);
    ListIterator iter2 = createIterator(l2->otherElements);

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

    return (coord_match && list_match);
}

//checks if given string is empty
int strIsEmpty(char *str)
{
    if(str == NULL) return false;

    int i = 0;

    while(i < strlen(str)-1)
    {
        if(!isspace(str[i]))
        {
            return false;
        }

        i++;
    }

    return true;
}
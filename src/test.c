#include "KMLHelpersA3.h" //this includes KMLParser.h

//module 1
//createValidKML(): can handle any NULL or empty parameters without leaking
    //-> validateXmlDoc(): helper function can handle any NULL or empty parameters without leaking
        //-> cleans up and frees all sorts of libxml2 variables from memory to make sure there are no leaks
//writeKML(): can handle any NULL or empty parameters without leaking
    //-> can assume that validateKML is called before writeKML is called
        //-> still checks to make sure that everything is up to spec from KMLParser.h before writing to file
//validateKML(): can handle any NULL or empty parameters without leaking
    //-> any strcmp() calls are preceded with a check to make sure the passed string is not NULL
    //-> any references to structs are preceded with a check to make sure the struct is not NULL
    //-> double checked that everything was up to spec from KMLParser.h

//module 2
//getPathLen(): works as intended and can handle incorrect parameters without crashing or failing
//isLoopPath(): works as intended and can handle incorrect parameters without crashing or failing
//getPathsWithLength(): works as intended and can handle incorrect parameters without crashing or failing
    //-> makes sure that nothing is NULL or negative
    //-> nothing in this function should be able to cause a segfault
    //-> can return an empty list since it was never specified that it should not do this

int main(int argc, char *argv[])
{

    if (argc < 1)
    {
        printf("usage: test <someXMLfile>\n");

        return -1;
    }

    KML* kmlfile = createValidKML(argv[1], argv[2]);
    bool success = validateKML(kmlfile, argv[2]);

    // if(success)
    // {
    //     success = writeKML(kmlfile, "mykml.kml");
    // }

    ListIterator iter = createIterator(kmlfile->pathPlacemarks);
    PathPlacemark *ppm = nextElement(&iter);


    double len = getPathLen(ppm);
    success = isLoopPath(ppm, 10);

    printf("%f\n", len);
    printf("%s\n", success ? "true":"false");

    // double len = getPathLen(ppm);

    // printf("%f\n", len);

    // List *results = getPathsWithLength(kmlfile, 401, 10);

    // ListIterator iter2 = createIterator(results);
    // PathPlacemark *p = nextElement(&iter2);
    
    // int c = 0;
    // while(p != NULL)
    // {
    //     char *str = pathPlacemarkToString(p);

    //     printf("%d: %s\n", c, str);
    //     free(str);

    //     c++;
    //     p = nextElement(&iter2);
    // }

    // freeList(results);

    //deleteKML(kmlfile);

    return 0;
}
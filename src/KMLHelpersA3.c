#include "KMLHelpersA3.h"


/* ---------------- Helper functions for populating Points tab in Python GUI ---------------------*/

//Return point name from void pointer of a pointplacemark
char *getPointName(void *ppm)
{
    if(ppm == NULL) return NULL;
    PointPlacemark *point = (PointPlacemark *)ppm;
    if(point->name == NULL) return NULL;
    return point->name;
}

//change name of point placemark
void setPointName(void *ppm, char *str)
{
    if (ppm == NULL) return;
    PointPlacemark *point = (PointPlacemark *)ppm;

    if(point->name == NULL)
    {
        point->name = malloc(strlen(str)*sizeof(char)+1);
    }
    else
    {
        point->name = realloc(point->name, strlen(str)*sizeof(char)+1);
    }

    //if string is empty just strcpy an escape character
    if(strcmp(str, "") == 0)
    {
        strcpy(point->name, "\0");
        return;
    }
    
    strcpy(point->name, str);
}

//Return longitude from void pointer of a pointplacemark
double getPointLong(void *ppm)
{
    if(ppm == NULL) return 0;
    PointPlacemark *point = (PointPlacemark *)ppm;

    if(point->point == NULL) return 0;
    if(point->point->coordinate == NULL) return 0;
    return point->point->coordinate->longitude;
}

//Return latitude from void pointer of a pointplacemark
double getPointLat(void *ppm)
{
    if(ppm == NULL) return 0;
    PointPlacemark *point = (PointPlacemark *)ppm;

    if(point->point == NULL) return 0;
    if(point->point->coordinate == NULL) return 0;
    return point->point->coordinate->latitude;
}

//Return altitude from void pointer of a pointplacemark
double getPointAlt(void *ppm)
{
    if(ppm == NULL) return 0;
    PointPlacemark *point = (PointPlacemark *)ppm;

    if(point->point == NULL) return 0;
    if(point->point->coordinate == NULL) return 0;
    return point->point->coordinate->altitude;
}

/* ---------------- Helper functions for populating Paths tab in Python GUI ---------------------*/

char *getPathName(void *ppm)
{
    if(ppm == NULL) return NULL;
    PathPlacemark *path = (PathPlacemark *)ppm;
    if(path->name == NULL) return NULL;
    return path->name;
}

//change name of path placemark
void setPathName(void *ppm, char *str)
{
    if (ppm == NULL) return;
    PathPlacemark *path = (PathPlacemark *)ppm;
    
    if(path->name == NULL)
    {
        path->name = malloc(strlen(str)*sizeof(char)+1);
    }
    else
    {
        path->name = realloc(path->name, strlen(str)*sizeof(char)+1);
    }

    //if string is empty just strcpy an escape character
    if(strcmp(str, "") == 0)
    {
        strcpy(path->name, "\0");
        return;
    }

    strcpy(path->name, str);
}

double getPathLength(void *ppm)
{
    if(ppm == NULL) return 0;
    PathPlacemark *path = (PathPlacemark *)ppm;
    return getPathLen(path);
}

bool checkPathLoop(void *ppm, double delta)
{
    if(ppm == NULL) return 0;
    PathPlacemark *path = (PathPlacemark *)ppm;
    return isLoopPath(path, delta);
}

/* ---------------- Helper functions for populating Styles tab in Python GUI ---------------------*/
char *getStyleColour(void *s)
{
    if(s == NULL) return NULL;
    Style *style = (Style *) s;
    if(style->colour == NULL) return NULL;
    return style->colour;
}

void setStyleColour(void *s, char *str)
{
    if (s == NULL) return;
    Style *style = (Style *)s;

    style->colour = realloc(style->colour, strlen(str)*sizeof(char)+1);
    strcpy(style->colour, str);
}

int getStyleWidth(void *s)
{
    if(s == NULL) return 0;
    Style *style = (Style *) s;
    return style->width;
}

void setStyleWidth(void *s, int w)
{
    if (s == NULL) return;
    if (w < 0) return;
    Style *style = (Style *)s;
    style->width = w;
}

int getStyleFill(void *s)
{
    if(s == NULL) return 0;
    Style *style = (Style *) s;
    return style->fill;
}
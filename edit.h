#ifndef EDIT_H
#define EDIT_H
#include <stdio.h>
#include <string.h>
#include "type.h"

typedef struct
{
    char *srcmp3_fname;
    char *newvalue;
    FILE *fptr_mp3;
    char tag[5];
    unsigned int size;
}Toeditmp3;

Status readandvalidateedit(char *argv[],Toeditmp3 *edit);    //validate edit argument
Status openeditfile(Toeditmp3 *edit);     //open mp3 files
Status toedit(Toeditmp3 *edit);      //perform edit
Status findframe(Toeditmp3 *edit);   //find the required frame
Status readeditsize(Toeditmp3 *edit);    //frame size
Status writenewvalue(Toeditmp3 *edit);

#endif




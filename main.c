#include <stdio.h>                          // printf
#include <string.h>                         // strcmp
#include "header.h"                         // View structure and functions
#include "edit.h"                           // Edit structure and functions
#include <stdlib.h>                         // General utilities

int main(int argc,char *argv[])
{
    Toviewmp3 view;                         // Holds data for the view operation
    Toeditmp3 edit;                         // Holds data for the edit operation
    int status=0;                           // 0 = success, 1 = error

    // Help option: ./a.out -help
    if(argc==2 && strcmp(argv[1],"-help")==0){
        printf("\nTo view:\n");             // Heading for view usage
        printf("./a.out -v songname.mp3\n");    // View usage example (-v added)

        printf("\nTo edit:\n");             // Heading for edit usage
        printf("./a.out -e -t song.mp3 \"New Title\"\n");   // Edit title example
        printf("./a.out -e -a song.mp3 \"New Artist\"\n");  // Edit artist example
        printf("./a.out -e -A song.mp3 \"New Album\"\n");   // Edit album example
        printf("./a.out -e -y song.mp3 \"2026\"\n");        // Edit year example
        printf("./a.out -e -g song.mp3 \"Rock\"\n");        // Edit genre example
        printf("./a.out -e -c song.mp3 \"New Comment\"\n"); // Edit comment example
    }
    // View operation takes 3 arguments
    else if (argc == 3){
        if (strcmp(argv[1], "-v") == 0){    // Check for view option
            if (readandvalidate(argv, &view) == e_success){     // Validate arguments
                if (openviewfiles(&view) == e_success){         // Open the file
                    if (toview(&view) == e_failure){            // Display the tags
                        printf("Unable to view the tag\n");     // Report failure
                        status=1;
                    }
                }else{
                    printf("Unable to open the mp3 file\n");    // File open error
                    status=1;
                }
            }else{
                printf("Invalid view argument\n");              // Bad arguments
                printf("Enter ./a.out -help to take reference\n"); // Show help hint
                status=1;
            }
        }else{
            printf("Invalid option, expected -v\n");    // Option is not -v
            status=1;
        }
    }

    // Edit operation takes 5 arguments
    else if(argc==5){
        if(strcmp(argv[1],"-e")==0){        // Check for edit option
            if(readandvalidateedit(argv,&edit)==e_success){     // Validate arguments
                if(openeditfile(&edit)==e_success){             // Open the file
                    if(toedit(&edit)==e_failure){               // Perform the edit
                        printf("Unable to edit the tag\n");     // Report failure
                        status=1;
                    }
                }else{
                    printf("Unable to open the mp3 file\n");    // File open error
                    status=1;
                }
            }else{
                printf("Invalid edit argument\nEnter ./a.out -help to take reference\n"); // Bad arguments
                status=1;
            }
        }else{
            printf("Invalid option, expected -e\n");    // Option is not -e
            status=1;
        }
    }
    else{
        printf("Invalid argument\nEnter ./a.out -help to take reference\n"); // Wrong argument count
        status=1;
    }

    return status;                          // End of program
}
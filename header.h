#ifndef VIEW_H                              // Prevent multiple inclusion of this header
#define VIEW_H                              // Define the guard macro

#include <stdio.h>                          // FILE type and I/O functions
#include <string.h>                         // String functions
#include "type.h"                           // Status enum (e_success / e_failure)

// Structure holding all data needed for the view operation
typedef struct
{
    char *srcmp3_fname;                     // Name of the mp3 file given by the user
    FILE *fptr_mp3;                         // File pointer to the opened mp3 file
    char tag[5];                            // Frame ID (4 chars + null), currently unused
    unsigned int size;                      // Size of the current frame
    char content[300];                      // Buffer to hold the frame content
}Toviewmp3;                                 // Type name used in the view functions

Status readandvalidate(char *argv[],Toviewmp3 *Toview);     // Validate view arguments
Status openviewfiles(Toviewmp3 *Toview);    // Open the mp3 file for reading
Status toview(Toviewmp3 *Toview);           // Main view function that calls the steps in order
Status checkid3(Toviewmp3 *Toview);         // Check that the file starts with "ID3"
Status checkversion(Toviewmp3 *Toview);     // Read and print the ID3 version
Status skipheader(Toviewmp3 *Toview);       // Skip the 10-byte ID3 header
Status readsize(Toviewmp3 *Toview);         // Read the 4-byte frame size
Status readcontents(Toviewmp3 *Toview);     // Read the frame content (currently not called)
Status mp3view(Toviewmp3 *Toview);          // Read the frames and print the tags

#endif                                      // End of header guard
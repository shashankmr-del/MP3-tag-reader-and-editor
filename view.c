#include <stdio.h>                          // printf, fread, fseek
#include <string.h>                         // strcmp
#include <stdlib.h>                         // general utilities
#include "header.h"                         // Toviewmp3 structure and prototypes

// Validate command line arguments for view
Status readandvalidate(char *argv[],Toviewmp3 *Toview){
    if(strcmp(argv[1],"-v")!=0){            // Option must be -v
        return e_failure;
    }
    char *dot=strrchr(argv[2],'.');         // Find the last dot in the name
    if(dot==NULL || strcmp(dot,".mp3")!=0){ // File must end with .mp3
        return e_failure;
    }
    Toview->srcmp3_fname=argv[2];           // Save the file name
    return e_success;
}

// Open the mp3 file for reading
Status openviewfiles(Toviewmp3 *Toview){
    Toview->fptr_mp3=fopen(Toview->srcmp3_fname,"rb");  // Open in binary read mode
    if(Toview->fptr_mp3==NULL){             // Check whether the open worked
        return e_failure;
    }
    return e_success;
}

// Check whether the file begins with "ID3"
Status checkid3(Toviewmp3 *Toview){
    char id[4];                             // Buffer for 3 chars + null
    rewind(Toview->fptr_mp3);               // Move file pointer to beginning
    if(fread(id,1,3,Toview->fptr_mp3)!=3){  // Read first 3 bytes
        return e_failure;
    }
    id[3]='\0';                             // Add null character

    if(strcmp(id,"ID3")==0){                // Compare with the signature
        return e_success;
    }
    return e_failure;
}

// Read and print the ID3 version
Status checkversion(Toviewmp3 *Toview){
    unsigned char version[2];               // Major and minor version bytes
    if(fread(version,1,2,Toview->fptr_mp3)!=2){
        return e_failure;
    }
    printf("ID3 Version : 2.%d.%d\n",version[0],version[1]);    // e.g. 2.3.0
    return e_success;
}

// Skip the 10-byte ID3 header
Status skipheader(Toviewmp3 *Toview){
    if(fseek(Toview->fptr_mp3,10,SEEK_SET)!=0){     // Jump to byte 10 from the start
        return e_failure;                   // Seek failed
    }
    return e_success;
}

// Read the 4-byte frame size
Status readsize(Toviewmp3 *Toview){
    unsigned char size[4];                  // Buffer for size bytes
    if(fread(size,1,4,Toview->fptr_mp3)!=4){
        return e_failure;
    }
    // Combine the bytes into one integer (big endian)
    Toview->size=((unsigned int)size[0]<<24) | ((unsigned int)size[1]<<16) |
                 ((unsigned int)size[2]<<8)  | (unsigned int)size[3];
    return e_success;
}

// Read the frame content, skipping the encoding byte
Status readcontents(Toviewmp3 *Toview){
    if(Toview->size==0 || Toview->size>=sizeof(Toview->content)){
        return e_failure;                   // Size is zero or too big for the buffer
    }
    fseek(Toview->fptr_mp3,1,SEEK_CUR);     // Skip encoding byte
    fread(Toview->content,1,Toview->size-1,Toview->fptr_mp3);   // Read the content
    Toview->content[Toview->size-1]='\0';   // Add null character
    return e_success;
}

// Read the frames and print the known tags
Status mp3view(Toviewmp3 *Toview){
    char frame_id[5];                       // Buffer for frame ID + null

    for(int i=0;i<6;i++){                   // Read up to 6 frames
        if(fread(frame_id,1,4,Toview->fptr_mp3)!=4){    // Read the frame ID
            break;                          // End of file reached
        }
        frame_id[4]='\0';                   // Null terminate the ID
        if(frame_id[0]=='\0'){              // Padding reached, no more frames
            break;
        }

        if(readsize(Toview)==e_failure){    // Read frame size
            return e_failure;
        }
        fseek(Toview->fptr_mp3,2,SEEK_CUR); // Skip 2 flag bytes

        // Frame is too large for our buffer, so skip it safely
        if(Toview->size==0 || Toview->size>=sizeof(Toview->content)){
            fseek(Toview->fptr_mp3,Toview->size,SEEK_CUR);
            continue;
        }

        // Read frame content and make sure all bytes were read
        if(fread(Toview->content,1,Toview->size,Toview->fptr_mp3)!=Toview->size){
            return e_failure;
        }
        Toview->content[Toview->size]='\0'; // Add null character

        if(strcmp(frame_id,"TIT2")==0){         // Title
            printf("Title   : %s\n",Toview->content+1);     // +1 skips encoding byte
        }else if(strcmp(frame_id,"TPE1")==0){   // Artist
            printf("Artist  : %s\n",Toview->content+1);
        }else if(strcmp(frame_id,"TALB")==0){   // Album
            printf("Album   : %s\n",Toview->content+1);
        }else if(strcmp(frame_id,"TYER")==0){   // Year
            printf("Year    : %s\n",Toview->content+1);
        }else if(strcmp(frame_id,"TCON")==0){   // Genre
            printf("Genre   : %s\n",Toview->content+1);
        }else if(strcmp(frame_id,"COMM")==0){   // Comment
            printf("Comment : %s\n",Toview->content+1);
        }
    }
    return e_success;                       // All frames processed
}

// Main view routine that calls the steps in order
Status toview(Toviewmp3 *Toview){
    Status result=e_failure;                // Final result, fails until everything works

    printf("-------------------------------------------\n");
    printf("            MP3 TAG READER VIEW            \n");
    printf("-------------------------------------------\n");

    if(checkid3(Toview)==e_failure){        // Check ID3 tag
        printf("Error:ID3 tag not found\n");
    }else if(checkversion(Toview)==e_failure){      // Check ID3 version
        printf("Error:Unable to read ID3 version\n");
    }else if(skipheader(Toview)==e_failure){        // Skip the 10-byte header
        printf("Error:Unable to skip header\n");
    }else if(mp3view(Toview)==e_failure){   // Display tags
        printf("Error:Unable to read tags\n");
    }else{
        printf("-------------------------------------------\n");
        result=e_success;                   // Everything worked
    }

    fclose(Toview->fptr_mp3);               // Close the file in every case
    return result;
}
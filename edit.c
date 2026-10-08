#include <string.h>                         // string functions like strcmp, strcpy, strstr
#include <stdlib.h>                         // general utilities
#include "type.h"                           // Status enum (e_success / e_failure)
#include "edit.h"                           // Toeditmp3 structure and edit function prototypes

// Validate command line arguments and fill the edit structure
Status readandvalidateedit(char *argv[],Toeditmp3 *edit){
    // Make sure all required arguments are present
    if(argv[1]==NULL || argv[2]==NULL || argv[3]==NULL ||argv[4]==NULL){
        return e_failure;                   // Missing argument, so fail
    }

    // Check that the file name contains ".mp3"
    if(strstr(argv[3],".mp3")==NULL){
        return e_failure;                   // Not an mp3 file, so fail
    }

    // Title option -t maps to ID3 frame TIT2
    if(strcmp(argv[2],"-t")==0){
        strcpy(edit->tag,"TIT2");           // Store the title frame ID
    // Artist option -a maps to frame TPE1
    }else if(strcmp(argv[2],"-a")==0){
        strcpy(edit->tag,"TPE1");           // Store the artist frame ID
    // Album option -A maps to frame TALB
    }else if(strcmp(argv[2],"-A")==0){
        strcpy(edit->tag,"TALB");           // Store the album frame ID
    // Year option -y maps to frame TYER
    }else if(strcmp(argv[2],"-y")==0){
        strcpy(edit->tag,"TYER");           // Store the year frame ID
    // Genre option -g maps to frame TCON
    }else if(strcmp(argv[2],"-g")==0){
        strcpy(edit->tag,"TCON");           // Store the genre frame ID
    // Comment option -c maps to frame COMM
    }else if(strcmp(argv[2],"-c")==0){
        strcpy(edit->tag,"COMM");           // Store the comment frame ID
    }else{
        return e_failure;                   // Unknown option, so fail
    }

    edit->srcmp3_fname=argv[3];             // Save the mp3 file name
    edit->newvalue=argv[4];                 // Save the new tag value

    return e_success;                       // All arguments are valid
}

// Open the mp3 file in read + write mode
Status openeditfile(Toeditmp3 *edit){
    edit->fptr_mp3=fopen(edit->srcmp3_fname,"r+");  // Open file for reading and updating
    if(edit->fptr_mp3==NULL){               // Check whether the file opened
        return e_failure;                   // Open failed
    }

    return e_success;                       // File opened successfully
}

// Find the requested frame and edit it
Status toedit(Toeditmp3 *edit){
    char frame_id[5];                       // Buffer for 4-char frame ID + null
    rewind(edit->fptr_mp3);                 // Move file pointer to the beginning

    // Read first 3 bytes to check for ID3 tag
    fread(frame_id,1,3,edit->fptr_mp3);
    frame_id[3]='\0';                       // Null terminate so it can be compared

    if(strcmp(frame_id,"ID3")!=0){          // Verify the ID3 signature
        printf("Error:ID3 tag not found\n");// Tell user tag is missing
        fclose(edit->fptr_mp3);             // Close the file before leaving
        return e_failure;                   // Cannot edit without ID3 tag
    }
    // Skip remaining 7 header bytes (version, flags, size)
    fseek(edit->fptr_mp3,7,SEEK_CUR);

    // Loop through the first 6 frames
    for(int i=0;i<6;i++){
        fread(frame_id,1,4,edit->fptr_mp3); // Read the 4-byte frame ID
        frame_id[4]='\0';                   // Null terminate the frame ID
        if(readeditsize(edit)==e_failure){  // Read the frame size
            fclose(edit->fptr_mp3);         // Close the file before leaving
            return e_failure;               // Size read failed
        }

        fseek(edit->fptr_mp3,2,SEEK_CUR);   // Skip the 2 flag bytes
        if(strcmp(frame_id,edit->tag)==0){  // Is this the frame we want?
            printf("Tag found : %s\n",frame_id);    // Show which tag was found
            if(writenewvalue(edit)==e_failure){     // Overwrite the old value
                fclose(edit->fptr_mp3);     // Close the file before leaving
                return e_failure;           // Writing failed
            }
            printf("Tag edited\n");         // Confirm edit to user
            fclose(edit->fptr_mp3);         // Close the file
            return e_success;               // Edit done
        }
        fseek(edit->fptr_mp3,edit->size,SEEK_CUR); // Skip this frame's content to reach the next frame
    }
    printf("Error: Tag not found\n");       // None of the 6 frames matched
    fclose(edit->fptr_mp3);                 // Close the file
    return e_failure;                       // Tag was not found
}

// Read the frame size
Status readeditsize(Toeditmp3 *edit){
    unsigned char size[4];                  // Buffer for the 4 size bytes
    fread(size,1,4,edit->fptr_mp3);         // Read 4 size bytes from file
    // Combine the 4 bytes into one integer (big endian)
    edit->size=(size[0]<<24) | (size[1]<<16) | (size[2]<<8) | (size[3]);
    return e_success;                       // Size read successfully
}

// Write the new value into the frame
Status writenewvalue(Toeditmp3 *edit){
    unsigned int text_len=strlen(edit->newvalue);   // Length of the new text
    unsigned int new_len=text_len+1;        // Text length + 1 encoding byte

    // Check that the new value fits in the old frame
    if(new_len>edit->size){
        printf("New value is too long for this tag\n");     // Too long
        return e_failure;                   // Cannot overwrite safely
    }

    fputc(0,edit->fptr_mp3);                // Write encoding byte (0 = ISO-8859-1)
    fwrite(edit->newvalue,1,text_len,edit->fptr_mp3);       // Write the new text
    // Fill the remaining bytes of the frame with 0
    for(unsigned int i=new_len;i<edit->size;i++){
        fputc(0,edit->fptr_mp3);            // Write one padding byte
    }
    fflush(edit->fptr_mp3);                 // Flush data to the file
    return e_success;                       // Write completed
}
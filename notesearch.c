#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>  //this header must be included, otherwise the POSIX function 'lseek' used in find_user_note doesn't work, because its return type 'off_t' is defined in this header. If not included, the compiler tries to use 'int' instead of 'off_t', but lseek will not work in this way. The functions 'open/read/write/close/lseek' are low-level system calls which only work on platforms that support them, while 'fopen/fread/fputs/fseek...' are buffered I/O standard C-library calls that will work on any platform.
#include "hacking.h"

#define FILENAME "/tmp/notes"

int print_notes(int, int, char *);
int find_user_note(int, int);
int search_note(char *, char *);
void fatal(char *);

int main(int argc, char *argv[]){
	int userid, printing = 1, fd; //file descriptor
	char searchstring[100];

	if(argc > 1)
		strcpy(searchstring, argv[1]);
	else
		searchstring[0] = 0;   //search string is empty
	
	userid = getuid();
	fd = open(FILENAME, O_RDONLY); //open the file for read-only access
	if(fd == -1)
		fatal("in main() while opening file for reading");
	
	while(printing)
		printing = print_notes(fd, userid, searchstring);
	printf("-------[ end of note data ]--------\n");
	close(fd);
	return 0;
}

//A function to print the notes for a given uid that match an optional search string;
//returns 0 at end of file, 1 if there are still more notes
int print_notes(int fd, int uid, char *searchstring){
	int note_length;
	char byte = 0, note_buffer[100];

	note_length = find_user_note(fd, uid);
	if(note_length == -1) //if end of file reached,
		return 0;		  //return 0.
	
	printf("reading note data\n");
	read(fd, note_buffer, note_length); //read note data
	note_buffer[note_length] = 0; //terminate the string

	printf("[DEBUG] note_buffer is \'%s\', keyword is \'%s\'\n", note_buffer, searchstring);
	if(search_note(note_buffer, searchstring)){ //if searchstring found,
		
		printf(note_buffer); //print the note.
	}
	return 1;
}

//A function to find the next note for a given userID
//returns -1 if end of file reached;
//otherwise returns the length of the found note.
int find_user_note(int fd, int user_uid){
	int note_uid = -1;
	unsigned char byte;
	int length;
	while(note_uid != user_uid) { //loop until a note for user_uid is found.
		if(read(fd, &note_uid, 4) != 4) //read the uid data
			return -1; //if 4 bytes aren't read, return end of file code.
		if(read(fd, &byte, 1) != 1) //read the newline separator.
			return -1;

		byte = length = 0;
		while(byte != '\n') { //figure out how many bytes to the end of line.
			if(read(fd, &byte, 1) != 1) //read a single byte.
				return -1; //if byte isn't read, return end of file code.
			length++;
		}
	}
	printf("[DEBUG] file position before lseek: %d\n", lseek(fd, 0, SEEK_CUR)); 
	lseek(fd, length*-1, SEEK_CUR); //rewind file reading by length bytes.
	printf("[DEBUG] file position after lseek: %d\n", lseek(fd, 0, SEEK_CUR));

	printf("[DEBUG] found a %d byte note for user_d %d\n", length, note_uid);
	return length;
}

//a function to search a note for a given keywork
//returns 1 if a match is found, 0 if there is no match.
int search_note(char *note, char *keyword){
	int i, keyword_length, match = 0;
	keyword_length = strlen(keyword);
	if(keyword_length == 0) //if there is no search string,
		return 1; //always 'match'.
	
	for(i = 0; i < strlen(note); i++) {
		if(note[i] == keyword[match]) //if byte matches keyword,
			match++; //get ready to check the next byte
		else{
			if(note[i] == keyword[0]) // if that byte matches first keyword byte,
				match = 1; //start the match count at 1.
			else
				match = 0;
		}
		if(match == keyword_length)
			return 1;
	}
	return 0;
}

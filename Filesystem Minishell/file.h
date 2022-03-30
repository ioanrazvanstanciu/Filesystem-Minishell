#ifndef __FILE_H__
#define __FILE_H__

typedef struct Directory {
	// The name of the directory
	char *name;

	// TODO: The list of files of the current directory
	struct Node_file *files;

	// TODO: The list of directories of the current directory
	struct Node_directory *directories;

	// The parent directory of the current directory
	// (NULL for the root directory)
	struct Directory *parentDir;
} Directory;

// DO NOT MODIFY THIS STRUCTURE
typedef struct File {
	// The name of the file
	char *name;

	// The size of the file
	int size;

	// The content of the file
	char *data;

	// The directory in which the file is located
	Directory *dir;
} File;

typedef struct Node_directory {
	struct Directory data;

	struct Node_directory *next;
} Node_directory;

typedef struct Node_file {
	struct File data;

	struct Node_file *next;
} Node_file;

#endif /* __FILE_H__ */

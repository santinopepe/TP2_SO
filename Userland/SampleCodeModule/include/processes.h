#include <stdint.h>
#include <stddef.h>
#include <syscalls.h>
#include <globals.h>
#include <shell.h>
#include <stdio.h>


 int wc(int argc, char *argv[]);
 int cat(int argc, char *argv[]);
 void filter(int argc, char *argv[]);
 void ps(int argc, char *argv[]);
 void loop(int argc, char *argv[]);
 void mem(int argc, char *argv[]);
 void kill(int argc, char *argv[]);
 int niceWrapper(int argc, char *argv[]);
 void blockProcessWrapper(int argc, char *argv[]);
 void unblockProcessWrapper(int argc, char *argv[]);
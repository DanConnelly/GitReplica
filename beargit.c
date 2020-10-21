#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "beargit.h"
#include "util.h"

/* Implementation Notes:
 *
 * - Functions return 0 if successful, 1 if there is an error.
 * - All error conditions in the function description need to be implemented
 *   and written to stderr. We catch some additional errors for you in main.c.
 * - Output to stdout needs to be exactly as specified in the function description.
 * - Only edit this file (beargit.c)
 * - Here are some of the helper functions from util.h:
 *   * fs_mkdir(dirname): create directory <dirname>
 *   * fs_rm(filename): delete file <filename>
 *   * fs_mv(src,dst): move file <src> to <dst>, overwriting <dst> if it exists
 *   * fs_cp(src,dst): copy file <src> to <dst>, overwriting <dst> if it exists
 *   * write_string_to_file(filename,str): write <str> to filename (overwriting contents)
 *   * read_string_from_file(filename,str,size): read a string of at most <size> (incl.
 *     NULL character) from file <filename> and store it into <str>. Note that <str>
 *     needs to be large enough to hold that string.
 *  - You NEED to test your code. The autograder we provide does not contain the
 *    full set of tests that we will run on your code. See "Step 5" in the project spec.
 */

/* beargit init
 *
 * - Create .beargit directory
 * - Create empty .beargit/.index file
 * - Create .beargit/.prev file containing 0..0 commit id
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_init(void) {
  fs_mkdir(".beargit");

  FILE* findex = fopen(".beargit/.index", "w");
  fclose(findex);

  FILE* fbranches = fopen(".beargit/.branches", "w");
  fprintf(fbranches, "%s\n", "master");
  fclose(fbranches);

  write_string_to_file(".beargit/.prev", "0000000000000000000000000000000000000000");
  write_string_to_file(".beargit/.current_branch", "master");

  return 0;
}



/* beargit add <filename>
 *
 * - Append filename to list in .beargit/.index if it isn't in there yet
 *
 * Possible errors (to stderr):
 * >> ERROR:  File <filename> has already been added.
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_add(const char* filename) {
  FILE* findex = fopen(".beargit/.index", "r");
  FILE* fnewindex = fopen(".beargit/.newindex", "w");

  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0) {
      fprintf(stderr, "ERROR:  File %s has already been added.\n", filename);
      fclose(findex);
      fclose(fnewindex);
      fs_rm(".beargit/.newindex");
      return 3;
    }

    fprintf(fnewindex, "%s\n", line);
  }

  fprintf(fnewindex, "%s\n", filename);
  fclose(findex);
  fclose(fnewindex);

  fs_mv(".beargit/.newindex", ".beargit/.index");

  return 0;
}

/* beargit status
 *
 * See "Step 1" in the project spec.
 *
 */

int beargit_status() {
  /* COMPLETE THE REST */
  fprintf(stdout, "%s\n\n", "Tracked files:");
  FILE* findex = fopen(".beargit/.index", "r");
  char line[FILENAME_SIZE];
  int counter = 0;
  while(fgets(line, sizeof(line),findex)) {
    counter++;
    strtok(line,"\n");
    fprintf(stdout,"%s\n", line);
  }
  fclose(findex);
  /* DO SOMETHING */
  fprintf(stdout, "\n");
  fprintf(stdout, "There are %d files total.\n", counter);
  return 0;
}

/* beargit rm <filename>
 *
 * See "Step 2" in the project spec.
 *
 */

int beargit_rm(const char* filename) {
  /* COMPLETE THE REST */
  FILE* findex = fopen(".beargit/.index", "r");
  FILE* fnewindex = fopen(".beargit/.newindex", "w");
  int check = 0;
  char line[FILENAME_SIZE];
  while (fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0) {
      check = 1;
    } else {
      fprintf(fnewindex, "%s\n", line);
    }
  }

  fclose(findex);
  fclose(fnewindex);
  
  if (check==0) {
      fprintf(stderr,"ERROR:  File %s not tracked.\n", filename);
      return 1;
  } else {
    fs_mv(".beargit/.newindex", ".beargit/.index");  
  }
  return 0;
}

/* beargit commit -m <msg>
 *
 * See "Step 3" in the project spec.
 *
 */

const char* go_bears = "THIS IS BEAR TERRITORY!";

int is_commit_msg_ok(const char* msg) {
  /* COMPLETE THE REST */
  
  int i = 0;
  int bearval = find_msg_length(go_bears);
  int msgval = find_msg_length(msg);
  /* If go_bears is longer than msg, then automatic ban! */
  if (bearval > msgval) {
    return 0;
  } else {
    /* Loop through the string until end to see if you fit. Nothing special */
    for (int j = 0; j < msgval; j++) {
      if (msg[j] == go_bears[i]) {
        /* If you get a match to letter 0 in go_bears, great! Increment go_bears index. */
        i++;
      } else {
        /* If you fail, reset to 0 for go_bears */
        i = 0;
      }
      if (i == bearval) {
        /*" If you cannot see anymore in go_bears, great you're done!*/
        break;
      }
      if (j == msgval - 1) {
        /* If you cannot see anymore in msg, you're a failure!*/
        return 0;
      }
    }
  }
  return 1;
}

// Making me some msg length helper function
int find_msg_length(const char* msg) {
  int count = 0; 
  for (int i = 0; msg[i] != 0; i++) {
    count++;
  }
  return count;
}

/* Use next_commit_id to fill in the rest of the commit ID.
 *
 * Hints:
 * You will need a destination string buffer to hold your next_commit_id, before you copy it back to commit_id
 * You will need to use a function we have provided for you.
 */

void next_commit_id(char* commit_id) {
     /* COMPLETE THE REST */
  char line[COMMIT_ID_SIZE];
  char brline[BRANCHNAME_SIZE];
  char hline[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", line, sizeof(line));
  read_string_from_file(".beargit/.current_branch", brline, sizeof(brline));
  cryptohash(brline, hline);
  for (int i = 0; i < strlen(hline); i++ ) {
    char a[2];
    a[0] = hline[i];
    a[1] = '\0';
    char b[2];
    b[0] = line[i];
    b[1] = '\0';
    long int val = strtol(a, NULL, 16) + strtol(b, NULL, 16);
    sprintf(&commit_id[i], "%01x", (unsigned int) val);
  }
  commit_id[strlen(hline)] = '\0';
}




int beargit_commit(const char* msg) {
  if (!is_commit_msg_ok(msg)) {
    fprintf(stderr, "ERROR:  Message must contain \"%s\"\n", go_bears);
    return 1;
  }

  char commit_id[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  next_commit_id(commit_id);

  /* COMPLETE THE REST */
  char* ncommit = (char*) calloc(FILENAME_SIZE, sizeof(char));
  strcat(ncommit, ".beargit/");
  strcat(ncommit, commit_id);
  char* ncommitprev = (char*) calloc(FILENAME_SIZE, sizeof(char));
  char* ncommitindex = (char*) calloc(FILENAME_SIZE, sizeof(char));
  char* ncommitmsg = (char*) calloc(FILENAME_SIZE, sizeof(char));
  strcat(ncommitmsg, ncommit);
  strcat(ncommitmsg, "/.msg");
  strcat(ncommitprev, ncommit);
  strcat(ncommitprev, "/.prev");
  strcat(ncommitindex, ncommit);
  strcat(ncommitindex, "/.index");

  fs_mkdir(ncommit);
  fs_cp(".beargit/.prev", ncommitprev);
  fs_cp(".beargit/.index", ncommitindex); 
  write_string_to_file(ncommitmsg, msg);
  write_string_to_file(".beargit/.newprev", commit_id);
  fs_mv(".beargit/.newprev", ".beargit/.prev");
  FILE *findex = fopen(".beargit/.index", "r");

  char line[FILENAME_SIZE];

  while (fgets(line, sizeof(line), findex)) {
    char dst[FILENAME_SIZE]; 
    strtok(line, "\n");
    
    strcpy(dst, ncommit);
    strcat(dst, "/");
    strcat(dst, line);
    fs_cp(line, dst);
  }
  fclose(findex);
  free(ncommit);
  free(ncommitprev);
  free(ncommitindex);
  return 0;
}

/* beargit log
 *
 * See "Step 4" in the project spec.
 *
 */

int beargit_log(int limit) {
  /* COMPLETE THE REST */
  char commit_id[COMMIT_ID_SIZE];
  int count = 0;
  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  if (strcmp(commit_id, "0000000000000000000000000000000000000000") == 0) {
    fprintf(stderr, "ERROR:  There are no commits.\n");
    return 1;
  }
  while (strcmp(commit_id, "0000000000000000000000000000000000000000") != 0) {
    /* Create a string of the prev commit id directory */
    if (count == limit) {
      break;
    }
    char *prev_id = (char*) calloc(FILENAME_SIZE, sizeof(char));
    char *prev_msg = (char*) calloc(FILENAME_SIZE, sizeof(char));
    char *prev_prev = (char*) calloc(FILENAME_SIZE, sizeof(char));
    char *prev_msg_msg = (char*) calloc(MSG_SIZE, sizeof(char));
    strcat(prev_id, ".beargit/");
    strcat(prev_id, commit_id);
    strcat(prev_prev, prev_id);
    strcat(prev_prev, "/.prev");
    strcat(prev_msg, prev_id);
    strcat(prev_msg, "/.msg");
    read_string_from_file(prev_msg, prev_msg_msg, MSG_SIZE);
    fprintf(stdout, "commit %s\n", commit_id);
    fprintf(stdout, "   %s\n\n", prev_msg_msg);


    read_string_from_file(prev_prev, commit_id, COMMIT_ID_SIZE);
    count++;
    free(prev_id);
    free(prev_msg);
    free(prev_prev);
    free(prev_msg_msg);
  }
  return 0;
}


// This helper function returns the branch number for a specific branch, or
// returns -1 if the branch does not exist.
int get_branch_number(const char* branch_name) {
  FILE* fbranches = fopen(".beargit/.branches", "r");

  int branch_index = -1;
  int counter = 0;
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), fbranches)) {
    strtok(line, "\n");
    if (strcmp(line, branch_name) == 0) {
      branch_index = counter;
    }
    counter++;
  }

  fclose(fbranches);

  return branch_index;
}

/* beargit branch
 *
 * See "Step 5" in the project spec.
 *
 */

int beargit_branch() {
  /* COMPLETE THE REST */
  char* cbranch = (char*) calloc(BRANCHNAME_SIZE, sizeof(char));
  char* branch = (char*) calloc (BRANCHNAME_SIZE, sizeof(char));
  read_string_from_file(".beargit/.current_branch", cbranch, BRANCHNAME_SIZE);
  int current = get_branch_number(cbranch);
  int count = 0;
  FILE *fbranches = fopen(".beargit/.branches", "r");
  while (fgets(branch, sizeof(branch), fbranches)) {
    if (count == current) {
      strtok(branch, "\n");
      fprintf(stdout, "*  %s\n", branch);
    } else {
      strtok(branch, "\n");
      fprintf(stdout, "   %s\n", branch);
      count++;
    }
  }
  return 0;
}

/* beargit checkout
 *
 * See "Step 6" in the project spec.
 *
 */

int checkout_commit(const char* commit_id) {
  /* COMPLETE THE REST */
  // Look at index to see which files are tracked
  FILE* findex = fopen(".beargit/.index", "r");
  char *oldfile = (char*) calloc(FILENAME_SIZE, sizeof(char));

  while (fgets(oldfile, sizeof(oldfile), findex)) {
    char *ofile = (char*) calloc(FILENAME_SIZE, sizeof(char));
    strtok(oldfile, "\n");
    strcat(ofile, oldfile);
    fs_rm(ofile);
    free(ofile);
  }
  free(oldfile);
  fclose(findex);
  fs_rm(".beargit/.index");
  // cp new index to old index and do stuff if not 0.00
  if (!is_it_a_commit_id(commit_id)) {
    return 0;
  }
  char *ndirectory = (char*) calloc(FILENAME_SIZE, sizeof(char));
  strcat(ndirectory, ".beargit/");
  strcat(ndirectory, commit_id);
  char *ndirindex = (char*) calloc(FILENAME_SIZE, sizeof(char));
  strcat(ndirindex, ndirectory);
  strcat(ndirindex, "/.index");
  fs_cp(ndirindex, ".beargit/.index");

  FILE *fnewindex = fopen(".beargit/.index", "r");
  char *nfile = (char*) calloc(FILENAME_SIZE, sizeof(char));
  while (fgets(nfile, sizeof(nfile), fnewindex)) {
    char *newfile = (char*) calloc(FILENAME_SIZE, sizeof(char));
    char *newplace = (char*) calloc(FILENAME_SIZE, sizeof(char));
    strtok(nfile, "\n");
    strcat(newplace, nfile);
    strcat(newfile, ndirectory);
    strcat(newfile, "/");
    strcat(newfile, nfile);
    fs_cp(newfile, newplace);
    free(newfile);
    free(newplace);
  }
  free(nfile);
  fclose(fnewindex);

  write_string_to_file(".beargit/.prev", commit_id);
  


  free(ndirectory);
  return 0;
}

int is_it_a_commit_id(const char* commit_id) {
  /* COMPLETE THE REST */
  char* id_dir = (char*) calloc(FILENAME_SIZE, sizeof(char));
  strcat(id_dir, ".beargit/");
  strcat(id_dir, commit_id);
  if (!fs_check_dir_exists(id_dir)) {
    return 0;
  }
  return 1;
}

int beargit_checkout(const char* arg, int new_branch) {
  // Get the current branch
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);

  // If not detached, leave the current branch by storing the current HEAD into that branch's file...
  if (strlen(current_branch)) {
    char current_branch_file[BRANCHNAME_SIZE+50];
    sprintf(current_branch_file, ".beargit/.branch_%s", current_branch);
    fs_cp(".beargit/.prev", current_branch_file);
  }

   // Check whether the argument is a commit ID. If yes, we just change to detached mode
  // without actually having to change into any other branch.
  if (is_it_a_commit_id(arg)) {
    char commit_dir[FILENAME_SIZE] = ".beargit/";
    strcat(commit_dir, arg);
    // ...and setting the current branch to none (i.e., detached).
    write_string_to_file(".beargit/.newcurrent_branch", "");
    fs_mv(".beargit/.newcurrent_branch", ".beargit/current_branch");

    return checkout_commit(arg);
  }



  // Read branches file (giving us the HEAD commit id for that branch).
  int branch_exists = (get_branch_number(arg) >= 0);

  // Check for errors.
  if (!(!branch_exists || !new_branch)) {
    fprintf(stderr, "ERROR:  A branch named %s already exists.\n", arg);
    return 1;
  } else if (!branch_exists && !new_branch) {
    fprintf(stderr, "ERROR:  No branch or commit %s exists.\n", arg);
    return 1;
  }

  // Just a better name, since we now know the argument is a branch name.
  const char* branch_name = arg;

  // File for the branch we are changing into.
  char branch_file[BRANCHNAME_SIZE+50];
  sprintf(branch_file, ".beargit/.branch_%s", branch_name);


  // // Update the branch file if new branch is created (now it can't go wrong anymore)
  if (new_branch) {
    FILE* fbranches = fopen(".beargit/.branches", "a");
    fprintf(fbranches, "%s\n", branch_name);
    fclose(fbranches);
    fs_cp(".beargit/.prev", branch_file);
  }

  write_string_to_file(".beargit/.current_branch", branch_name);

  // Read the head commit ID of this branch.
  char branch_head_commit_id[COMMIT_ID_SIZE];
  read_string_from_file(branch_file, branch_head_commit_id, COMMIT_ID_SIZE);

  // Check out the actual commit.
  return checkout_commit(branch_head_commit_id);
}

/* beargit reset
 *
 * See "Step 7" in the project spec.
 *
 */

int beargit_reset(const char* commit_id, const char* filename) {
  if (!is_it_a_commit_id(commit_id)) {
      fprintf(stderr, "ERROR:  Commit %s does not exist.\n", commit_id);
      return 1;
  }

  // Check if the file is in the commit directory
  /* COMPLETE THIS PART */
  char *dirindex = (char*) calloc(FILENAME_SIZE, sizeof(char));
  strcat(dirindex, ".beargit/");
  strcat(dirindex, commit_id);
  strcat(dirindex, "/.index");
  FILE *findex = fopen(dirindex, "r");
  char *file = (char*) calloc(FILENAME_SIZE, sizeof(char));
  int present = 0;
  while (fgets(file, sizeof(file), findex)) {
    strtok(file, "\n");
    if (strcmp(file, filename) == 0) {
      present = 1;
      break;
    }
  }
  fclose(findex);
  free(dirindex);
  free(file);
  if (!present) {
    fprintf(stderr, "ERROR:  %s is not in the index of commit %s.\n", filename, commit_id);
    return 1;
  }
  // Copy the file to the current working directory
  /* COMPLETE THIS PART */
  char *dirfile = (char*) calloc(FILENAME_SIZE, sizeof(char));
  strcat(dirfile, ".beargit/");
  strcat(dirfile, commit_id);
  strcat(dirfile, "/");
  strcat(dirfile, filename);
  char *herefile = (char*) calloc(FILENAME_SIZE, sizeof(char));
  fs_cp(dirfile, filename);
  // Add the file if it wasn't already there
  /* COMPLETE THIS PART */
  FILE *thisindex = fopen(".beargit/.index", "r");
  char *line = calloc(FILENAME_SIZE, sizeof(char));
  int pres = 0;
  while(fgets(line, sizeof(line), thisindex)) {
    strtok(line, "\n");
    if (strcmp(filename, line) == 0) {
      pres = 1;
      break;
    }
  }
  fclose(thisindex);
  free(line);
  if (!pres) {
    beargit_add(filename);
  }
  return 0;
}

/* beargit merge
 *
 * See "Step 8" in the project spec.
 *
 */

int beargit_merge(const char* arg) {
  // Get the commit_id or throw an error
  char commit_id[COMMIT_ID_SIZE];
  if (!is_it_a_commit_id(arg)) {
      if (get_branch_number(arg) == -1) {
            fprintf(stderr, "ERROR:  No branch or commit %s exists.\n", arg);
            return 1;
      }
      char branch_file[FILENAME_SIZE];
      snprintf(branch_file, FILENAME_SIZE, ".beargit/.branch_%s", arg);
      read_string_from_file(branch_file, commit_id, COMMIT_ID_SIZE);
  } else {
      snprintf(commit_id, COMMIT_ID_SIZE, "%s", arg);
  }

  // Iterate through each line of the commit_id index and determine how you
  // should copy the index file over
   /* COMPLETE THE REST */
  char *commit_index = (char*) calloc(FILENAME_SIZE, sizeof(char));
  strcat(commit_index, ".beargit/");
  strcat(commit_index, commit_id);
  strcat(commit_index, "/.index");
  FILE *cindex = fopen(commit_index, "r");
  char cline[FILENAME_SIZE];
  while (fgets(cline, sizeof(cline), cindex)) {
    FILE *findex = fopen(".beargit/.index", "r");
    strtok(cline, "\n");
    char line[FILENAME_SIZE];
    int present = 0;
    while (fgets(line, sizeof(line), findex )) {
      strtok(line, "\n");
      if (strcmp(line, cline) == 0) {
        fprintf(stdout, "%s conflicted copy created\n", line);
        present = 1;
        char *commit_file = (char*) calloc(FILENAME_SIZE, sizeof(char));
        strcat(commit_file, ".beargit/");
        strcat(commit_file, commit_id);
        strcat(commit_file, "/");
        strcat(commit_file, cline);
        char* filetomake = (char*) calloc(FILENAME_SIZE, sizeof(char));
        strcat(filetomake, line);
        strcat(filetomake, ".");
        strcat(filetomake, commit_id);
        fs_cp(commit_file, filetomake);
        free(commit_file);
        free(filetomake);
        break;
      }
    }
    
    if (present) {
    
      fclose(findex);
    
    } else {
      fprintf(stdout, "%s added\n", cline);
      char *com_file = (char*) calloc(FILENAME_SIZE, sizeof(char));
      strcat(com_file, ".beargit/");
      strcat(com_file, commit_id);
      strcat(com_file, "/");
      strcat(com_file, cline);
      fs_cp(com_file, cline);
      beargit_add(cline);

      free(com_file);
      fclose(findex);
    }

  }
  // free(commit_index);
  // free(cline);
  fclose(cindex);
  return 0;
}

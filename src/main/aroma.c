/*
 * Copyright (C) 2011 Ahmad Amarullah ( http://amarullz.com/ )
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Descriptions:
 * -------------
 * Main executable for AROMA FILEMANAGER Binary
 *
 */
#include <sys/reboot.h>
#ifndef _AROMA_NODEBUG
#include <mcheck.h>
#endif
#include "../aroma.h"

//*
//* GLOBAL UI VARIABLES
//*
static  FILE  * acmd_pipe;

//*
//* Pass Recovery PIPE
//*
FILE * apipe() {
  return acmd_pipe;
}

//*
//* Show Text Splash
//*
void a_splash() {
  acmd_pipe   = stdout;
  setlinebuf(acmd_pipe);
  //#-- Print Info Into Recovery
  fprintf(apipe(), "ui_print\n");
  fprintf(apipe(), "ui_print " AROMA_NAME " version " AROMA_VERSION "\n");
  fprintf(apipe(), "ui_print\n");
  fprintf(apipe(), "ui_print " AROMA_COPY "\n");
  fprintf(apipe(), "ui_print\n");
  fprintf(apipe(), "ui_print\n");
  usleep(1000000);
}

//*
//* Init All Resources
//*
void a_init_all() {
  //-- Init Graphic Framebuffer
  ag_init();
  LOGS("Graph Initialized\n");
  //-- Init Input Event Handler
  ui_init();
  LOGS("Input Initialized\n");
}

//*
//* Release All Resources
//*
void a_release_all() {
  //-- Release Fonts
  ag_closefonts();
  LOGS("Font Released\n");
  //-- Release Input Engine
  ev_exit();
  LOGS("Input Released\n");
  //-- Release Zip Handler
  az_close();
  LOGS("Archive Released\n");
  //-- Release Graph Engine
  ag_close();
  LOGS("Graph Released\n");
}

//*
//* AROMA FILEMANAGER Main Executable
//*
int main(int argc, char ** argv) {
  //* MEMORY DEBUG
#ifndef _AROMA_NODEBUG
  remove_directory("/tmp/aroma-memory");
  aroma_memory_debug_init();
#endif

  int retval = 1;
  LOGS("Initializing\n");
  //* Normal Updater Sequences
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);
  //* Init Temporary Directory
  remove_directory(AROMA_TMP);
  create_directory(AROMA_TMP);
  
  //* Init Pipe & Show Splash Info
  a_splash();
  //* Init Zip
  LOGS("Open Archive\n");
  
  if (az_init("/", 1)) {
    //* Initializing All Resources
    LOGS("Initializing Resource\n");
    a_init_all();
    
    //* Starting AROMA FILEMANAGER UI
    LOGS("Starting Interface\n");
    
    if (aui_start()) {
      fprintf(apipe(), "ui_print\n");
      fprintf(apipe(), "ui_print " AROMA_NAME " Finished...\n");
      fprintf(apipe(), "ui_print\n");
      retval = 0;
    }
    
    //* Close Graph Thread
    LOGS("Close Graph Thread\n");
    ag_close_thread();
    //* Wait Thread Exit
    usleep(300000);
    //* Release All Resource
    LOGS("Starting Release\n");
    a_release_all();
    
    //* Wait Until Clean Up
    usleep(200000);
  }
  else {
    LOGE("Cannot Open Archive\n");
  }
  
  //* REMOVE AROMA TEMPORARY
  LOGS("Cleanup Temporary\n");
  usleep(500000);
  remove_directory(AROMA_TMP);

  //* MEMORY DEBUG
#ifndef _AROMA_NODEBUG
  aroma_dump_malloc();
#endif
  //-- Return Exit Status
  return retval;
}

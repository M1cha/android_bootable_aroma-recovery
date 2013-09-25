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
 * minzip wrapper for AROMA FILEMANAGER
 *
 */

#include "../../libs/minzip/Zip.h"
#include "../aroma.h"
#include <dirent.h>

/*****************************[ GLOBAL VARIABLES ]*****************************/
static ZipArchive zip;
char * fsPath;
int use_filesystem = 0;

/*********************************[ FUNCTIONS ]********************************/

//-- Crawl Zip
AZREADDIRP az_readdir(char * path) {
  AARRAYP res = aarray_create();
  int i;

  if(!use_filesystem) {
    int pathlen = strlen(path);
    char * pbuf = malloc(pathlen + 1);
    memset(pbuf, 0, pathlen + 1);

    for (i = 0; i < zip.numEntries; i++) {
      ZipEntry * ze = &zip.pEntries[i];
      
      if (ze->fileNameLen > pathlen) {
        memcpy(pbuf, ze->fileName, pathlen);
        if (strcmp(pbuf, path) == 0) {
          int    fl = ze->fileNameLen - pathlen + 1;
          char * fn = malloc(fl);
          memset(fn, 0, fl);
          int j;
          byte t = 0;

          for (j = pathlen; j < ze->fileNameLen; j++) {
            if (ze->fileName[j] == '/') {
              t = 1;
              break;
            }

            fn[j - pathlen] = ze->fileName[j];
          }
          
          fn[j - pathlen] = 0;
          aarray_set(res, fn, (t ? "1" : "0"));
          free(fn);
        }
      }
    }
  }
  else {
    DIR *dp;
    struct dirent *ep;
    int fileNameLen;
    char dir_path[PATH_MAX];

    snprintf(dir_path, sizeof(dir_path) - 1, "%s%s", fsPath, path);

    dp = opendir (dir_path);
    if(dp != NULL)
    {
      while(dp != NULL && (ep = readdir (dp)) != NULL) {
        if(strcmp(ep->d_name, "..")==0 || strcmp(ep->d_name, ".")==0)
          continue;

        fileNameLen = sizeof(ep->d_name)/sizeof(ep->d_name[0]);
        aarray_set(res, ep->d_name, (ep->d_type==DT_DIR ? "1" : "0"));
      }

      closedir(dp);
    }
  }
  
  AZREADDIRP ret = NULL;
  
  if (res->length > 0) {
    ret     = malloc(sizeof(AZREADDIR));
    ret->n  = res->length;
    ret->f  = malloc(sizeof(char *)*ret->n);
    ret->t  = malloc(sizeof(byte) * ret->n);
    
    for (i = 0; i < res->length; i++) {
      ret->t[i] = (strcmp(res->items[i].val, "1") == 0) ? 1 : 0;
      ret->f[i] = strdup(res->items[i].key);
    }
  }
  
  aarray_free(res);
  return ret;
}

void az_readdir_free(AZREADDIRP r) {
  if (r == NULL) {
    return;
  }
  
  int i = 0;
  
  for (i = 0; i < r->n; i++) {
    free(r->f[i]);
  }
  
  if (r->f != NULL) {
    free(r->f);
  }
  
  if (r->t != NULL) {
    free(r->t);
  }
  
  r->f = NULL;
  r->t = NULL;
  r->n = 0;
}


//-- AROMA ZIP Init
byte az_init(const char * filename, int filesystem) {
  use_filesystem = filesystem;

  if(!use_filesystem) {
    if (mzOpenZipArchive(filename, &zip) != 0) {
      return 0;
    }
  }
  else {
    fsPath = strdup(filename);
  }
  
  mkdir(AROMA_TMP, 755);
  /*
  printf("\n\n--------\n\n");
  AZREADDIRP r=az_readdir("assets/");
  if (r!=NULL){
    int i=0;
    for (i=0;i<r->n;i++){
      printf(" %s [%i]\n",r->f[i],r->t[i]);
    }
    az_readdir_free(r);
  }
  printf("\n\n--------\n\n");
  */
  return 1;
}

//-- AROMA ZIP Close
void az_close() {
  if(!use_filesystem) {
    mzCloseZipArchive(&zip);
  }
  else {
    free(fsPath);
  }
}

//-- Extract To Memory
byte az_readmem(AZMEM * out, const char * zpath, byte bytesafe) {
  char z_path[256];
  
  if(!use_filesystem) {
    snprintf(z_path, sizeof(z_path) - 1, "%s", zpath);
    const ZipEntry * se = mzFindZipEntry(&zip, z_path);

    if (se == NULL) {
      return 0;
    }

    out->sz   = se->uncompLen + (bytesafe ? 0 : 1);
    out->data = malloc(out->sz);

    //memset(out->data,0,out->sz);
    if (!mzReadZipEntry(&zip, se, out->data, se->uncompLen)) {
      free(out->data);
      return 0;
    }

    if (!bytesafe) {
      out->data[se->uncompLen] = '\0';
    }
  }
  else {
    FILE * pFile;
    unsigned long lSize;

    snprintf(z_path, sizeof(z_path) - 1, "%s%s", fsPath, zpath);
    pFile = fopen (z_path ,"rb");
    if (pFile == NULL) {
      return 0;
    }

    // get filesize
    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);

    out->sz   = lSize + (bytesafe ? 0 : 1);
    out->data = malloc(out->sz);

    if(fread(out->data, 1, lSize, pFile) != lSize) {
      free(out->data);
      return 0;
    }

    fclose (pFile);

    if (!bytesafe) {
      out->data[lSize] = '\0';
    }
  }

  return 1;
}

//-- Extract To File
byte az_extract(const char * zpath, const char * dest) {
  if(!use_filesystem) {
    const ZipEntry * zdata = mzFindZipEntry(&zip, zpath);

    if (zdata == NULL) {
      return 0;
    }

    unlink(dest);
    int fd = creat(dest, 0755);

    if (fd < 0) {
      return 0;
    }

    byte ok = mzExtractZipEntryToFile(&zip, zdata, fd);
    close(fd);
    return ok;
  }
  else {
    char c, source_file[PATH_MAX];
    FILE *source, *target;

    snprintf(source_file, sizeof(source_file) - 1, "%s%s", fsPath, zpath);

    source = fopen(source_file, "r");
    if(source == NULL) {
      return 0;
    }

    target = fopen(zpath, "w");
    if(target == NULL) {
      return 0;
    }

    do {
      c = fgetc(source);
      fputc(c, target);
    } while (c != EOF);

    fclose(source);
    fclose(target);
    return 1;
  }
}

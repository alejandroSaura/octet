////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// Text overlay
//

#define OCTET_BULLET 1

#include "../../octet.h"
#include <stdio.h>
#include <string.h>
#include "tinyXMLUtils.h"
#include "sprite.h"
#include "tileset.h"
#include "layer.h"
#include "tileEngine.h"


/// Create a box with octet
int main(int argc, char **argv) {
  // set up the platform.
  octet::app::init_all(argc, argv);

  // our application.
  octet::tileEngine app(argc, argv);
  app.init();

  // open windows
  octet::app::run_all_apps();
}



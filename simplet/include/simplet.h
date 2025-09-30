
#ifndef SIMPLET_H
#define SIMPLET_H

#include "simplet_dictionary.h"

char* simplet_render_html(const char * restrict html_template, const simplet_dictionary_t * restrict dictionary);

#endif

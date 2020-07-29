#ifndef LTOPO_XML_H
#define LTOPO_XML_H

#include "ltopo_list.h"

#define LTOOL_ADDR_SIZE 24

typedef struct struct_LTOOL_XML_NODE
{
    char addr[LTOOL_ADDR_SIZE];
    char f_addr[LTOOL_ADDR_SIZE];
    char f_b_no[LTOOL_ADDR_SIZE];
    LTOPO_LIST list;
} LTOOL_XML_NODE;


LTOOL_XML_NODE g_xmlhead;
void ltopo_print_mlist(LTOPO_LIST * head);
int ltopo_clean_mlist();



int ltopo_xml_load_file(char * xml);
int ltopo_xml_save_file(LTOPO_LIST* head, const char *filename);
int ltopo_xml_save_ev_list(const char *filename, LTOPO_LIST * head);

#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>

#include "ltopo_define.h"
#include "ltopo_xml.h"


LTOOL_XML_NODE g_xmlhead;


//创建表节点
#if 0
static xmlNodePtr xml_create_meter_node(const XML_METER *meter)
{
    char type[8] = {0};
    xmlNodePtr meter_node = NULL;

    if(!meter){
        printf(LTOOL_ERR_TAG "%s failed, meter NULL!\n", __FUNCTION__);
        return NULL;
        }
    meter_node = xmlNewNode(NULL, BAD_CAST"METER");
    if (meter_node == NULL) {
        printf(LTOOL_ERR_TAG "Failed to create new node.\n");
        return NULL;
    }
    //设置属性
    snprintf(type, sizeof(type)-1, "%d", meter->mtype);
    xmlNewChild(meter_node, NULL, BAD_CAST"TYPE", (xmlChar *)type);
    xmlNewChild(meter_node, NULL, BAD_CAST"ADDR", (xmlChar *)meter->addr);
    xmlNewChild(meter_node, NULL, BAD_CAST"F_ADDR", (xmlChar *)meter->f_addr);
    snprintf(type, sizeof(type)-1, "%d", meter->resultby);
    xmlNewChild(meter_node, NULL, BAD_CAST"RESULTBY", (xmlChar *)type);
    snprintf(type, sizeof(type)-1, "%d", meter->id);
    xmlNewChild(meter_node, NULL, BAD_CAST"ID", (xmlChar *)type);
    snprintf(type, sizeof(type)-1, "%d", meter->f_id);
    xmlNewChild(meter_node, NULL, BAD_CAST"F_ID", (xmlChar *)type);
    snprintf(type, sizeof(type)-1, "%d", meter->level);
    xmlNewChild(meter_node, NULL, BAD_CAST"LEVEL", (xmlChar *)type);
    snprintf(type, sizeof(type)-1, "%d", meter->sm_state);
    xmlNewChild(meter_node, NULL, BAD_CAST"STATUS", (xmlChar *)type);
    snprintf(type, sizeof(type)-1, "%d", meter->sm_counter);
    xmlNewChild(meter_node, NULL, BAD_CAST"COUNTER", (xmlChar *)type);

    return meter_node;
}

//向根节点中添加一个表节点
static int xml_add_meter_node_to_root(xmlNodePtr root_node, XML_METER *meter)
{
    xmlNodePtr meter_node = NULL;

    //创建一个新的表
    if (meter == NULL) {
        printf(LTOOL_ERR_TAG "%s failed, meter NULL\n", __FUNCTION__);
        return -1;
    }

    //创建一个表节点
    meter_node = xml_create_meter_node(meter);
    if (meter_node == NULL) {
        printf(LTOOL_ERR_TAG "%s, Failed to create phone node.\n", __FUNCTION__);
        return -1;
    }
    //根节点添加一个子节点
    xmlAddChild(root_node, meter_node);

    return 0;
}
//保存线路拓扑的xml文件
int ltopo_xml_save_file(LTOPO_LIST* head, const char *filename)
{
    XML_METER meter;
    xmlDocPtr doc = NULL;
    xmlNodePtr root_node = NULL;

    if(!filename){
        printf(LTOOL_ERR_TAG "%s, filename NULL\n", __FUNCTION__);
        return -1;
        }

    //remove(filename);
    //创建一个xml 文档
    doc = xmlNewDoc(BAD_CAST"1.0");
    if (doc == NULL) {
        printf(LTOOL_ERR_TAG "%s, failed to new doc.\n", __FUNCTION__);
        return -1;
    }

    //创建根节点
    root_node = xmlNewNode(NULL, BAD_CAST"ROOT");
    if (root_node == NULL) {
        printf(LTOOL_ERR_TAG "%s, failed to new root node.\n", __FUNCTION__);
        goto FAILED;
    }
    //将根节点添加到文档中
    xmlDocSetRootElement(doc, root_node);

    LTOPO_LIST * p;
    LTOPO_MBOX_NODE  *pmbox;
    list_for_each(p, head){
        pmbox=container_of(p, LTOPO_METER_NODE, list);
    
        memcpy(&meter, &pmbox->minfo, sizeof(XML_METER));
        if (xml_add_meter_node_to_root(root_node, &meter) != 0) {
            printf(LTOOL_ERR_TAG "Failed to add a new meter node.\n");
            goto FAILED;
            }
        }

    //将文档保存到文件中，按照GBK编码格式保存
    xmlSaveFormatFileEnc(filename, doc, "GBK", 1);
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return 0;
    FAILED:
    if (doc) {
        xmlFreeDoc(doc);
    }
    xmlCleanupParser();

    return -1;
}

#endif

//////////////////////////////////////////
char * ltopo_meter_para[]={"ADDR", "F_ADDR", "F_B_NO"};

int ltopo_set_meter_para(LTOOL_XML_NODE * meter, int index, char * key)
{
    switch(index)
    {
        case LTOPO_METER_PARA_ADDR:
            strncpy(meter->addr, key, LTOOL_ADDR_SIZE-1);
            break;
        case LTOPO_METER_PARA_F_ADDR:
            strncpy(meter->f_addr, key, LTOPO_ADDR_LEN-1);
            break;
        case LTOPO_METER_PARA_F_B_NO:
            strncpy(meter->f_b_no, key, LTOPO_ADDR_LEN-1);
            break;
        default:
            break;
   }
    return 0;
}
//在g_ltopo.meters尾部加入节点
int ltopo_add_meter_in_mlist(LTOOL_XML_NODE * meter)
{
    LTOOL_XML_NODE * pnew;
    pnew=malloc(sizeof(LTOOL_XML_NODE));
    if(!pnew){
        printf(LTOOL_ERR_TAG "%s, malloc LTOPO_METER_NODE failed\n", __FUNCTION__);
        return -1;
        }
    memset(pnew, 0x0, sizeof(LTOOL_XML_NODE));
    strncpy(pnew->addr, meter->addr, LTOPO_ADDR_LEN-1);
    strncpy(pnew->f_addr, meter->f_addr, LTOPO_ADDR_LEN-1);
    strncpy(pnew->f_b_no, meter->f_b_no, LTOPO_ADDR_LEN-1);

    list_add(&g_xmlhead.list, &pnew->list);
    return 0;
}
//
//打印g_ltopo.meters
void ltopo_print_mlist(LTOPO_LIST * head)
{
    LTOPO_LIST *p;
    LTOOL_XML_NODE  *pmeter;
    printf(LTOPO_TOOL "**************meters***************\n");
    list_for_each(p, head){
        pmeter=container_of(p, LTOOL_XML_NODE, list);
        printf(LTOPO_TOOL "[meter]\n");
        printf(LTOPO_TOOL "addr %s, f_addr %s, f_b_no %s\n",
            pmeter->addr, pmeter->f_addr, pmeter->f_b_no);
        }
}
void meter_fini(void *arg)
{
        LTOPO_LIST *q = (LTOPO_LIST *)arg;
        LTOOL_XML_NODE  *p = container_of(q, LTOOL_XML_NODE, list);
        p->list.next = NULL;
        free(p);
}

int ltopo_clean_mlist()
{
    list_fini(&g_xmlhead.list, meter_fini);
    return 0;
}
int ltopo_xml_load__meter(xmlDocPtr doc, xmlNodePtr cur)
{
    int j;
    xmlChar *key;
    xmlNodePtr son;
    
    LTOOL_XML_NODE meter;

    cur = cur->xmlChildrenNode;
    while (cur != NULL)
    {

            if ((!xmlStrcmp(cur->name, (const xmlChar *)"METER")))
            {
                //printf(LTOPO_TOOL "\tgot %s\n", "METER");
                son=cur->xmlChildrenNode;
                while (son != NULL)
                {
                    for(j=0;j<LTOPO_METER_PARA_MAX;j++)
                    {
                        if ((!xmlStrcmp(son->name, (const xmlChar *)ltopo_meter_para[j])))
                        {
                            key = xmlNodeListGetString(doc, son->xmlChildrenNode, 1);
                            //printf(LTOPO_TOOL "<%s>%s</%s>\n", ltopo_meter_para[j], key, ltopo_meter_para[j]);
                            if(!key){
                                printf(LTOOL_ERR_TAG "%s, NULL <%s>\n", __FUNCTION__, ltopo_meter_para[j]);
                                return -1;
                                }
                            printf("%s:%s\n", ltopo_meter_para[j], (char *)key);
                            ltopo_set_meter_para(&meter, j, (char *)key);
                            xmlFree(key);
                        }                    
                    }
                    son=son->next;
                }
                ltopo_add_meter_in_mlist(&meter);
            }
        cur = cur->next;
    }
    return 0;
}

//读取线路拓扑的xml文件
int ltopo_xml_load_file(char * xml)
{
	xmlDocPtr doc; //xml整个文档的树形结构
	xmlNodePtr cur; //xml节点	
	int ret;

    

    printf(LTOPO_TOOL "Enter %s:%s\n", __FUNCTION__, xml);	
	//获取树形结构
    doc=xmlReadFile(xml, NULL, XML_PARSE_NOBLANKS);
	//doc = xmlParseFile(xml);
	if (doc == NULL) {
		printf(LTOOL_ERR_TAG "Failed to parse xml file:%s\n", xml);
		goto FAILED;
	}
	//获取根节点
	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		printf(LTOOL_ERR_TAG "Root is empty.\n");
		goto FAILED;
	}
    ret=ltopo_xml_load__meter(doc, cur);
    if(ret!=0)
        printf(LTOPO_TOOL "%s, ltopo_xml_load__meter ret %d\n", __FUNCTION__, ret);
	xmlFreeDoc(doc);
    xmlCleanupParser();
	return 0;
FAILED:
	if (doc) {xmlFreeDoc(doc);}
    xmlCleanupParser();
	return -1;
}



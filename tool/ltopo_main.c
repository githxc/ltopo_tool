#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include "ltopo_define.h"
#include "ltopo_xml.h"


#include "pul_data.h"
#define TOOL_TEST_INPUT_PATH "../test/tool"
#define TOOL_XML_FILE TOOL_TEST_INPUT_PATH "/tool.xml"


typedef struct struct_ltool_statis {
    int s1_count;
} ltool_statis;

ltool_statis g_s;


void help()
{
	printf(LTOPO_TOOL "help:\n");
	printf(LTOPO_TOOL "./ltool <filename>\n");
}
int ltool_strstr(char * s, char * e, char ** sf, char ** ef, char * needle)
{
    char * p;
    if(!s){
        printf(LTOPO_TOOL "%s, NULL start\n", __FUNCTION__);
        return 0;
        }
    p=strstr(s, needle);
    if(!p || p > e){
        *sf=NULL;
        *ef=NULL;
        return 0;
        }
    *sf=p;
    p=strstr(p+strlen(needle), needle);
    if(!p || p > e){
        *ef=NULL;
        }
    else
        *ef=p;
    if(*ef)
        return *ef-*sf;
    else
        return e-*sf;
}

int print_len(char * s, int len)
{
    char c;
    c=s[len];
    s[len]=0;
    printf("<%s>\n", s);
    s[len]=c;
    return 0;
}

int print_to(char * s, char * e)
{
    int len;
    char * p;
    p=strstr(s, e);
    if(!p)
        return -1;
    len=p-s;
    return print_len(s, len);
}
#define LTOOL_KEYWORD_PERIOD "ltopo_alg_set_path, start at"
#define LTOOL_KEYWORD_S1 "got the S1 jump, sline"
#define LTOOL_KEYWORD_ALGRUN "ltopo_alg_run, cycle"

int ltopo_get_str_between(char * s, char * l, char * r, char *buf, int bufsize)
{
    char * p1, * p2, *p;
    int i;
    p1=strstr(s, l);
    if(!p1){
        printf("%s, can't find left, %s\n", __FUNCTION__, l);
        return -1;
        }
    p2=strstr(p1, r);
    if(!p2){
        printf("%s, can't find right, %s\n", __FUNCTION__, r);
        return -1;
        }
    if(p2-p1>bufsize-1){
        printf("%s, bufsize too small\n", __FUNCTION__);
        return -1;
        }
    for(i=0, p=p1;i<p2-p1;i++){
        buf[i]=*p;
        p++;
        }
    buf[i]=0x0;
    return 0;
        
}
LTOOL_XML_NODE * ltopo_find_xml_node(char * addr)
{
    LTOPO_LIST *p;
    LTOOL_XML_NODE  *pmeter;
    list_for_each(p, &g_xmlhead.list){
        pmeter=container_of(p, LTOOL_XML_NODE, list);
        if(!strcmp(pmeter->addr, addr)){
                return pmeter;
            }
        }
    return NULL;
}
int ltopo_atol_after(char * buf, char * anchor, long int * val)
{
    char * p;
    p=strstr(buf, anchor);
    if(!p){
        printf("%s, strstr : failed\n", __FUNCTION__); 
        return -1;
        }
    p=p+strlen(anchor);
    *val=atol(p);
    return 0;
}




long int* ltopo_proc_str(char * msg, FILE * fp)
{
    /* mt-000000001013 [0 1]: 140 140 [ 142 4437 11037 1041 ] 922 908 */
    long int v[8];
    int i;
    char buf[512];
    char * p;
 
    if(strlen(msg)>=sizeof(buf)){
        printf("%s, invalid msg %s\n", __FUNCTION__, msg);
        return NULL;
        }
    p=strstr(msg, ": ");
    if(!p){
        printf("%s, strstr : failed\n", __FUNCTION__); 
        return NULL;
        }
    p=p+strlen(": ");
    /* 140 140 [ 142 4437 11037 1041 ] 922 908*/
    printf("got p %s\n", p);
    for(i=0;i<strlen(p);i++){
        if(p[i]==']' || p[i]=='[')
            buf[i]=' ';
        else
            buf[i]=p[i];
        }
    buf[i]=0x0;
    sscanf(buf, "%ld %ld %ld %ld %ld %ld %ld %ld", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6], &v[7]);
 
	
    return v;
    
}

int ltopo_proc_s1(char * start, int len, FILE* fp)
{
    char *s1, *p;
    char mbox[80];
    char msg[512];
    char bno[8];
    int i, ret;
    LTOOL_XML_NODE  *pmeter;
    s1=malloc(len+1);
    if(!s1){
        printf("%s, malloc failed\n", __FUNCTION__);
        return -1;
        }
    memcpy(s1, start, len);
    s1[len]=0x0;

    ltopo_get_str_between(s1, "mt-", "\n", mbox, sizeof(mbox));
    printf("got %s\n", mbox);

    p=strstr(s1, "\n");

    
    pmeter=ltopo_find_xml_node(mbox);
    printf("pmeter %s, father %s, no %s\n", pmeter->addr, pmeter->f_addr, pmeter->f_b_no);
    strncpy(bno, pmeter->f_b_no, sizeof(bno)-1);
    ret=ltopo_get_str_between(p, mbox, "\n", msg, sizeof(msg));
    if(ret!=0){
        printf("can't find mbox %s, return\n", mbox);
        free(s1);
        return 0;
        }
	
    /* proc */
    /* mt-000000001013 [0 1]: 140 140 [ 142 4437 11037 1041 ] 922 908 */

    g_s.s1_count++;
    long int* datas,*d_datas;
	datas = ltopo_proc_str(msg, fp);

	int pulse,d_pulse;
	int pul_num,d_pul_num;
	pul_num = pul_detect(datas);
	pulse = pul_judg(pul_num);
	pul_out_m(pulse,msg,fp);


    while(pmeter && strcmp(pmeter->f_addr, "-1")){
		strncpy(bno, pmeter->f_b_no, sizeof(bno)-1);
        pmeter=ltopo_find_xml_node(pmeter->f_addr);
        printf("pmeter %s, father %s, no %s\n", pmeter->addr, pmeter->f_addr, pmeter->f_b_no);
        if(!strcmp(bno, "-1")){
            for(i=0;i<5;i++){
                snprintf(mbox, sizeof(mbox), "%s-%d", pmeter->addr,i+1);
                ltopo_get_str_between(p, mbox, "\n", msg, sizeof(msg));

				d_datas = ltopo_proc_str(msg,fp);
				d_pul_num = pul_detect(d_datas);
				d_pulse = pul_judg(d_pul_num);
				pul_out_d(pulse,d_pulse,msg,fp);
            }
	    }
	    else{
	  		snprintf(mbox, sizeof(mbox), "%s-%s", pmeter->addr, bno);
            ltopo_get_str_between(p, mbox, "\n", msg, sizeof(msg));

			d_datas = ltopo_proc_str(msg,fp);
			d_pul_num = pul_detect(d_datas);
			d_pulse = pul_judg(d_pul_num);
			pul_out_d(pulse,d_pulse,msg,fp);
        }
    }
    free(s1);
    return 0;
}


int ltopo_proc_period(char * start, int len, FILE * fp)
{
    int ret;
    char msg[512];
    char * s1_start, * s1_end;
    char *s1_p;
    int s1_p_len;

    s1_p=start;
    s1_p_len=len;
    while((ret=ltool_strstr(s1_p, s1_p+s1_p_len, &s1_start, &s1_end, LTOOL_KEYWORD_S1)))    {
        //printf("######got str %d!\n", ret);
        ltopo_get_str_between(s1_start, LTOOL_KEYWORD_S1, "\n", msg, sizeof(msg));
        fprintf(fp, "\n%s\n", msg);
        ltopo_proc_s1(s1_start, ret, fp);
        if(!s1_end){
            printf("***got end s1, break to the next period!\n");
            break;
            }
        s1_p_len=start+len-s1_end;
        s1_p=s1_end;
        }
    return 0;
}

int ltopo_statis(FILE * fp)
{
    fprintf(fp, "#######################\n");
    fprintf(fp, "total s1 %d\n", g_s.s1_count);
    return 0;
}


int ltopo_proc_file(char * filename, char * outfile)
{
    char * period_start, *period_end;
    char * period_p;
    char msg[512];
    int period_p_len;
    struct stat st;
    FILE *fp=NULL, *fpo=NULL;
    int ret;
    char * log=NULL;
    
    ret=stat(filename, &st);
    if(ret!=0)
    {
            printf("%s failed, stat %s failed, errno %d\n", __FUNCTION__, filename, errno);
            return -1;
    }
    printf("%s, filename len %ld\n", __FUNCTION__, st.st_size);

    log=(char * )malloc(st.st_size+1);
    if(!log)
    {
            printf("%s failed, malloc xmlbuf failed\n", __FUNCTION__);
            return -1;
    }
    fp = fopen(filename, "r");

    ret=fread(log, 1, st.st_size, fp);
    fclose(fp);
    if(ret!=st.st_size)
    {
            printf("%s failed, fread failed\n", __FUNCTION__);
            ret=-1;
            goto fin;
    }
    fpo=fopen(outfile, "wb");
    if(!fpo){
        printf("%s failed, open %s!!!\n", __FUNCTION__, outfile);
        return -1;;
        }
    
    printf("job start!\n");
    period_p=log;
    period_p_len=st.st_size;
    while((ret=ltool_strstr(period_p, period_p+period_p_len, &period_start, &period_end, LTOOL_KEYWORD_PERIOD)))    {
        //printf("****got period, len %d\n", ret);
        ltopo_get_str_between(period_start, LTOOL_KEYWORD_PERIOD, "\n", msg, sizeof(msg));
        fprintf(fpo, "%s\n", msg);
        ltopo_get_str_between(period_start, LTOOL_KEYWORD_ALGRUN, "\n", msg, sizeof(msg));
        fprintf(fpo, "%s\n", msg);
        
        printf("****got period\n");
        ltopo_proc_period(period_start, ret, fpo);
        //print_len(period_start, ret);
        if(!period_end){
            printf("***got end period,job finished!\n");
            break;
            }
        period_p=period_end;
        period_p_len=log+st.st_size-period_p;
        }
fin:
    ltopo_statis(fpo);

    if(fpo)
        fclose(fpo);
    if(log)
        free(log);
    return ret;
}
void ltool_init()
{
    memset(&g_s, 0x0, sizeof(g_s));
}
int main(int argc, char ** argv)
{
    char filename[128];

    if(argc!=2){
        help();
        return -1;
        }
    strncpy(filename, argv[1], sizeof(filename)-1);

    ltool_init();
    g_xmlhead.list.next=NULL;
    ltopo_xml_load_file(TOOL_XML_FILE);
    ltopo_print_mlist(&g_xmlhead.list);
    ltopo_proc_file(filename, "./o");
    
//
    ltopo_clean_mlist();
    return 0;
}


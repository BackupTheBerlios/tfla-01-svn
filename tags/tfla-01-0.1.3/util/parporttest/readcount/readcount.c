#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>

#include <ieee1284.h>

static bool g_stop = false;

#define SECONDS_TO_MEASURE 5

/* ---------------------------------------------------------------------------------------------- */
void sighandler(int signal)
{
    // supress compiler warning
    signal = signal;
    
    g_stop = true;
}

/* ---------------------------------------------------------------------------------------------- */
void print_pplist(struct parport_list* list)
{
    int i;
    printf("%-15.15s  %-6.6s  %-6.6s  %-20.20s\n", "Name", "Base", "Hibase", "Filename");
    for (i = 0; i < list->portc; i++)
    {
        printf("%-15.15s  0x%-4.4lX  0x%-4.4lX  %-20.20s\n", 
            list->portv[i]->name, list->portv[i]->base_addr, list->portv[i]->hibase_addr,
            list->portv[i]->filename );
    }
}

/* ---------------------------------------------------------------------------------------------- */
int main(void)
{
    int ret;
    int portno;
    struct parport* port = NULL;
    struct parport_list pplist;
    
    // signal -------------------------------------------------------------------------------------
    signal(SIGALRM, sighandler);
    
    // find the ports ------------------------------------------------------------------------------
    printf("Available ports\n");
    ret = ieee1284_find_ports(&pplist, 0);
    if (ret != E1284_OK)
    {
        printf("Failed to run ieee1284_find_ports: %d\n", ret);
        return EXIT_FAILURE;
    }
    print_pplist(&pplist);
    printf("\n\n");
    
    // use the first port
    printf("Portnummer eingeben: ");
    scanf("%d", &portno);
    getchar();
    port = pplist.portv[portno];
    
    // open ----------------------------------------------------------------------------------------
    int cap = 0;
    ret = ieee1284_open(port, 0, &cap);
    if (ret != E1284_OK)
    {
        printf("Failed to run ieee1284_open: %d\n", ret);
        return EXIT_FAILURE;
    }
    
    // claim ----------------------------------------------------------------------------------------
    ret = ieee1284_claim(port);
    if (ret != E1284_OK)
    {
        printf("Failed to run ieee1284_claim: %d\n", ret);
        return EXIT_FAILURE;
    }
    
    // direction set -------------------------------------------------------------------------------
    ret = ieee1284_data_dir(port, true);
    if (ret != E1284_OK)
    {
        printf("Failed to run ieee1284_data_dir: %d\n", ret);
        return EXIT_FAILURE;
    }
    
    alarm(SECONDS_TO_MEASURE);
    
    
    long long count = 0;
    while (!g_stop)
    {
        // read
        ieee1284_read_data(port);
        
        count++;
    }
    
    printf("Read %lld per second.\n", count/SECONDS_TO_MEASURE);
    
    return EXIT_SUCCESS;
}


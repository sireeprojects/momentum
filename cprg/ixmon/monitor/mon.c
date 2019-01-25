#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

struct thread_info {
    char name[64];
    char pid[16];
    char cpu[8];
};

int main() {
    struct thread_info tinfo[32];
    // name of the process to monitor
    char pname[] ="run.x";
    //-----------------------------------------------
    // prepare the command to execute to get the pid 
    // of the process
    //-----------------------------------------------
    char *cmd = (char*) malloc (16*sizeof(char));
    sprintf(cmd, "pidof %s", pname);
    // printf("command to execute: \'%s\'\n", cmd);
    //-----------------------------------------------
    // get the pid of the process
    //-----------------------------------------------
    char line[32];
    FILE *run_cmd = popen(cmd,"r");
    fgets(line,32,run_cmd);
    pid_t pid = strtoul(line,NULL,10);
    // printf ("pid of %s: %d\n", pname, pid);
    pclose(run_cmd);
    free(cmd);
    //-----------------------------------------------
    // get number of threads by counting the entries
    // inside task directory
    //-----------------------------------------------
    char *taskdir = (char*) malloc (32*sizeof(char));
    sprintf(taskdir, "/proc/%d/task", pid);
    // printf("taskdir: \'%s\'\n", taskdir);
    DIR *tdir;
    struct dirent *task_dir_entry;
    int thread_count=0;
    tdir = opendir(taskdir);
    char pidstr[32];
    sprintf(pidstr, "%d", pid);
    while ((task_dir_entry = readdir(tdir)) != NULL) {
        if ((strcmp(task_dir_entry->d_name, ".")!=0) && 
            (strcmp(task_dir_entry->d_name, "..")!=0) )
            {
            // (strcmp(task_dir_entry->d_name, pidstr)!=0)) 
            //-----------------
            // get thread name
            //-----------------
            char tnamefile[64];
            sprintf(tnamefile, "%s/%s/comm", taskdir, task_dir_entry->d_name);
            FILE *tname = fopen(tnamefile, "r");
            char buff[124];
            fscanf(tname, "%s", buff);
            // printf ("Thread info: pid:%s name:%s\n", task_dir_entry->d_name, buff);
            strcpy(tinfo[thread_count].pid, task_dir_entry->d_name);
            strcpy(tinfo[thread_count].name, buff);
            close(tname);
            //-------------------------
            // get thread cpu affinity
            //-------------------------
            char cpufile[64];
            char tag[1024];
            char id[64];
            sprintf(cpufile, "%s/%s/status", taskdir, task_dir_entry->d_name);
            FILE *cname = fopen(cpufile, "r");
            // Cpus_allowed_list:   1
            // scan the whole file
            int r;
            r=fscanf(cname, "%s %s\n", tag, id);
            while (r!=EOF) {
                r=fscanf(cname, "%s   %s", tag, id);
                if (strcmp(tag, "Cpus_allowed_list:")==0) {
                    // printf ("tag: %s %s\n", tag, id);
                    strcpy(tinfo[thread_count].cpu, id);
                }
            }
            thread_count++;
        } //
    }
    printf ("Total Number of threads: %d\n", thread_count);
    int x;
    for (x=0; x<thread_count; x++) {
        printf ("Name:%s Pid:%s CPU:%s\n",
                tinfo[x].name,
                tinfo[x].pid,
                tinfo[x].cpu
                );
    }
    closedir(tdir);
    free(taskdir);
    return 0;
}














    // //-----------------------------------------------
    // // derive the prox/pid directory
    // //-----------------------------------------------
    // char *proc_dir = (char*) malloc (32*sizeof(char));
    // sprintf(proc_dir, "/proc/%d", pid);
    // printf("directory to monitor: \'%s\'\n", proc_dir);
    // // get the content of the proc directory
    // DIR *pdir;
    // struct dirent *proc_dir_entry;
    // pdir = opendir(proc_dir);
    // if (pdir==NULL) {
    //     printf ("Cannot open directory '%s'\n", proc_dir);
    //     return 0;
    // }

    // while ((proc_dir_entry = readdir(pdir)) != NULL) {
    //     printf ("%s\n", proc_dir_entry->d_name);
    // }
    // closedir(pdir);
    // free(proc_dir);


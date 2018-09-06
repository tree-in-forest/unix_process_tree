#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

int
spawn_proc_tree(int *dfs_tree, int start, int before, int after, int chlds, int numel_tree){
    
//    printf("chlds: %d, before: %d, after: %d, start: %d\n", chlds, before, after, start);
    if(chlds < 1)
        return 0;
    
    int j, new_before = 0, new_after = 0, new_size = 0, curr_chlds, status;
    
    for(j = start; j < numel_tree && j < start + before + chlds + after; j++){
        if(j < start + before)
            new_before += dfs_tree[j];
        
        new_size += dfs_tree[j];
    }
    new_after = new_size - new_before;

    for(j = start + before; j < start + before + chlds; j++){
        if (j < numel_tree)
            curr_chlds = dfs_tree[j];
        else
            curr_chlds = 0;

        new_after -= curr_chlds;

        if(fork() == 0){
            spawn_proc_tree(dfs_tree, start + before + chlds + after, new_before,
                    new_after, curr_chlds, numel_tree);
            wait(&status);
            sleep(5);
            exit(status);
        }

        new_before +=curr_chlds;
    }
    return 0;
}

int
main(int argc, char **argv){
    if (argc < 2){
        return 0;
    }
    int size = argc - 1;
    int *tree = (int *)malloc((argc - 1) * sizeof(int));
    int i, exit_code;
    char c_root_pid[16];

    sprintf(c_root_pid, "%d", getpid());

    for(i = 0; i < size; i++){
        tree[i] = atoi(argv[i + 1]);
//        printf("%d\n", tree[i]);
    }
    
    spawn_proc_tree(tree, 1, 0, 0, tree[0], size);

    if(fork() == 0){
        sleep(1);
        execlp("pstree", "/home", c_root_pid, "-c", NULL);
    }else{
        sleep(2);
        wait(&exit_code);
    }
    return exit_code;
}

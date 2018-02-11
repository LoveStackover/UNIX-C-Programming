#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <err.h>
#include <pthread.h>
#include <string.h>

extern char **environ;
pthread_mutex_t env_mutex;
pthread_once_t init_done = PTHREAD_ONCE_INIT;
void thread_init(void)
{
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&env_mutex, &attr);
		pthread_mutexattr_destroy(&attr);
}


int putenv_r(char *string)
{
    int i, len,len2;
    sigset_t new,old;
    sigemptyset(&old);
    sigfillset(&new);
    if(sigprocmask(SIG_SETMASK,&new,&old) != 0)
        return -1;
    pthread_once(&init_done, thread_init);
    len2=strlen(string);
    for (i = 0; string[i] == '='; i++);
    len=i;
    pthread_mutex_lock(&env_mutex);
    for (i = 0; environ[i] != NULL; i++)
    {
        if (strncmp(string, environ[i],len) == 0)
        {
                strncpy(&environ[i][len],&string[len], len2+1-len);
                pthread_mutex_unlock(&env_mutex);
                if(sigprocmask(SIG_SETMASK,&old,NULL) != 0)
                        return -1;
                return 0;
        }
    }
    strncpy(environ[i],string, len2+1);
    pthread_mutex_unlock(&env_mutex);
    if(sigprocmask(SIG_SETMASK,&old,NULL) != 0)
        return -1;
    return 0;
}


int main(void)
{
    char * buffer="LoverStackover=is a good boy";
    int err;
    if((err = putenv_r(buffer)) != 0)
        errx(1,"error in putenv_r \n");
    if((buffer = getenv("LoverStackover")) == NULL)
        errx(1,"error in getenv\n");
    printf("%s\n",buffer);
    return 0;
}

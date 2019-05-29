#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <stdio.h>

int main() {
   int pid = fork();

   if (pid == 0) {
      // може да се опитаме да изпълним "/bin/fail-for-sure, за да ни fail-не execl и
      // да exit-нем с 77
      if (execl("/bin/sleep", "sleepChild", "20", (char*) NULL) == -1) {
         errx(77, "whops");
      }
   }

   printf("Child is with pid: %d. Maybe kill it?\n", pid);

   int status;
   wait(&status);

   if (WIFEXITED(status)) {
      // детето е приключило нормално (в смисъл, че не е било убито от сигнал, а просто
      // е завършило изпълнението си; това включва и завършвания с exit code различен от 0)
      
      printf("Child finished with exit code: %d\n", WEXITSTATUS(status));
   } else if (WIFSIGNALED(status)) {
      // детето е приключило заради сигнал
      // лесно може да симулираме това като kill-нем детето
      printf("Child was terminated by signal: %d\n", WTERMSIG(status));
   } else {
      // нещо друго от описаните в man 2 wait се е случило
      printf("This is going too far\n");
   }
}

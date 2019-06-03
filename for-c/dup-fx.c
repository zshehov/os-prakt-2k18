#include <unistd.h>
#include <fcntl.h>
#include <err.h>

int main() {
   int devnull = open("/dev/null", O_WRONLY);
   if (devnull == -1) {
      errx(5, "Couldn't find /dev/null");
   }

   // заменяме fd 2(stderr) да сочи към /dev/null
   dup2(devnull, 2);

   int pipefd[2];

   if (pipe(pipefd) == -1) {
      errx(55, "Pipe didn't work");
   }

   // заменяме fd 0(stdin) да бъде краят за четене на pipe-a
   // т.е. когато някой чете така:
   // read(0, &some_buffer, 10);
   // всъщност ще чете от pipe-a
   // правим го понеже програмите, които exec-ваме, нямат понятие за нашия pipe,
   // a ще четат от fd 0(stdin)
   dup2(pipefd[0], 0);

   // със същите съображения заменяме fd 1(stdout) да бъде краят за писане на pipe-a
   dup2(pipefd[1], 1);

   // точно за sleep няма много значение, че пренасочихме входа и изхода,
   // но пък може да погледнем какво е станало в /proc
   if (execlp("sleep", "WeirdSleep", "100", (char*) NULL) == -1) {
      errx(12, "Exec didn't execute");
   }

   // $ ps -e -o cmd,pid | grep Wei[r]d   -> взимаме pid-то
   // $ ls -l /proc/pid/fd
   //
   // total 0
   // lr-x------ 1 ryuk shinigami 64 Jun  2 00:23 0 -> pipe:[5240503]
   // l-wx------ 1 ryuk shinigami 64 Jun  2 00:23 1 -> pipe:[5240503]
   // l-wx------ 1 ryuk shinigami 64 Jun  2 00:23 2 -> /dev/null
   // l-wx------ 1 ryuk shinigami 64 Jun  2 00:23 3 -> /dev/null
   // lr-x------ 1 ryuk shinigami 64 Jun  2 00:23 4 -> pipe:[5240503]
   // l-wx------ 1 ryuk shinigami 64 Jun  2 00:23 5 -> pipe:[5240503]
   //
   // ^ 
   // 1) на 3 отворихме /dev/null
   // 2) създадохме pipe, на който се паднаха файл дескриптори 4 за четене и 5 за писане
   // 3) излъгахме fd 0 вече да е краят за четене от pipe-a, и fd 1 да е краят за писане
}
